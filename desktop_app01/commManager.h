#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "NetworkTCP.h"
#include "TcpSendRecvJpeg.h"
#include "user.h"
#include "MSG_DB.H"
#include "TcpSendRecvCmd.h"
#include "listener.h"

using namespace std;

class CommManagerInterface {
protected:
	FaceImageListener* faceImageListener = nullptr;

public:
	virtual bool connect(const bool secureMode) = 0;
	virtual User login(const string& username, const string& password) = 0;
	virtual bool requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces) = 0;
	virtual void disconnect() = 0;
	void setFaceImageListener(FaceImageListener* f)
	{
		faceImageListener = f;
	}
};

class CommManager : public CommManagerInterface {
private:
	TTcpConnectedPort* connection = nullptr;
	HANDLE hThread = 0;
	DWORD tid;
	static DWORD WINAPI StaticReceiver(LPVOID lpParam);
	bool send(int cmd);

public:
	~CommManager();
	DWORD receiver();
	bool connect(const bool secureMode);
	bool connect(const string& hostname, const string& portname, const bool secureMode);
	User login(const string& username, const string& password);
	bool requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces);
	void disconnect();
	bool isConnected();
	bool recvVideo(cv::Mat* frame);
	Payload* createCmdPacket(int cmd);
};

