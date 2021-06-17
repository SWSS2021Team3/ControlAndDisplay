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

	std::vector<std::string> studentList;

	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam);
	void renderVideo(cv::Mat& frame);
	void updateUserAttendance(std::string username);
	void updateUserList();
	void clearUserAttendance();

public:
	AttendanceView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker *ac);
	void start();

	void onVideoUpdate(cv::Mat& frame);
	void onAttendanceUpdate(std::string username);
	void onStudentListUpdate(std::vector<std::string>& studentList);

};