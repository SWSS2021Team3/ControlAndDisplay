#pragma once

#include "framework.h"
#include "view.h"
#include "attendanceChecker.h"

class AttendanceView : public View
{
private:
	AttendanceChecker* acs;
	BITMAPINFO* m_pBitmapInfo = nullptr;

	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam);
public:
	AttendanceView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker *ac);
};