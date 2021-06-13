#include "userAuthManager.h"

bool UserAuthManager::login(const string& username, const string& password)
{
	if (commManager == nullptr) return false;
	if (!commManager->connect()) return false;
	User user = commManager->login(username, password);
	// TODO: login fail -> disconnect ?
	if (user.uid < 0) return false;

	currentUser = user;
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