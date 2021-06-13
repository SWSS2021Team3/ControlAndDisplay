//------------------------------------------------------------------------------------------------
// File: SendImageTCP.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// This program Sends a jpeg image From the Camera via a TCP Stream to a remote destination.
//----------------------------------------------------------------------------------------------
#define HAVE_STRUCT_TIMESPEC 1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "../desktop_app01/NetworkTCP.h"
#include "../desktop_app01/TcpSendRecvJpeg.h"
#include "../desktop_app01/MSG_DB.h"
#include "../desktop_app01/Payload.h"
#include "../desktop_app01/TcpSendRecvCmd.h"

#ifdef _WIN32
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif
//#define SINGLE_LOOP

using namespace cv;
using namespace std;

std::string gstreamer_pipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method)
{
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
		std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
		"/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
		std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

//----------------------------------------------------------------
// main - This is the main program for the RecvImageUDP demo
// program  contains the control loop
//---------------------------------------------------------------

pthread_mutex_t sendMutex;

std::string getFilePath(std::string filename);
void* receiverFunc(void* arg);
int sendCmd(TTcpConnectedPort* connection, int cmd);

int main(int argc, char* argv[])
{

	Mat image; // camera image in Mat format
	TTcpListenPort* TcpListenPort;
	TTcpConnectedPort* TcpConnectedPort;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	int key;

	int portNumber = 5000;
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	else
	{
		fprintf(stderr, "usage %s [port]\n", argv[0]);
	}

	int capture_width = 1280;
	int capture_height = 720;
	int display_width = 1280;
	int display_height = 720;
	int framerate = 60;
	int flip_method = 2;

#if defined(_WIN32) || defined(_WIN64)
	cv::VideoCapture capture(0, cv::CAP_ANY);
#else
	std::string pipeline = gstreamer_pipeline(capture_width,
		capture_height,
		display_width,
		display_height,
		framerate,
		flip_method);
	std::cout << "Using pipeline: \n\t" << pipeline << "\n";

	cv::VideoCapture capture(pipeline, cv::CAP_GSTREAMER);
#endif
	if (!capture.isOpened())
	{
		std::cout << "Failed to open camera." << std::endl;
		return (-1);
	}

	pthread_mutex_init(&sendMutex, NULL);
	while (true)
	{

		if ((TcpListenPort = OpenTcpListenPort(portNumber)) == NULL) // Open UDP Network port
		{
			printf("OpenTcpListenPortFailed\n");
			return (-1);
		}

		clilen = sizeof(cli_addr);

		printf("Listening for connections : port %d\n", portNumber);

		if ((TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen)) == NULL)
		{
			printf("AcceptTcpConnection Failed\n");
			return (-1);
		}

		printf("Accepted connection Request\n");

		int status;
		pthread_t tid;
		if ((status = pthread_create(&tid, NULL, &receiverFunc, (void*)TcpConnectedPort)) != 0)
		{
			std::cout << "thread create error" << std::endl;
			return (-1);
		}

#ifdef SINGLE_LOOP

		capture.read(image);
		cout << "SendImage..." << endl;
		// Send processed TCP image
		if (sendCmd(TcpConnectedPort, SIGNAL_FM_RESP_GET_FACES) < 0)
		{
			std::cout << "failed to send command" << endl;
		}

		if (TcpSendImageAsJpeg(TcpConnectedPort, image) < 0)
			cout << "SendImage Failed" << endl;

		waitKey(10000);

		capture.read(image);
		cout << "SendImage...2" << endl;
		if (sendCmd(TcpConnectedPort, SIGNAL_FM_RESP_GET_FACES) < 0)
		{
			std::cout << "failed to send command" << endl;
		}

		if (TcpSendImageAsJpeg(TcpConnectedPort, image) < 0)
			cout << "SendImage Failed" << endl;
#else

		do
		{
			//// wait for a new frame from camera and store it into 'frame'
			//capture.read(image);
			//// check if we succeeded
			//if (image.empty())
			//{
			//	printf("ERROR! blank frame grabbed\n");
			//	continue;
			//}
#if 0	
			cudaRGBA32ToBGR8(image, (uchar3*)rgb_gpu, capture_width, capture_height);

			cv::cuda::GpuMat imgRGB_gpu(imgHeight, capture_height, CV_8UC3, rgb_gpu);

			std::vector<struct Bbox> detections;
			finder.findFace(imgRGB_gpu, &detections);

			std::vector<cv::Rect> rects;
			std::vector<float*> keypoints;
			num_dets = get_detections(origin_cpu, &detections, &rects, &keypoints);
#endif

			//pthread_mutex_lock(&sendMutex);
			//if (sendCmd(TcpConnectedPort, SIGNAL_FM_RESP_VIDEO_FRAME) < 0)
			//{
			//	std::cout << "failed to send command" << endl;
			//}

			//if (TcpSendImageAsJpeg(TcpConnectedPort, image) < 0)
			//{
			//	cout << "SendImage Failed" << endl;
			//}
			//pthread_mutex_unlock(&sendMutex);

			key = (waitKey(100) & 0xFF);
			pthread_mutex_lock(&sendMutex);
			if (sendCmd(TcpConnectedPort, SIGNAL_FM_BASE) < 0) // PING
			{
				std::cout << "failed to send command" << endl;
				pthread_mutex_unlock(&sendMutex);
				break;
			}
			pthread_mutex_unlock(&sendMutex);

			//printf("%d\n", key);
		} while (key != 'q'); // loop until user hits quit

#endif

		pthread_cancel(tid);
		//pthread_join(tid, NULL);

		CloseTcpConnectedPort(&TcpConnectedPort); // Close network port;
		CloseTcpListenPort(&TcpListenPort);       // Close listen port
	}

	return 0;
}

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

void* receiverFunc(void* arg)
{
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	TTcpConnectedPort* connection = (TTcpConnectedPort*)arg;

	Payload payload;

	while (true)
	{

		if (!TcpRecvCommand(connection, &payload)) {
			std::cout << "failed to receive payload" << std::endl;
			continue;
		}

		std::cout << "payload data_id: " << payload.data_id << std::endl;

		switch (payload.data_id)
		{
		case SIGNAL_FM_REQ_GET_FACES:
		{
			std::cout << "SIGNAL_FM_REQ_GET_FACES" << std::endl;

			std::string filenames[] = { "Chandler.png", "Rachel.png" };
			std::string imagePath = getFilePath(filenames[rand() % (sizeof(filenames) / sizeof(filenames[0]))]);
			cv::Mat face = cv::imread(imagePath, cv::IMREAD_COLOR);
			pthread_mutex_lock(&sendMutex);
			if (sendCmd(connection, SIGNAL_FM_RESP_GET_FACES) < 0)
			{
				std::cout << "failed to send command" << endl;
			}

			if (TcpSendImageAsJpeg(connection, face) < 0)
			{
				cout << "SendImage Failed" << endl;
			}
			pthread_mutex_unlock(&sendMutex);
			break;
		}
		case SIGNAL_FM_REQ_FACE_DELETE:
			std::cout << "SIGNAL_FM_REQ_FACE_DELETE" << std::endl;
			break;
			//case SIGNAL_FM_REQ_LOGIN:
			//{
			//	std::cout << "SIGNAL_FM_REQ_LOGIN" << std::endl;
			//	 TODO: get login data from payload
			//	string userid, passwd;
			//	bool loginResult = userAuthManager->verifyUser(userid, passwd);
			//	std::cout << "login result : " << loginResult << endl;
			//	break;
			//}

		default:
			break;
		}
	}
	return NULL;
}
int sendCmd(TTcpConnectedPort* connection, int cmd)
{
	Payload payload;
	payload.data_id = cmd;
	payload.data_length = 0;

	return TcpSendCommand(connection, &payload);
}
//-----------------------------------------------------------------
// END main
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
