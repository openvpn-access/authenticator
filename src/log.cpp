#include "main.hpp"

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