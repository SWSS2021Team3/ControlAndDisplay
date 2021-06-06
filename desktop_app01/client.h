#pragma once

#include <opencv2/opencv.hpp>
#include "NetworkTCP.h"
#include "TcpSendRecvJpeg.h"

class Client
{
private:
	TTcpConnectedPort* connection = nullptr;
	cv::Mat mat_frame;

public:
	Client();
	~Client();
	bool connect(const char* hostname, const char* portname);
	bool isConnected();
	bool recvVideo();
	cv::Mat& getVideoFrame();
	void disconnect();
};
