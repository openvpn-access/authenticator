#include <gtest/gtest.h>
#include "main.hpp"

TEST(User, RejectsBadPasswords)
{
    auto user = std::make_shared<User>();
    user->password = "password";

    char salt[BCRYPT_HASHSIZE];
    bcrypt_gensalt(12, salt);

    char hash[BCRYPT_HASHSIZE];
    bcrypt_hashpw("not the same password", salt, hash);

    user->hash = hash;

    ASSERT_EXIT({ user->verify_password(); }, ::testing::ExitedWithCode(1), "") << "Should reject bad passwords";
}

TEST(User, AcceptsGoodPasswords)
{
    auto user = std::make_shared<User>();
    user->password = "password";

    char salt[BCRYPT_HASHSIZE];
    bcrypt_gensalt(12, salt);

    char hash[BCRYPT_HASHSIZE];
    bcrypt_hashpw("password", salt, hash);

    user->hash = hash;
}