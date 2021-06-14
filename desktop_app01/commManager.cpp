#include <opencv2/opencv.hpp>
#include "commManager.h"
#include "Payload.h"

CommManager::~CommManager()
{
	if (isConnected())
		disconnect();
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
		}
		break;
		default:
			break;
		}
	}

	return 0;
}
bool CommManager::connect(const bool secureMode)
{
	//return connect("192.168.0.106", "5010", secureMode);
	return connect("127.0.0.1", "5000", secureMode);
}

bool CommManager::connect(const string& hostname, const string& portname, const bool secureMode)
{
	if (isConnected())
		disconnect();

	if ((connection = OpenTcpConnection(hostname.c_str(), portname.c_str())) == NULL)
		return false;
	if (!hThread)
		hThread = CreateThread(NULL, 0, CommManager::StaticReceiver, (LPVOID)this, 0, &tid);
	return true;
}

bool CommManager::send(int cmd, int payload1)
{
	if (connection == nullptr) return false;

	SerializablePayload payload;
	payload.data_id = cmd;
	payload.i1 = payload1;

	int ret = TcpSendObject(connection, &payload);
	return ret >= 0;
}

bool CommManager::send(int cmd)
{
	return send(cmd, 0);
}

User CommManager::login(const string& username, const string& password)
{
	return { 3, username };
}

bool CommManager::requestFaces(const int uid)
{
	return send(SIGNAL_FM_REQ_GET_FACES);
}

bool CommManager::requestAddFace(const int uid, const int numberOfImages)
{
	return send(SIGNAL_FM_REQ_FACE_ADD, uid);
}

bool CommManager::isConnected()
{
	return connection != nullptr;
}

bool CommManager::recvVideo(cv::Mat* frame)
{
	return TcpRecvImageAsJpeg(connection, frame);
}

void CommManager::disconnect()
{
	if (hThread)
	{
		CloseHandle(hThread);
		hThread = 0;
	}
	CloseTcpConnectedPort(&connection);
	connection = nullptr;
}
