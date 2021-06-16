#include "pch.h"
#include "../desktop_app01/userAuthManager.h"
#include "../desktop_app01/user.h"

#include "mockCommManager.h"

using namespace std;
using testing::Return;
using testing::_;

TEST(Login, login_success_with_correct_password) {
	std::string username = "user1";
	std::string correctPassword = "prngj9tug1";
	User correctUser = { 501, username };

	MockCommManager commManager;
	EXPECT_CALL(commManager, connect(false)).WillOnce(Return(true));
	EXPECT_CALL(commManager, login(username, correctPassword)).WillOnce(Return(false));

	UserAuthManager uam(&commManager);

	EXPECT_TRUE(uam.login(username, correctPassword, false));
	EXPECT_EQ(uam.getCurrentUser().uid, correctUser.uid);
	EXPECT_EQ(uam.getCurrentUser().username, correctUser.username);
}

TEST(Login, login_failed_with_incorrect_password) {
	std::string username = "user1";
	std::string incorrectPassword = "badpass1";
	User notUser = { -1, username };

	MockCommManager commManager;
	EXPECT_CALL(commManager, connect(false)).WillOnce(Return(true));
	EXPECT_CALL(commManager, login(username, incorrectPassword)).WillOnce(Return(false));

	UserAuthManager uam(&commManager);

	EXPECT_FALSE(uam.login(username, incorrectPassword, false));
}

TEST(Login, login_failed_when_connection_failed) {
	MockCommManager commManager;
	EXPECT_CALL(commManager, connect(false)).WillOnce(Return(false));
	EXPECT_CALL(commManager, login(_, _)).Times(0);

	UserAuthManager uam(&commManager);

	EXPECT_FALSE(uam.login("user1", "pwd", false));
}

TEST(Logout, should_be_disconnect_when_logout) {
	MockCommManager commManager;
	EXPECT_CALL(commManager, disconnect());

	UserAuthManager uam(&commManager);

	uam.logout();
}

