#include "studentController.h"

StudentController::StudentController(CommManagerInterface* comm, UserAuthManagerInterface* uam) : commManager(comm), userAuthManager(uam)
{
	comm->setFaceImageListener(this);
}

bool StudentController::fetchFaces()
{
	if (userAuthManager == nullptr) return false;
	if (commManager == nullptr) return false;

	int uid = userAuthManager->getCurrentUser().uid;
	commManager->connect(); // temp
	return commManager->requestFaces(uid, NUMBER_OF_FACES_REQ, faces);
}

std::vector<cv::Mat>& StudentController::getFaces()
{
	return faces;
}

void StudentController::onFaceImageReceive(cv::Mat& faceImage)
{
	faces.push_back(faceImage);
}