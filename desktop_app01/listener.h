#pragma once
#include <opencv2/opencv.hpp>

class FaceImageListener
{
public:
	virtual void onFaceImageReceive(cv::Mat& face) = 0;
	virtual void onFaceAdd(cv::Mat& face) = 0;
};

class VideoListener
{
public:
	virtual void onVideoFrameReceive(cv::Mat& image) = 0;
	virtual void onUserAttend(std::string username) = 0;
};