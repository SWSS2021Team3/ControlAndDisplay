#pragma once

#include "framework.h"

enum class ViewState
{
	END,
	USERAUTH,
	STUDENT,
	ATTENDANCE
};

class View
{
protected:
	HWND hWndParent;
	ViewState nextState;

	static INT_PTR CALLBACK StaticDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticEventHandler(HWND hWnd, UINT message, UINT_PTR upParam, DWORD dwParam);

	virtual INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam) = 0;
public:
	HWND hWnd;
	HINSTANCE hInstance;
	View(HWND _hWndParent) : hWndParent(_hWndParent) {}
	View(HINSTANCE _hInstance, HWND _hWndParent) : hInstance(_hInstance), hWndParent(_hWndParent) {}
	~View();
	virtual ViewState start() = 0;
	void show();
	void hide();
};