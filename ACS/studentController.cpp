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
	return commManager->requestFaces(uid);
}

bool StudentController::addFace()
{
	if (userAuthManager == nullptr) return false;
	if (commManager == nullptr) return false;

	int uid = userAuthManager->getCurrentUser().uid;
	return commManager->requestAddFace(uid, NUMBER_OF_FACES_REQ - faces.size());
}

bool StudentController::deleteFace()
{
	if (userAuthManager == nullptr) return false;
	if (commManager == nullptr) return false;

	int uid = userAuthManager->getCurrentUser().uid;
	int fid = 0;	//Delete Last one
	return commManager->requestDelFace(uid, fid);
}

std::vector<cv::Mat>& StudentController::getFaces()
{
	return faces;
}

void StudentController::onFaceImageReceive(cv::Mat& faceImage)
{
	faces.push_back(faceImage);
	if (viewHandler != nullptr)
	{
		viewHandler->onFaceImageUpdate();
	}
}

void StudentController::onFaceAdd(cv::Mat& faceImage)
{
	faces.push_back(faceImage);
	if (viewHandler != nullptr)
	{
		viewHandler->onFaceImageUpdate();
	}
}

void StudentController::onFaceDelete()
{
	if (faces.size() <= 0)
		return;
	faces.pop_back();
	if (viewHandler != nullptr)
	{
		viewHandler->onFaceImageUpdate();
	}
}

void StudentController::setViewHandler(StudentViewHandler* vh)
{
	viewHandler = vh;
}