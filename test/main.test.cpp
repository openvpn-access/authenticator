#include <gtest/gtest.h>
#include "main.hpp"

TEST(Authenticator, VerifyPassword)
{
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    int ret;

    ret = bcrypt_gensalt(12, salt);
    ASSERT_EQ(ret, 0) << "Should generate salt.";

    ret = bcrypt_hashpw("password", salt, hash);
    ASSERT_EQ(ret, 0) << "Should hash password.";

    user_info test_user;
    test_user.username = "user";
    test_user.password = "wrong_password";

    test_user.hash = hash;

    ASSERT_EXIT({ verify_password(test_user); }, ::testing::ExitedWithCode(1), "") << "Should not allow false passwords";

    test_user.password = "password";

    verify_password(test_user);
}

TEST(Authenticator, DoExit)
{
    ASSERT_EXIT({ do_exit(0); }, ::testing::ExitedWithCode(0), "") << "Should exit with code 0.";
    ASSERT_EXIT({ do_exit(1); }, ::testing::ExitedWithCode(1), "") << "Should exit with code 1.";
}

TEST(Authenticator, ParseConfig)
{
    ASSERT_EXIT({ parse_config("/dev/null"); }, ::testing::ExitedWithCode(1), "") << "Should fail on non-existent config file.";
    parse_config(application_executable_path);
}