//------------------------------------------------------------------------------------------------
// File: SendImageTCP.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// This program Sends a jpeg image From the Camera via a TCP Stream to a remote destination.
//----------------------------------------------------------------------------------------------
#include <string>

#include "commManager.h"
#include "faceManager.h"

// Uncomment to print timings in milliseconds
#define LOG_TIMES


int main(int argc, char* argv[])
{
	CommManagerFRS* commManager;
	FaceManager* faceManager;

	bool UseCamera = false;

	int portNumber = 5000;

	if (argc < 2)
	{
		fprintf(stderr, "usage %s [port] [filename]\n", argv[0]);
	}
	if (argc > 1)
		portNumber = atoi(argv[1]);

	commManager = new CommManagerFRS(portNumber);

	if (argc == 2)
		faceManager = new FaceManager(commManager);
	else
		faceManager = new FaceManager(commManager);//, argv[2]);

	if (!faceManager->init())
		return 1;

	bool inLoop = true;

	while (inLoop)
	{
		if (commManager->connect() == false)
		{
			printf("CommManager - connect failed.\n");
			return (-1);
		}

		faceManager->start();

		inLoop = commManager->do_loop(faceManager);

		faceManager->stop();

		commManager->disconnect();
	}

	delete faceManager;
	delete commManager;

	return 0;
}

//-----------------------------------------------------------------
// END main
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
