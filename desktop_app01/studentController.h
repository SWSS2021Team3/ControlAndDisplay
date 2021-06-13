#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "userAuthManager.h"
#include "listener.h"
#include "viewHandler.h"

class StudentController : public FaceImageListener
{
private:
	CommManagerInterface* commManager;
	UserAuthManagerInterface* userAuthManager;

	ViewHandler* viewHandler = nullptr;

public:
	static const int NUMBER_OF_FACES_REQ = 5;

	StudentController(CommManagerInterface* comm, UserAuthManagerInterface* uam);
	bool fetchFaces();

	std::vector<cv::Mat> faces;
	std::vector<cv::Mat>& getFaces();

	void onFaceImageReceive(cv::Mat& faceImage);

	void setViewHandler(ViewHandler* vh);
};