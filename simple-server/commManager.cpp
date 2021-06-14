
#if _WIN32
#define HAVE_STRUCT_TIMESPEC 1
#endif
#include <pthread.h>

#include <iostream>
#include <string>
#include <chrono>

#include "commManager.h"
#include "faceManager.h"
//#include "userAuthManager.h"


void* receiverFunc(void* arg);

bool CommManagerFRS::connect()
{
    struct sockaddr_in cli_addr;

    socklen_t clilen;

    if ((TcpListenPort = OpenTcpListenPort(port)) == NULL) // Open TCP Network port
    {
        printf("OpenTcpListenPortFailed\n");
        return false;
    }

    clilen = sizeof(cli_addr);

    printf("Listening for connections\n");

    if ((TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen)) == NULL)
    {
        printf("AcceptTcpConnection Failed\n");
        return false;
    }

    printf("Accepted connection Request\n");

    return true;
}

bool CommManagerFRS::sendMessage()
{
    Payload* p = new Payload();
    p->data_id = 0x1001;
    p->data_length = 0;
    // p->data = NULL;
    return true;
}

bool CommManagerFRS::sendFrame(cv::Mat& frame)
{
    pthread_mutex_lock(&sendMutex);

    if (!sendCommand(SIGNAL_FM_RESP_VIDEO_FRAME))
    {
        pthread_mutex_unlock(&sendMutex);
        return false;
    }

    int ret = TcpSendImageAsJpeg(TcpConnectedPort, frame);
    pthread_mutex_unlock(&sendMutex);

    return ret >= 0;
}

bool CommManagerFRS::sendRegisteredFace(cv::Mat& frame)
{
    pthread_mutex_lock(&sendMutex);

    if (!sendCommand(SIGNAL_FM_RESP_GET_FACES))
    {
        pthread_mutex_unlock(&sendMutex);
        return false;
    }

    int ret = TcpSendImageAsJpeg(TcpConnectedPort, frame);
    pthread_mutex_unlock(&sendMutex);

    return ret >= 0;
}

bool CommManagerFRS::sendFace(cv::Mat& frame)
{
    pthread_mutex_lock(&sendMutex);

    if (!sendCommand(SIGNAL_FM_RESP_FACE_ADD))
    {
        pthread_mutex_unlock(&sendMutex);
        return false;
    }

    int ret = TcpSendImageAsJpeg(TcpConnectedPort, frame);
    pthread_mutex_unlock(&sendMutex);

    return ret >= 0;
}

bool CommManagerFRS::sendLoginResp(bool result_ok)
{
    pthread_mutex_lock(&sendMutex);

    if (result_ok) {
        if (!sendCommand(SIGNAL_FM_RESP_LOGIN_OK)) {
            pthread_mutex_unlock(&sendMutex);
            return false;
        }
    }
    else {
        if (!sendCommand(SIGNAL_FM_RESP_LOGIN_FAILED)) {
            pthread_mutex_unlock(&sendMutex);
            return false;
        }
    }

    pthread_mutex_unlock(&sendMutex);
    return true;
}

void CommManagerFRS::disconnect()
{
    CloseTcpConnectedPort(&TcpConnectedPort); // Close network port;
    CloseTcpListenPort(&TcpListenPort);       // Close listen port
}

bool CommManagerFRS::do_loop(FaceManager* faceManager)
{
    // loop over frames with inference
    int nbFrames = 0;
    Payload* payload = NULL;
    //UserAuthManager* userAuthManager = new UserAuthManager();

    int status;
    pthread_t tid;
    if ((status = pthread_create(&tid, NULL, &receiverFunc, (void*)this)) != 0)
    {
        std::cout << "thread create error" << std::endl;
        return (-1);
    }
    long cnt = 0;

    while (true)
    {
        // if (cnt == 1000000) std::cout << "start processFrame" << std::endl;
        // if (cnt > 1000000)
        // {
        //     if (!faceManager->processFrame())
        //         break;
        // }

        // nbFrames++;

        pthread_mutex_lock(&recvMutex);
        if (!commandQueue.empty())
        {
            CommandMessage cmdMsg = commandQueue.front();
            // int param = commandQueue.front().second;
            commandQueue.pop();

            switch (cmdMsg.cmd)
            {
            case Command::GET_FACES:
            {
                std::cout << "get-face" << std::endl;
                // int uid = param;
                faceManager->sendFaceImages("2");
                faceManager->getFaceListFromDB("2");
                break;
            }
            case Command::ADD_FACE:
            {
                // TODO: get num of pictures from payload
                if (!faceManager->registerFace())
                {
                    std::cout << "[ERR] failed to register face" << endl;
                }
                break;
            }
            case Command::LOGIN:
            {
                std::cout << "process LOGIN" << endl;
                string userid = cmdMsg.userId;
                string password = cmdMsg.password;
                bool loginResult = false;
                //loginResult = userAuthManager->verifyUser(userid, password);
                std::cout << "login result : " << loginResult << endl;

                // send response payload with login result
                int uid = 0;
                //uid = userAuthManager->getCurrentUid();
                if (uid < 0) {
                    std::cout << "failed to get user id from database" << endl;
                    sendLoginResp(false);
                }
                else {
                    // set uid to faceManager here
                    faceManager->setCurrentUid(to_string(uid));
                    sendLoginResp(true);
                }
                break;
            }
            default:
                break;
            }
        }
        pthread_mutex_unlock(&recvMutex);

        pthread_mutex_lock(&sendMutex);
        if (!sendCommand(SIGNAL_FM_BASE)) // PING
        {
            std::cout << " failed to send command" << endl;
            pthread_mutex_unlock(&sendMutex);
            break;
        }
        pthread_mutex_unlock(&sendMutex);
    }

    pthread_cancel(tid);


    //delete userAuthManager;
    return true;
}

void* receiverFunc(void* arg)
{
    CommManagerFRS* commManager = (CommManagerFRS*)arg;

    commManager->receive();

    return NULL;
}

void CommManagerFRS::receive()
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    TTcpConnectedPort* connection = TcpConnectedPort;

    SerializablePayload payload;

    while (true)
    {
        std::cout << "wait cmd" << std::endl;

        if (!TcpRecvObject(connection, &payload))
        {
            std::cout << "failed to receive payload" << std::endl;
            continue;
        }

        std::cout << "payload data_id: " << payload.data_id << std::endl;

        switch (payload.data_id)
        {
        case SIGNAL_FM_REQ_GET_FACES:
            std::cout << "SIGNAL_FM_REQ_GET_FACES" << endl;
            pthread_mutex_lock(&recvMutex);
            commandQueue.push(CommandMessage(Command::GET_FACES, 11));  // TODO: user id
            pthread_mutex_unlock(&recvMutex);
            break;
        case SIGNAL_FM_REQ_FACE_ADD:
        {
            std::cout << "SIGNAL_FM_REQ_FACE_ADD" << endl;
            pthread_mutex_lock(&recvMutex);
            commandQueue.push(CommandMessage(Command::ADD_FACE, 11));
            pthread_mutex_unlock(&recvMutex);
            break;
        }
        case SIGNAL_FM_REQ_FACE_DELETE:
            std::cout << "SIGNAL_FM_REQ_FACE_DELETE" << std::endl;
            break;
        case SIGNAL_FM_REQ_LOGIN:
        {
            std::cout << "SIGNAL_FM_REQ_LOGIN" << endl;
            pthread_mutex_lock(&recvMutex);
            string userid, password;
            // TODO: get userid, password from payload

            commandQueue.push(CommandMessage(Command::LOGIN, userid, password));
            pthread_mutex_unlock(&recvMutex);
            break;
        }
        default:
            break;
        }
    }
    return;
}

bool CommManagerFRS::sendCommand(int cmd)
{
    SerializablePayload payload;
    payload.data_id = cmd;
    payload.i1 = 0;

    return TcpSendObject(TcpConnectedPort, &payload) >= 0;
}