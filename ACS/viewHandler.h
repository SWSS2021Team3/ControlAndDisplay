#pragma once
#include "user.h"
#include <opencv2/opencv.hpp>

class StudentViewHandler
{
public:
	virtual void onFaceImageUpdate() = 0;
};

class UserAuthViewHandler
{
public:
	virtual void onConnectFailed() = 0;
	virtual void onLoginFailed() = 0;
	virtual void onLoginSuccess(User user) = 0;
};

class AttendanceViewHandler
{
public:
	virtual void onVideoUpdate(cv::Mat& image) = 0;
	virtual void onAttendanceUpdate(std::string username) = 0;
	virtual void onStudentListUpdate(std::vector<std::string>& ss) = 0;
};