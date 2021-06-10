#include "attendanceChecker.h"

AttendanceChecker::AttendanceChecker()
{
	commManager = new CommManager();
	userAuthManager = new UserAuthManager(commManager);
}

AttendanceChecker::~AttendanceChecker()
{
	delete commManager;
}

bool AttendanceChecker::login(const string& username, const string& password)
{
	return userAuthManager->login(username, password);
}

bool AttendanceChecker::isConnected()
{
	return commManager->isConnected();
}

bool AttendanceChecker::recvVideo()
{
	return commManager->recvVideo(&mat_frame);
}

cv::Mat& AttendanceChecker::getVideoFrame()
{
	return mat_frame;
}

void AttendanceChecker::logout()
{
	userAuthManager->logout();
}
