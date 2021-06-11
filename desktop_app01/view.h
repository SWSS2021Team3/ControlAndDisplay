#pragma once

#include "framework.h"

class View
{
protected:
	HWND hWnd;
	HWND hWndParent;

	static INT_PTR CALLBACK StaticDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticEventHandler(HWND hWnd, UINT message, UINT_PTR upParam, DWORD dwParam);

	virtual INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual BOOL eventHandler(HWND hWnd, UINT message, DWORD dwParam) = 0;
public:
	View(HWND _hWndParent) : hWndParent(_hWndParent) {}
	~View();
	void show();
	//void hide();
};