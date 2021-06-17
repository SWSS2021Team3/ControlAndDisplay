#pragma once

#include "framework.h"
#include "view.h"
#include "viewHandler.h"
#include "attendanceChecker.h"

class StudentView : public View, StudentViewHandler
{
private:
	AttendanceChecker* acs;
	BITMAPINFO* m_pBitmapInfo = nullptr;

	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam);
public:
	StudentView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker* ac);
	~StudentView();
	ViewState start();
	void stop();
	void showFaceImages();

	void onFaceImageUpdate();
};