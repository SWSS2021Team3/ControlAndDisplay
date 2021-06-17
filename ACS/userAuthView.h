#pragma once

#include "framework.h"
#include "view.h"
#include "viewHandler.h"
#include "attendanceChecker.h"

class UserAuthView : public View, UserAuthViewHandler
{
private:
	AttendanceChecker* acs;
	BITMAPINFO* m_pBitmapInfo = nullptr;

	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam);
public:
	UserAuthView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker* ac);
	ViewState start();

	void onConnectFailed();
	void onLoginFailed();
	void onLoginSuccess(User user);
};