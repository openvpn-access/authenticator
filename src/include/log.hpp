#ifndef OPENVPN_ACCESS_AUTHENTICATOR_LOG_HPP
#define OPENVPN_ACCESS_AUTHENTICATOR_LOG_HPP

#include <string>

#define SILENT      00000
#define VERB_1      01000
#define VERB_2      02000
#define VERB_3      03000

#define VERBOSITY   07000

#define NOERR       00000
#define ERROR       00100

#define ERR_BAD_ARG_COUNT           VERB_1 | ERROR | 001
#define ERR_BAD_CRED_FILE           VERB_1 | ERROR | 002
#define ERR_BAD_CONFIG_FILE         VERB_1 | ERROR | 006
#define ERR_INVALID_CONFIG_FILE     VERB_1 | ERROR | 014
#define ERR_MARIADB_QUERY           VERB_1 | ERROR | 011
#define ERR_FILE_IO                 VERB_1 | ERROR | 012
#define ERR_MARIADB_CONNECTION      VERB_1 | ERROR | 013

#define AUTH_ACCEPT                 VERB_2 | NOERR | 003

#define AUTH_REJECT_EMPTY_CRED      VERB_2 | ERROR | 005
#define AUTH_REJECT_BAD_PASSWD      VERB_2 | ERROR | 007
#define AUTH_REJECT_EMAIL_NONVER    VERB_2 | ERROR | 011
#define AUTH_REJECT_NOT_ACTIVATED   VERB_2 | ERROR | 012

#define DBG_LOG                     VERB_3 | NOERR | 004

extern unsigned int log_verbosity;
extern bool file_logging_enabled;
extern std::string log_file_path;

#endif //OPENVPN_ACCESS_AUTHENTICATOR_LOG_HPP
