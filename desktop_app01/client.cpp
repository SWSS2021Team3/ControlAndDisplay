#include "client.h"

Client::Client() {}
Client::~Client()
{
	if (isConnected())
	{
		disconnect();
	}
}

bool Client::connect(const char* hostname, const char* portname)
{
	if ((connection = OpenTcpConnection(hostname, portname)) == NULL)
		return false;
	return true;
}

bool Client::isConnected()
{
	return connection != nullptr;
}

bool Client::recvVideo()
{
	bool result = TcpRecvImageAsJpeg(connection, &mat_frame);
	return result;
}
cv::Mat& Client::getVideoFrame()
{
	return mat_frame;
}

void Client::disconnect()
{
	CloseTcpConnectedPort(&connection);
	connection = nullptr;
}
