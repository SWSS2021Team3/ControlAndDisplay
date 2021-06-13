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
	Payload payload;
	bool responseResult;
	TTcpConnectedPort conn = *connection;
	while (true)
	{
		if (!TcpRecvCommand(&conn, &payload)) {
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

bool CommManager::send(int cmd)
{
	if (connection == nullptr) return false;

	Payload payload;
	payload.data_id = cmd;
	payload.data_length = 0;

	int ret = TcpSendCommand(connection, &payload);
	return ret >= 0;
}

User CommManager::login(const string& username, const string& password)
{
	return { 3, username };
}

bool CommManager::requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces)
{
	return send(SIGNAL_FM_REQ_GET_FACES);
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

Payload* CommManager::createCmdPacket(int cmd)
{
	Payload* payload = new Payload();
	payload->data_id = cmd;
	payload->data_length = 0;
	// payload->data = NULL; // TODO: set to NULL
	return payload;
}