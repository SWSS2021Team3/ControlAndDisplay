#pragma once

#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"

class AttendanceChecker
{
private:
	CommManager* commManager = nullptr;
	UserAuthManager* userAuthManager = nullptr;

	cv::Mat mat_frame;

public:
	AttendanceChecker();
	~AttendanceChecker();
	bool login(const string& username, const string& password);
	bool isConnected();
	bool recvVideo();
	cv::Mat& getVideoFrame();
	void logout();
};
