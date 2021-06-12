#pragma once

#include "pch.h"
#include <opencv2/opencv.hpp>
#include "../desktop_app01/commManager.h"

using namespace std;

class MockCommManager : public CommManagerInterface {
public:
	MOCK_METHOD(bool, connect, (), (override));
	MOCK_METHOD(User, login, (const string& username, const string& password), (override));
	MOCK_METHOD(bool, requestFaces, (const int uid, const int numberOfImages, vector<cv::Mat>& faces), (override));
	MOCK_METHOD(void, disconnect, (), (override));
};