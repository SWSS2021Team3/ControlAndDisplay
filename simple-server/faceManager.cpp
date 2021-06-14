#include "faceManager.h"
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

FaceManager::FaceManager(CommManagerFRS* comm) : useCamera(true), rotate180(true), commManager(comm)
{
    capture = new cv::VideoCapture(0, cv::CAP_ANY);
}

FaceManager::FaceManager(CommManagerFRS* comm, const char* filename) : useCamera(false), rotate180(false), commManager(comm)
{
    capture = new cv::VideoCapture(filename);
}

FaceManager::~FaceManager()
{
    delete capture;
}

bool FaceManager::init()
{
    return true;
}

void FaceManager::start()
{
}

void FaceManager::openRecordFile(const char* filename)
{
}

void FaceManager::record(cv::Mat& frame)
{
}

void FaceManager::rotateFrame(cv::Mat& frame)
{
}

bool FaceManager::processFrame()
{
    cv::Mat frame;

    *capture >> frame;

    if (!commManager->sendFrame(frame))
        return false;

    return true;
}

#ifdef _WIN32
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif

std::string getFilePath(std::string filename)
{
#ifdef _WIN32
    WCHAR path[MAX_PATH + 1] = { 0 };
    GetModuleFileName(NULL, path, MAX_PATH);
    PathRemoveFileSpec(path);

    std::wstring wsPath(path);
    std::string sPath;
    std::transform(wsPath.begin(), wsPath.end(), std::back_inserter(sPath), [](wchar_t c) {
        return (char)c;
        });

    sPath += "\\" + filename;
    return sPath;
#else
    return filename;
#endif
}

bool FaceManager::registerFace()
{

    std::string filenames[] = { "Chandler.png", "Rachel.png" };
    std::string imagePath = getFilePath(filenames[rand() % (sizeof(filenames) / sizeof(filenames[0]))]);
    cv::Mat face = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (!commManager->sendFace(face))
        return false;

    return true;
}

void FaceManager::sendFaceImages(string userId)
{
    std::string filenames[] = { "Chandler.png", "Rachel.png" };
    for (int i = 0; i < 3; i++)
    {
        std::string imagePath = getFilePath(filenames[rand() % (sizeof(filenames) / sizeof(filenames[0]))]);
        cv::Mat face = cv::imread(imagePath, cv::IMREAD_COLOR);
            commManager->sendRegisteredFace(face);
    }
}

void FaceManager::stop()
{
}

bool FaceManager::deleteFaceDB(string userId, string faceId)
{
    std::cout << "deleteFace: " << userId << " / " << faceId << endl;
    return true;
}

bool FaceManager::addFaceDB(string userId, string faceid)
{
    std::cout << "addFaceDB: " << userId << " / " << faceid << endl;
    return true;
}

bool FaceManager::findUserFromDB(string userId)
{
    std::cout << "findUserFromDB: " << userId << endl;

    return true;
}

void FaceManager::setCurrentUid(string userId) {
}
vector<string> FaceManager::getFaceListFromDB(string userId)
{
    vector<string> ret;
    return ret; //return NULL
}

vector<faceData> FaceManager::readFaceDB()
{
    std::cout << "readFaceDB" << endl;
    vector<faceData> _facelist;

    return _facelist;
}