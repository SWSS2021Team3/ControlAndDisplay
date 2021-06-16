#pragma once
#include <string>
#include "commManager.h"
#include "viewHandler.h"
#include "user.h"
#include "listener.h"

using namespace std;

class UserAuthManagerInterface
{
public:
	virtual User getCurrentUser() = 0;
};

class UserAuthManager : public UserAuthManagerInterface, UserListener {
private:
	CommManagerInterface* commManager = nullptr;
	User currentUser;

	UserAuthViewHandler* viewHandler = nullptr;

public:
	UserAuthManager(CommManagerInterface* comm);
	bool login(const string& username, const string& password, const bool secureMode);
	void logout();
	User getCurrentUser();

	void setViewHandler(UserAuthViewHandler* vh);

	void onUserLogin(bool success, int uid, std::string s);
};
