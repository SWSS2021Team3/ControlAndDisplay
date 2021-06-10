#include "attendanceChecker.h"

AttendanceChecker::AttendanceChecker() {}
AttendanceChecker::~AttendanceChecker()
{
	if (isConnected())
	{
		disconnect();
	}
}

bool AttendanceChecker::connect(const char* hostname, const char* portname)
{
	if ((connection = OpenTcpConnection(hostname, portname)) == NULL)
		return false;
	return true;
}

bool AttendanceChecker::isConnected()
{
	return connection != nullptr;
}

bool AttendanceChecker::recvVideo()
{
	bool result = TcpRecvImageAsJpeg(connection, &mat_frame);
	return result;
}
cv::Mat& AttendanceChecker::getVideoFrame()
{
	return mat_frame;
}

void AttendanceChecker::disconnect()
{
	CloseTcpConnectedPort(&connection);
	connection = nullptr;
}
