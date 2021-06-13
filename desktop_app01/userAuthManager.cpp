#include "userAuthManager.h"

bool UserAuthManager::login(const string& username, const string& password, const bool secureMode)
{
	if (commManager == nullptr)
	{
		viewHandler->onConnectFailed();
		return false;
	}
	if (!commManager->connect(secureMode))
	{
		viewHandler->onConnectFailed();
		return false;
	}
	User user = commManager->login(username, password);

	if (user.uid < 0)
	{
		commManager->disconnect();
		viewHandler->onLoginFailed();
		return false;
	}

	currentUser = user;
	viewHandler->onLoginSuccess(user);

	return true;
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