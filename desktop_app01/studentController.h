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

	StudentViewHandler* viewHandler = nullptr;

public:
	static const int NUMBER_OF_FACES_REQ = 5;

	StudentController(CommManagerInterface* comm, UserAuthManagerInterface* uam);
	bool fetchFaces();
	bool addFace();
	bool deleteFace();

	std::vector<cv::Mat> faces;
	std::vector<cv::Mat>& getFaces();

	void onFaceImageReceive(cv::Mat& faceImage);
	void onFaceAdd(cv::Mat& faceImage);
	void onFaceDelete();

	void setViewHandler(StudentViewHandler* vh);
};