#include "attendanceChecker.h"

AttendanceChecker::AttendanceChecker()
{
	commManager = new CommManager();
	userAuthManager = new UserAuthManager(commManager);
	studentController = new StudentController(commManager, userAuthManager);
}

AttendanceChecker::~AttendanceChecker()
{
	delete commManager;
	delete userAuthManager;
	delete studentController;
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

void AttendanceChecker::fetchFaceImages()
{
	studentController->fetchFaces();
}

vector<cv::Mat>& AttendanceChecker::getFaces()
{
	return studentController->getFaces();
}

void AttendanceChecker::setViewHandler(ViewHandler* vh)
{
	studentController->setViewHandler(vh);
	//userAuthManager->setViewHandler(vh);

}