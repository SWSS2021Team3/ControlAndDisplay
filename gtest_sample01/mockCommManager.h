#pragma once

#include "pch.h"
#include <opencv2/opencv.hpp>
#include "../desktop_app01/commManager.h"

using namespace std;

class MockCommManager : public CommManagerInterface {
public:
	MOCK_METHOD(bool, connect, (const bool secureMode), (override));
	MOCK_METHOD(User, login, (const string& username, const string& password), (override));
	MOCK_METHOD(bool, requestFaces, (const int uid), (override));
	MOCK_METHOD(bool, requestAddFace, (const int uid, const int numberOfImages), (override));
	MOCK_METHOD(void, disconnect, (), (override));
};