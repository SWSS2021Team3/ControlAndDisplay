#include "userAuthManager.h"

UserAuthManager::UserAuthManager(CommManagerInterface* comm) : commManager(comm)
{
	comm->setUserListener(this);
}

bool UserAuthManager::login(const string& username, const string& password, const bool secureMode)
{
	if (commManager == nullptr)
	{
		if (viewHandler)
			viewHandler->onConnectFailed();
		return false;
	}
	if (!commManager->connect(secureMode))
	{
		if (viewHandler)
			viewHandler->onConnectFailed();
		return false;
	}
	return commManager->login(username, password);
}

void UserAuthManager::logout()
{
	if (commManager == nullptr) return;
	commManager->disconnect();
}

User UserAuthManager::getCurrentUser()
{
	return currentUser;
}

void UserAuthManager::setViewHandler(UserAuthViewHandler* vh)
{
	viewHandler = vh;
}

void UserAuthManager::onUserLogin(bool success, int uid, std::string s)
{
	if (!success)
	{
		commManager->disconnect();
		if (viewHandler)
			viewHandler->onLoginFailed();
		return;
	}

	currentUser.uid = uid;
	currentUser.username = s;
	if (viewHandler)
		viewHandler->onLoginSuccess(currentUser);
}
