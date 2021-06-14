#pragma once
#ifndef _FACE_MANAGER_H
#define _FACE_MANAGER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "commManager.h"

using namespace std;

struct faceData {
    std::string userID;
    std::vector<std::string> faceNumber;
};

class FaceManager
{
private:
    CommManagerFRS* commManager;

    cv::VideoCapture* capture;

    // const int videoFrameWidth = 1280;
    // const int videoFrameHeight = 720;
    const int videoFrameWidth = 640;
    const int videoFrameHeight = 480;

    bool useCamera;
    bool rotate180;

    int mCurrentUid;

    void rotateFrame(cv::Mat& frame);
    void openRecordFile(const char* filename);
    void record(cv::Mat& frame);

public:
    FaceManager(CommManagerFRS* comm);
    FaceManager(CommManagerFRS* comm, const char* filename);
    ~FaceManager();
    bool init();
    void start();
    bool processFrame();
    bool registerFace();
    void stop();
    bool deleteFaceDB(string userId, string faceId);
    bool addFaceDB(string userId, string faceId);
    void sendFaceImages(string userId);
    void setCurrentUid(string userId);
    bool findUserFromDB(string userId);
    vector<string> getFaceListFromDB(string userId);
    vector<faceData> readFaceDB();
};

#endif // _FACE_MANAGER_H