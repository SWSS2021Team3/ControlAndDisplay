#pragma once
#define SIGNAL_FM_BASE                          0x1000
#define SIGNAL_FM_REQ_GET_FACES         SIGNAL_FM_BASE + 0x0001
#define SIGNAL_FM_RESP_GET_FACES        SIGNAL_FM_BASE + 0x0002
#define SIGNAL_FM_REQ_FACE_ADD          SIGNAL_FM_BASE + 0x0003
#define SIGNAL_FM_RESP_FACE_ADD         SIGNAL_FM_BASE + 0x0004
#define SIGNAL_FM_REQ_FACE_DELETE       SIGNAL_FM_BASE + 0x0005
#define SIGNAL_FM_RESP_FACE_DELETE      SIGNAL_FM_BASE + 0x0006
#define SIGNAL_FM_REQ_LOGIN             SIGNAL_FM_BASE + 0x0007
#define SIGNAL_FM_RESP_LOGIN_OK         SIGNAL_FM_BASE + 0x0008
#define SIGNAL_FM_RESP_LOGIN_FAILED     SIGNAL_FM_BASE + 0x0009
#define SIGNAL_FM_REQ_VIDEO_START		SIGNAL_FM_BASE + 0x000a
#define SIGNAL_FM_REQ_VIDEO_END			SIGNAL_FM_BASE + 0x000b
#define SIGNAL_FM_RESP_VIDEO_FRAME      SIGNAL_FM_BASE + 0x0010
#define SIGNAL_FM_RESP_USER_ATTEND      SIGNAL_FM_BASE + 0x0011
#define SIGNAL_FM_REQ_STUDENT_LIST      SIGNAL_FM_BASE + 0x0012
#define SIGNAL_FM_RESP_STUDENT_LIST     SIGNAL_FM_BASE + 0x0013
#define SIGNAL_FM_REQ_DISCONNECT		SIGNAL_FM_BASE + 0x0030
#define SIGNAL_FM_REQ_VIDEO_LIVE		SIGNAL_FM_BASE + 0x0031
#define SIGNAL_FM_REQ_VIDEO_RECORD		SIGNAL_FM_BASE + 0x0032


