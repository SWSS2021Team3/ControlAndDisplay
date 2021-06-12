#pragma once
#include <opencv2/opencv.hpp>

class FaceImageListener
{
public:
	virtual void onFaceImageReceive(cv::Mat& face) = 0;
};