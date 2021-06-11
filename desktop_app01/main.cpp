#include "framework.h"
#include "resource.h"
#include "attendanceChecker.h"
#include "studentView.h"
#include "attendanceView.h"

AttendanceChecker acs;

StudentView* studentView;
AttendanceView* attendanceView;

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HWND hDlgWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ACS_DIALOG), nullptr, (DLGPROC)DlgProc);

	studentView = new StudentView(hInstance, hDlgWnd, &acs);
	attendanceView = new AttendanceView(hInstance, hDlgWnd, &acs);

	ShowWindow(hDlgWnd, SW_SHOWNORMAL);
	studentView->show();
	acs.showFaceImages();
	//attendanceView->show();

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		if (!IsDialogMessage(hDlgWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete studentView;
	delete attendanceView;

	DestroyWindow(hDlgWnd);

	return 0;
}


BOOL CALLBACK RecvVideo2(HWND hWnd, UINT message, UINT_PTR upParam, DWORD dwParam)
{
	return TRUE;
}

BOOL CALLBACK DlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		KillTimer(hWndDlg, 1000);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hWndDlg, wParam);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}
