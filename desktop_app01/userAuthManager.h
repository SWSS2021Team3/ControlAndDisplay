#pragma once
#include <string>
#include "commManager.h"

using namespace std;

class UserAuthManager {
private:
	CommManagerInterface* commManager = nullptr;
public:
	UserAuthManager(CommManagerInterface* comm) : commManager(comm) {}
	bool login(const string& username, const string& password);
	void logout();
};
