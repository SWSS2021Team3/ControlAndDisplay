#pragma once
#include <string>

using namespace std;

class CommManagerInterface {
public:
	virtual bool connect() = 0;
	virtual bool login(const string& username, const string& password) = 0;
};
