#include "attendanceChecker.h"

AttendanceChecker::AttendanceChecker()
{
	commManager = new CommManager();
	userAuthManager = new UserAuthManager(commManager);
	studentController = new StudentController(commManager, userAuthManager);
	attendanceController = new AttendanceController(commManager);
}

AttendanceChecker::~AttendanceChecker()
{
	delete commManager;
	delete userAuthManager;
	delete studentController;
}

int AttendanceChecker::login(const string& username, const string& password, const bool secureMode)
{
	return userAuthManager->login(username, password, secureMode);
}

bool AttendanceChecker::isLogin()
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
	studentController->faces.clear();
	userAuthManager->logout();
}

void AttendanceChecker::fetchFaceImages()
{
	studentController->fetchFaces();
}

void AttendanceChecker::addFaceImage()
{
	studentController->addFace();
}

void AttendanceChecker::deleteFaceImage()
{
	studentController->deleteFace();
}

vector<cv::Mat>& AttendanceChecker::getFaces()
{
	return studentController->getFaces();
}

void AttendanceChecker::setStudentViewHandler(StudentViewHandler* vh)
{
	studentController->setViewHandler(vh);
}
void AttendanceChecker::setUserAuthViewHandler(UserAuthViewHandler* vh)
{
	userAuthManager->setViewHandler(vh);
}

void AttendanceChecker::setAttendanceViewHandler(AttendanceViewHandler* vh)
{
	attendanceController->setViewHandler(vh);
}

void AttendanceChecker::startVideo()
{
	attendanceController->videoStart();
}

void AttendanceChecker::endVideo()
{
	attendanceController->videoEnd();
}

void AttendanceChecker::fetchStudentList()
{
	attendanceController->fetchStudentList();
}

void AttendanceChecker::clearAttendanceList()
{
	attendanceController->clearList();
}

void AttendanceChecker::switchToLive()
{
	attendanceController->switchVideo("live");
}

void AttendanceChecker::switchToRecord(std::string recordfile)
{
	attendanceController->switchVideo(recordfile);
}
