#include "main.hpp"

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