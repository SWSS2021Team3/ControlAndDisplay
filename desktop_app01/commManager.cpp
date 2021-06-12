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

	while (true)
	{
		if (!TcpRecvCommand(connection, &payload)) {
			std::cout << "failed to receive payload" << endl;
			continue;
		}

		switch (payload.data_id)
		{
		case SIGNAL_FM_RESP_GET_FACES:
		{
			cv::Mat faceImage;

			responseResult = TcpRecvImageAsJpeg(connection, &faceImage);
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
bool CommManager::connect()
{
	return connect("127.0.0.1", "5000");
}

bool CommManager::connect(const string& hostname, const string& portname)
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
	return { -1, username };
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
	CloseTcpConnectedPort(&connection);
	connection = nullptr;
	if (hThread)
	{
		CloseHandle(hThread);
		hThread = 0;
	}
}

Payload* CommManager::createCmdPacket(int cmd)
{
	Payload* payload = new Payload();
	payload->data_id = cmd;
	payload->data_length = 0;
	// payload->data = NULL; // TODO: set to NULL
	return payload;
}