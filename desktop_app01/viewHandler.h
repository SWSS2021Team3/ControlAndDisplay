#pragma once
#include "user.h"

class StudentViewHandler
{
public:
	virtual void onFaceImageUpdate() = 0;
};

class UserAuthViewHandler
{
public:
	virtual void onConnectFailed() = 0;
	virtual void onLoginFailed() = 0;
	virtual void onLoginSuccess(User user) = 0;
};
