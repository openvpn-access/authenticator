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

#ifdef TESTING
    #define CONFIG_FILE_NAME "authenticator_test.yml"
    extern const char* application_executable_path;
#else
    #define CONFIG_FILE_NAME "authenticator.yml"
#endif

typedef struct {
    std::string username;
    std::string password;
    std::string hash;
    std::string user_type;
    unsigned long expiry;
} user_info;

extern std::thread time_ctrl_thread;

void do_exit(int code);
void log(const std::string& log_string, unsigned int code = DBG_LOG);
void populate_user_credentials(const std::string& credentials_path, user_info &user);
void query_user(mariadb::connection_ref &connection, user_info &user);
void verify_password(user_info& user);
void time_ctrl();
void authenticate_user(user_info& user, const YAML::Node& config);

mariadb::connection_ref connect(const YAML::Node &config);
YAML::Node parse_config(const char* executable_path);

#endif //OPENVPN_ACCESS_AUTHENTICATOR_MAIN_HPP
