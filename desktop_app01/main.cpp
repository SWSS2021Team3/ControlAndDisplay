#include "framework.h"
#include "resource.h"
#include "attendanceChecker.h"
#include "userAuthView.h"
#include "studentView.h"
#include "attendanceView.h"

AttendanceChecker acs;

UserAuthView* userAuthView;
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
	userAuthView = new UserAuthView(hInstance, hDlgWnd, &acs);
	attendanceView = new AttendanceView(hInstance, hDlgWnd, &acs);

	ShowWindow(hDlgWnd, SW_SHOWNORMAL);
	userAuthView->show();

	MSG msg;
	while (IsWindow(hDlgWnd) && GetMessage(&msg, 0, 0, 0)) {
		if (!IsDialogMessage(hDlgWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (acs.isLogin())
		acs.logout();

	delete studentView;
	delete userAuthView;
	delete attendanceView;

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
		EndDialog(hWndDlg, wParam);
		DestroyWindow(hWndDlg);
		return TRUE;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDD_STUDENT_FORMVIEW:
			userAuthView->hide();
			studentView->show();
			attendanceView->hide();
			break;
		case IDD_USER_AUTH_FORMVIEW:
			userAuthView->show();
			studentView->hide();
			attendanceView->hide();
			break;
		case IDD_ATTENDANCE_FORMVIEW:
			userAuthView->hide();
			studentView->hide();
			attendanceView->show();
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}
