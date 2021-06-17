#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "Payload.h"
#include "SecurityManagerAcs.h"

CommManager::CommManager() : tid(0)
{
	securityManager = new SecurityManagerAcs;
}

CommManager::~CommManager()
{
	if (isConnected())
		disconnect();
	delete securityManager;
}

DWORD WINAPI CommManager::StaticReceiver(LPVOID lpParam)
{
	CommManager* that = (CommManager*)lpParam;
	return that->receiver();
}


DWORD CommManager::receiver()
{
	SerializablePayload payload;
	bool responseResult;
	TTcpConnectedPort conn = *connection;
	while (true)
	{
		if (!TcpRecvObject(&conn, &payload)) {
			std::cout << "failed to receive payload" << endl;
			continue;
		}

		switch (payload.data_id)
		{
		case SIGNAL_FM_RESP_GET_FACES:
		{
			cv::Mat faceImage;

			responseResult = TcpRecvImageAsJpeg(&conn, &faceImage);
			if (responseResult)
			{
				if (faceImageListener != nullptr)
				{
					faceImageListener->onFaceImageReceive(faceImage);
				}
			}
			break;
		}
		case SIGNAL_FM_RESP_FACE_ADD:
		{
			cv::Mat faceImage;

			responseResult = TcpRecvImageAsJpeg(&conn, &faceImage);
			if (responseResult)
			{
				if (faceImageListener != nullptr)
				{
					faceImageListener->onFaceAdd(faceImage);
				}
			}
			break;
		}
		case SIGNAL_FM_RESP_FACE_DELETE:
		{
			if (faceImageListener != nullptr)
			{
				faceImageListener->onFaceDelete();
			}
		}
		case SIGNAL_FM_RESP_VIDEO_FRAME:
		{
			cv::Mat frame;
			responseResult = TcpRecvImageAsJpeg(&conn, &frame);
			if (responseResult)
			{
				if (videoListener != nullptr)
				{
					videoListener->onVideoFrameReceive(frame);
				}
			}
		}
		case SIGNAL_FM_RESP_USER_ATTEND:
		{
			if (videoListener != nullptr)
				videoListener->onUserAttend(payload.str1);
			break;
		}
		case SIGNAL_FM_RESP_LOGIN_OK:
		{
			if (userListener != nullptr)
				userListener->onUserLogin(true, payload.i1, payload.str1);
			break;
		}
		case SIGNAL_FM_RESP_LOGIN_FAILED:
		{
			if (userListener != nullptr)
				userListener->onUserLogin(false, 0, "");
			break;
		}
		default:
			break;
		}
	}

	return 0;
}
int CommManager::connect(const bool secureMode)
{
	std::string ip = "192.168.0.106";
	std::string port = "5000";
	std::string secureport = "5001";
	int ret;
	ret = securityManager->readKey(); //fixme: abort if return
	if (!securityManager->readKey()) {
		return -1024;
	}
	if (!securityManager->readConfig(ip, port, secureport)) {
		return -1025;
	}

	if (secureMode) {
		return (int)connect(ip, secureport, secureMode);
	}
	else {
		return (int)connect(ip, port, secureMode);
	}
}

bool CommManager::connect(const string& hostname, string portname, const bool secureMode)
{
	if (isConnected())
		disconnect();

	if (secureMode) {
		int portNum = std::stoi(portname);
		portNum += 10;
		portname = std::to_string(portNum);
	}

	if ((connection = OpenTcpConnection(hostname.c_str(), portname.c_str())) == NULL)
		return false;
	if (secureMode) {
		connection->secureMode = true;
		connection->ssl = securityManager->getSecureNeworkContext();
		securityManager->setNetworkSd(connection->ssl, (int)connection->ConnectedFd);
		//fixme if return -1, then alert messagebox
	}
	if (!hThread)
		hThread = CreateThread(NULL, 0, CommManager::StaticReceiver, (LPVOID)this, 0, &tid);
	return true;
}

bool CommManager::send(const int cmd)
{
	return send(cmd, 0, 0, "", "");
}

bool CommManager::send(const int cmd, const int v)
{
	return send(cmd, v, 0, "", "");
}

bool CommManager::send(const int cmd, const int v1, const int v2)
{
	return send(cmd, v1, v2, "", "");
}

bool CommManager::send(const int cmd, const int v, const std::string s)
{
	return send(cmd, v, 0, s, "");
}

bool CommManager::send(const int cmd, const std::string s)
{
	return send(cmd, 0, 0, s, "");
}

bool CommManager::send(const int cmd, const std::string s1, const std::string s2)
{
	return send(cmd, 0, 0, s1, s2);
}

bool CommManager::send(const int cmd, const int v1, const int v2, const std::string s1, const std::string s2)
{
	if (connection == nullptr) return false;

	SerializablePayload payload;
	payload.data_id = cmd;
	payload.i1 = v1;
	payload.i2 = v2;
	payload.str1 = s1;
	payload.str2 = s2;

	int ret = TcpSendObject(connection, &payload);
	return ret >= 0;
}

int CommManager::login(const string& username, const string& password)
{
	return (int)send(SIGNAL_FM_REQ_LOGIN, username, password);
}

bool CommManager::requestFaces(const int uid)
{
	return send(SIGNAL_FM_REQ_GET_FACES, std::to_string(uid));
}

bool CommManager::requestAddFace(const int uid, const int numberOfImages)
{
	return send(SIGNAL_FM_REQ_FACE_ADD, numberOfImages, std::to_string(uid));
}

bool CommManager::requestDelFace(const int uid, const int fid)
{
	return send(SIGNAL_FM_REQ_FACE_DELETE, std::to_string(uid));
}

bool CommManager::isConnected()
{
	return connection != nullptr;
}

bool CommManager::recvVideo(cv::Mat* frame)
{
	return TcpRecvImageAsJpeg(connection, frame);
}

bool CommManager::requestVideoStart()
{
	return send(SIGNAL_FM_REQ_VIDEO_START);
}

bool CommManager::requestVideoEnd()
{
	return send(SIGNAL_FM_REQ_VIDEO_END);
}

bool CommManager::requestVideoSource(std::string videoname)
{
	if (videoname == "live")
		return send(SIGNAL_FM_REQ_VIDEO_LIVE);
	return send(SIGNAL_FM_REQ_VIDEO_RECORD, videoname);
}

void CommManager::disconnect()
{
	send(SIGNAL_FM_REQ_DISCONNECT);
	Sleep(300);
	if (hThread)
	{
		CloseHandle(hThread);
		hThread = 0;
	}
	CloseTcpConnectedPort(&connection);
	connection = nullptr;
}
