#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "NetworkTCP.h"
#include "TcpSendRecvJpeg.h"

using namespace std;

class CommManagerInterface {
public:
	virtual bool connect() = 0;
	virtual bool login(const string& username, const string& password) = 0;
	virtual void disconnect() = 0;
};

class CommManager : public CommManagerInterface {
private:
	TTcpConnectedPort* connection = nullptr;
public:
	~CommManager();
	bool connect();
	bool connect(const string& hostname, const string& portname);
	bool login(const string& username, const string& password);
	void disconnect();
	bool isConnected();
	bool recvVideo(cv::Mat* frame);
};