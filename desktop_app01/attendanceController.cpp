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

void AttendanceController::onVideoFrameReceive(cv::Mat& image)
{
	if (viewHandler != nullptr)
		viewHandler->onVideoUpdate(image);
}

void AttendanceController::setViewHandler(AttendanceViewHandler* vh)
{
	viewHandler = vh;
}
