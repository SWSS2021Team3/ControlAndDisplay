#pragma once
#ifndef _COMM_MANAGER_H
#define _COMM_MANAGER_H
#define HAVE_STRUCT_TIMESPEC 1

#include <opencv2/core.hpp>
#include <pthread.h>
#include <queue>
#if _WIN32
#include "../desktop_app01/NetworkTCP.h"
#include "../desktop_app01/TcpSendRecvJpeg.h"
#include "../desktop_app01/TcpSendRecvCmd.h"
#include "../desktop_app01/MSG_DB.h"
#else
#include "NetworkTCP.h"
#include "TcpSendRecvJpeg.h"
#include "TcpSendRecvCmd.h"
#include "MSG_DB.h"
#endif
class FaceManager;
class Payload;

enum class Command
{
    GET_FACES,
    ADD_FACE,
    LOGIN,
};

class CommandMessage
{
public:
    CommandMessage(Command _c) : cmd(_c) {}
    CommandMessage(Command _c, int _uid) : cmd(_c), uid(_uid) {}
    CommandMessage(Command _c, std::string userId, std::string password) {}
    Command cmd;
    std::string userId;
    std::string password;
    int uid;
};

class CommManagerFRS
{
private:
    int port;
    TTcpListenPort* TcpListenPort;
    TTcpConnectedPort* TcpConnectedPort;
    pthread_mutex_t sendMutex;

    pthread_mutex_t recvMutex;
    std::queue<CommandMessage> commandQueue;

public:
    CommManagerFRS(int _port) : port(_port)
    {
        pthread_mutex_init(&sendMutex, NULL);
        pthread_mutex_init(&recvMutex, NULL);
    }
    bool connect();
    void disconnect();
    bool sendFace(cv::Mat& frame);
    bool sendFrame(cv::Mat& frame);
    bool sendRegisteredFace(cv::Mat& frame);
    bool sendLoginResp(bool result_ok);
    bool sendMessage();
    void receive();
    bool do_loop(FaceManager* faceManager);
    bool sendCommand(int cmd);
};

#endif // _COMM_MANAGER_H