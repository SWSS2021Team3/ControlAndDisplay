#include "commManager.h"

CommManager::~CommManager()
{
	if (isConnected())
	{
		disconnect();
	}
}
bool CommManager::connect()
{
	return connect("127.0.0.1", "5000");
}

bool CommManager::connect(const string& hostname, const string& portname)
{
	if ((connection = OpenTcpConnection(hostname.c_str(), portname.c_str())) == NULL)
		return false;
	return true;
}

bool CommManager::login(const string& username, const string& password)
{
	return true;
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
}