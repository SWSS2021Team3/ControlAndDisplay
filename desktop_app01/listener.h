#pragma once
#include <opencv2/opencv.hpp>

class FaceImageListener
{
public:
	virtual void onFaceImageReceive(cv::Mat& face) = 0;
	virtual void onFaceAdd(cv::Mat& face) = 0;
};