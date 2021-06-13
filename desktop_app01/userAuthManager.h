#pragma once
#include <string>
#include "commManager.h"
#include "viewHandler.h"
#include "user.h"

using namespace std;

class UserAuthManagerInterface
{
public:
	virtual User getCurrentUser() = 0;
};

class UserAuthManager : public UserAuthManagerInterface {
private:
	CommManagerInterface* commManager = nullptr;
	User currentUser;

	UserAuthViewHandler* viewHandler = nullptr;

public:
	UserAuthManager(CommManagerInterface* comm) : commManager(comm) {}
	bool login(const string& username, const string& password);
	void logout();
	User getCurrentUser();

	void setViewHandler(UserAuthViewHandler* vh);
};
