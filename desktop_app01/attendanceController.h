#pragma once

#include <vector>
#include <set>
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

	std::set<std::string> attendanceSet;
	std::vector<std::string> studentList;

public:
	static const int NUMBER_OF_FACES_REQ = 5;

	AttendanceController(CommManagerInterface* comm);
	void videoStart();
	void videoEnd();
	void switchVideo(std::string videoname);
	void clearList();
	void fetchStudentList();
	void onVideoFrameReceive(cv::Mat& image);
	void onStudentListReceive(std::vector<std::string>& ss);
	void onUserAttend(std::string username);

	void setViewHandler(AttendanceViewHandler* vh);
};