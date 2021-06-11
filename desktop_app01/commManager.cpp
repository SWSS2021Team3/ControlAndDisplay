#include <opencv2/opencv.hpp>
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

User CommManager::login(const string& username, const string& password)
{
	return {-1, username};
}

bool CommManager::requestFaces(const int uid, const int numberOfImages, vector<cv::Mat>& faces)
{
	// TODO: request
	// TODO: receive images and fetch to faces;
	for (int i = 0; i < numberOfImages; i++)
	{
		faces.emplace_back();
		cv::Mat& face = faces.back();
		cv::String imagePath = i % 2 ? ".\\imgs\\Chandler.png":".\\imgs\\Rachel.png";
		face = cv::imread(imagePath, cv::IMREAD_COLOR);
	}

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