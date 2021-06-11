#include "studentController.h"

bool StudentController::fetchFaces()
{
	if (userAuthManager == nullptr) return false;
	if (commManager == nullptr) return false;

	int uid = userAuthManager->getCurrentUser().uid;
	return commManager->requestFaces(uid, NUMBER_OF_FACES_REQ, faces);
}

std::vector<cv::Mat>& StudentController::getFaces()
{
	return faces;
}