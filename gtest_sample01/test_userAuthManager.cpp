#include "pch.h"
#include "../desktop_app01/commManager.h"
#include "../desktop_app01/userAuthManager.h"

using namespace std;
using testing::Return;
using testing::_;

class MockCommManager : public CommManagerInterface {
public:
	MOCK_METHOD(bool, connect, (), (override));
	MOCK_METHOD(bool, login, (const string& username, const string& password), (override));
};
TEST(Login, login_success_with_correct_password) {
	std::string username = "user1";
	std::string correctPassword = "prngj9tug1";

	MockCommManager commManager;
	EXPECT_CALL(commManager, connect()).WillOnce(Return(true));
	EXPECT_CALL(commManager, login(username, correctPassword)).WillOnce(Return(true));

	UserAuthManager uam(&commManager);

	EXPECT_TRUE(uam.login(username, correctPassword));
}

TEST(Login, login_failed_with_incorrect_password) {
	std::string username = "user1";
	std::string incorrectPassword = "badpass1";

	MockCommManager commManager;
	EXPECT_CALL(commManager, connect()).WillOnce(Return(true));
	EXPECT_CALL(commManager, login(username, incorrectPassword)).WillOnce(Return(false));

	UserAuthManager uam(&commManager);

	EXPECT_FALSE(uam.login(username, incorrectPassword));
}

TEST(Login, login_failed_when_connection_failed) {
	MockCommManager commManager;
	EXPECT_CALL(commManager, connect()).WillOnce(Return(false));
	EXPECT_CALL(commManager, login(_, _)).Times(0);

	UserAuthManager uam(&commManager);

	EXPECT_FALSE(uam.login("user1", "pwd"));
}