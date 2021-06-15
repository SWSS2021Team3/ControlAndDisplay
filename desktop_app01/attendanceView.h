#pragma once

#include "framework.h"
#include "view.h"
#include "attendanceChecker.h"
#include "viewHandler.h"

class AttendanceView : public View, AttendanceViewHandler
{
private:
	AttendanceChecker* acs;
	BITMAPINFO* m_pBitmapInfo = nullptr;

	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam);
	void renderVideo(cv::Mat& frame);

public:
	AttendanceView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker *ac);
	void start();

	void onVideoUpdate(cv::Mat& frame);
};