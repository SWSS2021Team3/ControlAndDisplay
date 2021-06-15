#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"
#include "listener.h"
#include "viewHandler.h"

class AttendanceController : public VideoListener
{
private:
	CommManagerInterface* commManager;
	UserAuthManagerInterface* userAuthManager;

	AttendanceViewHandler* viewHandler = nullptr;

public:
	static const int NUMBER_OF_FACES_REQ = 5;

	AttendanceController(CommManagerInterface* comm);
	void videoStart();
	void videoEnd();
	void onVideoFrameReceive(cv::Mat& image);

	void setViewHandler(AttendanceViewHandler* vh);
};