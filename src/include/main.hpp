#ifndef OPENVPN_ACCESS_AUTHENTICATOR_MAIN_HPP
#define OPENVPN_ACCESS_AUTHENTICATOR_MAIN_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <filesystem>
#include <random>

#include <mariadb++/connection.hpp>
#include <yaml-cpp/yaml.h>
#include <bcrypt.h>

#include "log.hpp"

#define TIME_ATTACK_DEFENSE_TIMER_MS 500

class User {
private:
    mariadb::connection_ref connection;

public:
    std::string username;
    std::string password;
    std::string hash;
    std::string user_type;
    std::string user_state;
    bool email_verified;
    int id;

    void verify_password() const;

    User(const std::string& credentials_path, mariadb::connection_ref connection);
    User() = default;
    ~User();
};

extern std::thread time_ctrl_thread;

void do_exit(int code);
void log(const std::string& log_string, unsigned int code = DBG_LOG);
void time_ctrl();
void authenticate_user(std::shared_ptr<User>& user);

mariadb::connection_ref connect(const YAML::Node &config);
YAML::Node parse_config(const char* config_path);

#endif //OPENVPN_ACCESS_AUTHENTICATOR_MAIN_HPP
