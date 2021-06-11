#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "NetworkTCP.h"
#include "TcpSendRecvJpeg.h"
#include "user.h"
#include "MSG_DB.H"
#include "TcpSendRecvCmd.h"

using namespace std;

class CommManagerInterface {
public:
	virtual bool connect() = 0;
	virtual User login(const string& username, const string& password) = 0;
	virtual bool requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces) = 0;
	virtual void disconnect() = 0;
};

class CommManager : public CommManagerInterface {
private:
	TTcpConnectedPort* connection = nullptr;
public:
	~CommManager();
	bool connect();
	bool connect(const string& hostname, const string& portname);
	User login(const string& username, const string& password);
	bool requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces);
	void disconnect();
	bool isConnected();
	bool recvVideo(cv::Mat* frame);
	Payload* createCmdPacket(int cmd);
};

