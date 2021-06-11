#pragma once

#include "pch.h"
#include "../desktop_app01/userAuthManager.h"

using namespace std;

class MockUserAuthManager : public UserAuthManagerInterface {
public:
	MOCK_METHOD(User, getCurrentUser, (), (override));
};