#include "attendanceController.h"

AttendanceController::AttendanceController(CommManagerInterface* comm) : commManager(comm)
{
	comm->setVideoListener(this);
}

void AttendanceController::videoStart()
{
	if (commManager == nullptr) return;
	commManager->requestVideoStart();
}

void AttendanceController::videoEnd()
{
	if (commManager == nullptr) return;
	commManager->requestVideoEnd();
}

void AttendanceController::switchVideo(std::string videoname)
{
	if (commManager == nullptr) return;
	commManager->requestVideoSource(videoname);
}

void AttendanceController::clearList()
{
	attendanceSet.clear();
}

void AttendanceController::onVideoFrameReceive(cv::Mat& image)
{
	if (viewHandler != nullptr)
		viewHandler->onVideoUpdate(image);
}

void AttendanceController::onUserAttend(std::string username)
{
	if (attendanceSet.find(username) != attendanceSet.end()) return;

	attendanceSet.insert(username);
	if (viewHandler != nullptr)
		viewHandler->onAttendanceUpdate(username);
}

void AttendanceController::setViewHandler(AttendanceViewHandler* vh)
{
	viewHandler = vh;
}
