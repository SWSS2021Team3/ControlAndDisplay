#pragma once

#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"
#include "studentController.h"
#include "viewHandler.h"

class AttendanceChecker
{
private:
	CommManager* commManager = nullptr;
	UserAuthManager* userAuthManager = nullptr;
	StudentController* studentController = nullptr;

	cv::Mat mat_frame;

public:
	AttendanceChecker();
	~AttendanceChecker();
	void setStudentViewHandler(StudentViewHandler* vh);
	void setUserAuthViewHandler(UserAuthViewHandler* vh);
	bool login(const string& username, const string& password);
	bool isConnected();
	bool recvVideo();
	cv::Mat& getVideoFrame();
	void logout();
	void fetchFaceImages();
	vector<cv::Mat>& getFaces();
};
