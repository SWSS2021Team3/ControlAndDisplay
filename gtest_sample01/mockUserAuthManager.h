#pragma once

#include "pch.h"
#include "../ACS/userAuthManager.h"

using namespace std;

class MockUserAuthManager : public UserAuthManagerInterface {
public:
	MOCK_METHOD(User, getCurrentUser, (), (override));
};