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
class SecurityManagerAcs;

class CommManagerInterface {
public:
	virtual int connect(const bool secureMode) = 0;
	virtual int login(const string& username, const string& password) = 0;
	virtual bool requestFaces(const int uid) = 0;
	virtual bool requestAddFace(const int uid, const int numberOfImages) = 0;
	virtual bool requestDelFace(const int uid, const int fid) = 0;
	virtual void disconnect() = 0;
	virtual bool requestVideoStart() = 0;
	virtual bool requestVideoEnd() = 0;
	virtual bool requestVideoSource(std::string videoname) = 0;
	virtual bool requestStudentList() = 0;

	virtual void setFaceImageListener(FaceImageListener* l) = 0;
	virtual void setVideoListener(VideoListener* l) = 0;
	virtual void setUserListener(UserListener* l) = 0;
};

class CommManager : public CommManagerInterface {
private:
	FaceImageListener* faceImageListener = nullptr;
	VideoListener* videoListener = nullptr;
	UserListener* userListener = nullptr;

	TTcpConnectedPort* connection = nullptr;
	HANDLE hThread = 0;
	DWORD tid;
	static DWORD WINAPI StaticReceiver(LPVOID lpParam);
	bool send(const int cmd);
	bool send(const int cmd, const int v);
	bool send(const int cmd, const int v1, const int v2);
	bool send(const int cmd, const int v, const std::string s);
	bool send(const int cmd, const std::string s);
	bool send(const int cmd, const std::string s1, const std::string s2);
	bool send(const int cmd, const int v1, const int v2, const std::string s1, const std::string s2);
	SecurityManagerAcs* securityManager;
public:
	CommManager();
	~CommManager();
	DWORD receiver();
	int connect(const bool secureMode);
	bool connect(const string& hostname, string portname, const bool secureMode);
	int login(const string& username, const string& password);
	bool requestFaces(const int uid);
	bool requestAddFace(const int uid, const int numberOfImages);
	bool requestDelFace(const int uid, const int fid);
	void disconnect();
	bool isConnected();
	bool recvVideo(cv::Mat* frame);

	bool requestVideoStart();
	bool requestVideoEnd();
	bool requestVideoSource(std::string videoname);
	bool requestStudentList();

	void setFaceImageListener(FaceImageListener* l)
	{
		faceImageListener = l;
	}
	void setVideoListener(VideoListener* l)
	{
		videoListener = l;
	}
	void setUserListener(UserListener* l)
	{
		userListener = l;
	}
};

