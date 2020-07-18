#include "main.hpp"

/**
 * Check the provided user password string against the hash from the database and exit(1) if not the same.
 */
void User::verify_password() const
{
    // Check password
    if(bcrypt_checkpw(this->password.c_str(), (const char*) this->hash.c_str()) != 0)
    {
        log("Provided password does not match password hash!", AUTH_REJECT_BAD_PASSWD);
        do_exit(1);
    }
}

User::User(const std::string& credentials_path, mariadb::connection_ref connection)
{
    this->connection = std::move(connection);

    log("Opening OpenVPN user credentials file...");
    try
    {
        std::fstream credentials(credentials_path, std::ios::in);

        if (!credentials.is_open()) {
            log("Unable to open OpenVPN credentials file!", ERR_BAD_CRED_FILE);
            do_exit(1);
        }

        log("Getting username and password...");
        getline(credentials, this->username);
        getline(credentials, this->password);

        if (this->username.empty() || this->password.empty()) {
            log("Empty username or password!", AUTH_REJECT_EMPTY_CRED);
            do_exit(1);
        }
    }
    catch(...)
    {
        log("File IO error with opening OpenVPN credentials file!", ERR_FILE_IO);
        do_exit(1);
    }

    log("Getting user information from the database...");
    try
    {
        auto stmt = this->connection->create_statement("SELECT id, type, state, password, email_verified FROM user WHERE username = ?");
        stmt->set_string(0, this->username);

        auto res = stmt->query();

        if (res->next()) {
            this->id = res->get_signed32("id");
            this->hash = res->get_string("password");
            this->user_type = res->get_string("type");
            this->user_state = res->get_string("state");
            this->email_verified = res->get_boolean("email_verified");
        } else {
            this->connection->disconnect();
            log("Error querying MariaDB!", ERR_MARIADB_QUERY);
            do_exit(1);
        }
    }
    catch(...)
    {
        log("Error querying MariaDB!", ERR_MARIADB_QUERY);
        do_exit(1);
    }
}

User::~User()
{
    if(this->connection)
    {
        this->connection->disconnect();
    }
}