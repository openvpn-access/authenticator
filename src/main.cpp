#include "main.hpp"

#ifdef TESTING
    #include <gtest/gtest.h>
    const char* application_executable_path;
#endif

// Global vars
std::thread time_ctrl_thread;
unsigned int log_verbosity = VERB_1;
bool file_logging_enabled = false;
std::string log_file_path;

/**
 * Exit with the specified code after waiting for the time control thread to finish.
 * @param code Code to exit with.
 */
void do_exit(int code)
{
    if(time_ctrl_thread.joinable())
    {
        time_ctrl_thread.join();
    }

    std::exit(code);
}

/**
 * Log to the console (`stdout`/`stderr`) as well as to any log file specified by the configuration.
 * @param log_string String to log.
 * @param code Logging code. See `include/log.hpp` for more information.
 */
void log(const std::string& log_string, unsigned int code)
{
    if((code & VERBOSITY) <= log_verbosity)
    {
        std::ostream &out_stream = (code & ERROR) ? std::ref(std::cerr) : std::ref(std::cout);

        std::time_t current_time = std::time(nullptr);
        char* formatted_time = std::asctime(std::localtime(&current_time));
        formatted_time[strlen(formatted_time) - 1] = '-';

        std::string formatted_log_string = formatted_time + std::to_string(code) + '\t' + log_string;

        out_stream << formatted_log_string << std::endl;

        if(file_logging_enabled)
        {
            std::fstream log_file_stream(log_file_path, std::ios::app);

            log_file_stream << formatted_log_string << std::endl;
        }
    }
}

/**
 * Connect to MariaDB instance.
 * @param config Database config with all required data to connect to the MariaDB server (`host`, `user`, `password`, `database`).
 * @return Connection to the MariaDB database.
 */
mariadb::connection_ref connect(const YAML::Node& config)
{
    try
    {
        auto host = config["host"].as<std::string>();
        auto username =  config["user"].as<std::string>();
        auto password = config["password"].as<std::string>();
        auto database = config["database"].as<std::string>();

        auto account = mariadb::account::create(host, username, password, database);
        auto connection = mariadb::connection::create(account);

        return connection;
    }
    catch(...)
    {
        log("Unable to create connection to MariaDB", ERR_MARIADB_CONNECTION);
        do_exit(1);
    }

    return nullptr;
}

/**
 * Parse config, checking variables and populating global variables.
 *
 * @todo Also search in /etc/openvpn-access-authenticator
 *
 * @param executable_path Path to the `authenticator` executable. The config is assumed to be `authenticator.yml` in the same directory as the executable.
 * @return The parsed YAML::Node.
 */
YAML::Node parse_config(const char* executable_path)
{
    std::filesystem::path config_path = executable_path;

    YAML::Node config;

    try
    {
        config = YAML::LoadFile((config_path.remove_filename() / CONFIG_FILE_NAME).c_str());
    }
    catch(...)
    {
        log("Couldn't find configuration file!", ERR_BAD_CONFIG_FILE);
        do_exit(1);
    }

    if(!config["db"].IsDefined())
    {
        log("Configuration doesn't contain DB credentials!", ERR_INVALID_CONFIG_FILE);
        do_exit(1);
    }

    if(config["log"].IsDefined())
    {
        auto log_config = config["log"].as<YAML::Node>();

        if(log_config["verb"].IsDefined())
        {
            log_verbosity = log_config["verb"].as<unsigned int>();
        }

        if(log_config["file"].IsDefined())
        {
            log_file_path = log_config["file"].as<std::string>();
        }

        file_logging_enabled = !log_file_path.empty();
    }

    return config;
}

/**
 * Open credentials file and populate `user` with the credentials given to us by OpenVPN.
 * @param credentials_path Path to the file that OpenVPN passes to us.
 * @param user User to populate.
 */
void populate_user_credentials(const std::string& credentials_path, user_info &user)
{
    try {
        std::fstream credentials(credentials_path, std::ios::in);

        if (!credentials.is_open()) {
            log("Unable to open OpenVPN credentials file!", ERR_BAD_CRED_FILE);
            do_exit(1);
        }

        log("Getting username and password...");
        getline(credentials, user.username);
        getline(credentials, user.password);

        if (user.username.empty() || user.password.empty()) {
            log("Empty username or password!", AUTH_REJECT_EMPTY_CRED);
            do_exit(1);
        }
    }
    catch(...)
    {
        log("File IO error with opening OpenVPN credentials file!", ERR_FILE_IO);
        do_exit(1);
    }
}

/**
 * Find user through database connection ref and populate subsequent user object.
 * @param connection Connection to MariaDB database.
 * @param user User that we are searching for.
 */
void query_user(mariadb::connection_ref &connection, user_info &user)
{
    try
    {
        auto stmt = connection->create_statement("SELECT id, hash, user_type, expiry FROM users WHERE username = ?");
        stmt->set_string(0, user.username);

        auto res = stmt->query();

        if (res->next()) {
            user.hash = res->get_string("hash");
            user.user_type = res->get_string("user_type");
            user.expiry = res->get_unsigned32("expiry");
        } else {
            connection->disconnect();
            do_exit(1);
        }
    }
    catch(...)
    {
        log("Error querying MariaDB!", ERR_MARIADB_QUERY);
        do_exit(1);
    }
}

/**
 * Check the provided user password string against the hash from the database and exit(1) if not the same.
 * @param user User of which to verify password.
 */
void verify_password(user_info& user)
{
    // Check password
    if(bcrypt_checkpw(user.password.c_str(), (const char*) user.hash.c_str()) != 0)
    {
        log("Provided password does not match password hash!", AUTH_REJECT_BAD_PASSWD);
        do_exit(1);
    }
}

/**
 * Thread function to ensure random runtimes for the application to help prevent time attacks.
 * Program will run for at least a random number between 1-TIME_ATTACK_DEFENSE_TIMER_MS ms.
 */
void time_ctrl()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,TIME_ATTACK_DEFENSE_TIMER_MS);

    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
}

/**
 * Performs standard user authentication.
 * @param user User object with `username` and `password` already populated.
 * @param config Parsed config file node.
 */
void authenticate_user(user_info& user, const YAML::Node& config)
{
    log("Starting connection to database...");
    YAML::Node db_config = config["db"];
    auto connection = connect(db_config);

    log("Querying user for user type and hash...");
    query_user(connection, user);

    connection->disconnect();

    log("Checking provided password against the user's hash...");
    verify_password(user);

    log("Checking to see if user is an admin or friend...");
    if(user.user_type == "admin" || user.user_type == "friend")
    {
        log("Auth accepted - Admin or friend!", AUTH_ACCEPT);
        do_exit(0);
    }
    else
    {
        log("User is neither an admin or a friend...");
    }

    log("Checking user's expiry time...");
    const auto current_time = std::chrono::system_clock::now();
    unsigned long current_unix_time = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch()).count();
    if(user.expiry >= current_unix_time)
    {
        log("Auth accepted - Valid user account!", AUTH_ACCEPT);
        do_exit(0);
    }
    else
    {
        log("User's account has an invalid expiry time...");
    }

    log("Auth denied - User passed no authentication strategies!", AUTH_REJECT_EOF);
    do_exit(1);
}

#ifndef TESTING
int main(int argc, char** argv) {
    // Launch wait thread
    time_ctrl_thread = std::thread(time_ctrl);

    // Parse config
    log("Parsing config file...");
    YAML::Node config = parse_config(argv[0]);

    // Start auth
    log("Starting authentication...");

    if(argc < 2)
    {
        log("No credentials file provided!", ERR_BAD_ARG_COUNT);
        do_exit(1);
    }

    // Create user
    user_info user;

    // Get credentials passed by OpenVPN
    log("Opening OpenVPN user credentials file...");
    populate_user_credentials(argv[1], user);

    // Authenticate
    authenticate_user(user, config);
}
#else
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    application_executable_path = argv[0];

    return RUN_ALL_TESTS();
}
#endif
