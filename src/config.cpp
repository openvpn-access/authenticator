#include "main.hpp"

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