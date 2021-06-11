#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"

class StudentController
{
private:
	CommManagerInterface* commManager;
	UserAuthManagerInterface* userAuthManager;

public:
	static const int NUMBER_OF_FACES_REQ = 5;

	StudentController(CommManagerInterface* comm, UserAuthManagerInterface* uam) : commManager(comm), userAuthManager(uam) {}
	bool fetchFaces();

	std::vector<cv::Mat> faces;
	std::vector<cv::Mat>& getFaces();
};