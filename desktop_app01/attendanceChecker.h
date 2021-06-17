#pragma once

#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"
#include "studentController.h"
#include "attendanceController.h"
#include "viewHandler.h"

class AttendanceChecker
{
private:
	CommManager* commManager = nullptr;
	UserAuthManager* userAuthManager = nullptr;
	StudentController* studentController = nullptr;
	AttendanceController* attendanceController = nullptr;

	cv::Mat mat_frame;

public:
	AttendanceChecker();
	~AttendanceChecker();
	void setStudentViewHandler(StudentViewHandler* vh);
	void setUserAuthViewHandler(UserAuthViewHandler* vh);
	void setAttendanceViewHandler(AttendanceViewHandler* vh);
	int login(const string& username, const string& password, const bool secureMode);
	bool isLogin();
	bool recvVideo();
	cv::Mat& getVideoFrame();
	void logout();
	void fetchFaceImages();
	void addFaceImage();
	void deleteFaceImage();
	vector<cv::Mat>& getFaces();

	void startVideo();
	void endVideo();
	void clearAttendanceList();

	void switchToLive();
	void switchToRecord(std::string recordfile);
};
