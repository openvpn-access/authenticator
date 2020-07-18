#include "main.hpp"

/**
 * Performs standard user authentication.
 * @param user User object with `username` and `password` already populated.
 * @param config Parsed config file node.
 */
void authenticate_user(std::shared_ptr<User>& user)
{
    log("Checking provided password against the user's hash...");
    user->verify_password();

    log("Checking to see if user is an admin or friend...");
    if(user->user_type == "admin")
    {
        log("Auth accepted - Admin", AUTH_ACCEPT);
        do_exit(0);
    }
    else
    {
        log("User is neither an admin or a friend...");
    }

    log("Checking email verification status...");
    if(!user->email_verified)
    {
        log("Auth denied - Email not verified", AUTH_REJECT_EMAIL_NONVER);
        do_exit(1);
    }

    if(user->user_state != "activated")
    {
        log("Auth denied - User not activated", AUTH_REJECT_NOT_ACTIVATED);
        do_exit(1);
    }

    log("Auth accepted - User is valid", AUTH_ACCEPT);
    do_exit(0);
}