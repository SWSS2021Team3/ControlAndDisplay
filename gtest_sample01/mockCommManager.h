#pragma once

#include "pch.h"
#include <opencv2/opencv.hpp>
#include "../desktop_app01/commManager.h"

using namespace std;

class MockCommManager : public CommManagerInterface {
public:
	MOCK_METHOD(int, connect, (const bool secureMode), (override));
	MOCK_METHOD(int, login, (const string& username, const string& password), (override));
	MOCK_METHOD(bool, requestFaces, (const int uid), (override));
	MOCK_METHOD(bool, requestAddFace, (const int uid, const int numberOfImages), (override));
	MOCK_METHOD(bool, requestDelFace, (const int uid, const int fid), (override));

	MOCK_METHOD(void, disconnect, (), (override));
	MOCK_METHOD(bool, requestVideoStart, (), (override));
	MOCK_METHOD(bool, requestVideoEnd, (), (override));
	MOCK_METHOD(bool, requestVideoSource, (std::string videoname), (override));
	MOCK_METHOD(bool, requestStudentList, (), (override));

	MOCK_METHOD(void, setFaceImageListener, (FaceImageListener* l), (override));
	MOCK_METHOD(void, setVideoListener, (VideoListener* l), (override));
	MOCK_METHOD(void, setUserListener, (UserListener* l), (override));
};