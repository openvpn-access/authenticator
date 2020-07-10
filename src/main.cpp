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
    log("Starting connection to database...");
    YAML::Node db_config = config["db"];
    auto connection = connect(db_config);

    log("Creating user...");
    auto user = std::make_shared<User>(argv[1], connection);

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