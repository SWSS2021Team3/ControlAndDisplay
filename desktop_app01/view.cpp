#include "view.h"
#include "resource.h"

View::~View()
{
	if (hWnd) DestroyWindow(hWnd);
}

INT_PTR CALLBACK View::StaticDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	View* view = reinterpret_cast<View*>(GetWindowLongPtr(hWndDlg, GWLP_USERDATA));
	if (!view)
	{
		if (message == WM_INITDIALOG)
		{
			view = reinterpret_cast<View*>(lParam);
			view->hWnd = hWndDlg;
			SetWindowLongPtr(view->hWnd, GWLP_USERDATA, lParam);
		}
		else
		{
			return 0; //let system deal with message
		}
	}
	//forward message to member function handler
	return view->DlgProc(message, wParam, lParam);
}

BOOL CALLBACK View::StaticEventHandler(HWND hWnd, UINT message, UINT_PTR upParam, DWORD dwParam)
{
	View* view = (View*)upParam;

	return view->eventHandler(hWnd, message, dwParam);
}

void View::show()
{
	ShowWindow(hWnd, SW_SHOWNORMAL);
}
