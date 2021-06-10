#include "userAuthManager.h"

bool UserAuthManager::login(const string& username, const string& password)
{
	if (commManager == nullptr) return false;
	if (!commManager->connect()) return false;
	return commManager->login(username, password);
}

void UserAuthManager::logout()
{
	if (commManager == nullptr) return;
	commManager->disconnect();
}