#include "userAuthView.h"
#include "resource.h"

BOOL UserAuthView::eventHandler(HWND hWnd, UINT message, DWORD dwParam)
{
	return TRUE;
}

std::string wstr2string(const LPWSTR lpwstr)
{
	std::wstring ws(lpwstr);
	std::string s;
	std::transform(ws.begin(), ws.end(), std::back_inserter(s), [](WCHAR c) { return (char)c;});
	return s;
}

INT_PTR UserAuthView::DlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hWnd, IDC_CMODE_RADIO1, BM_SETCHECK, 1, 0);
		SendDlgItemMessage(hWnd, IDC_CMODE_RADIO2, BM_SETCHECK, 0, 0);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, wParam);
		return TRUE;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LOGIN_BUTTON:
		{
			bool secureMode = SendDlgItemMessage(hWnd, IDC_CMODE_RADIO1, BM_GETCHECK, 0, 0) == BST_CHECKED;
			WCHAR username[100];
			WCHAR password[100];
			UINT c1 = GetDlgItemText(hWnd, IDC_USERNAME_EDIT, username, 100);
			UINT c2 = GetDlgItemText(hWnd, IDC_PASSWORD_EDIT, password, 100);
			//WCHAR s[300];
			//wsprintf(s, L"Login button clicked! %d %s %s %d %d", secureMode, username, password, c1, c2);
			//MessageBox(hWnd, s, L"", MB_OK);
			int ret = acs->login(wstr2string(username), wstr2string(password), secureMode);
			if (ret == -1024) {
				MessageBox(hWnd, _T("Unable Acess USB Key. Not Connected"), _T("Warning"), MB_OK | MB_ICONERROR);
			}
			else if (ret == -1025) {
				MessageBox(hWnd, _T("Configuration file is missing or changed. Not Connected"), _T("Warning"), MB_OK | MB_ICONERROR);
			}
			return TRUE;
		}

		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

UserAuthView::UserAuthView(HINSTANCE _hInstance, HWND _hWndParent, AttendanceChecker* ac) : View(_hInstance, _hWndParent), acs(ac)
{
	acs->setUserAuthViewHandler(this);
}

ViewState UserAuthView::start()
{
	nextState = ViewState::END;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_USER_AUTH_DIALOG), hWndParent, (DLGPROC)StaticDlgProc, (LPARAM)this);

	return nextState;
}

void UserAuthView::onConnectFailed()
{
	MessageBox(hWnd, L"Connect Fail.", L"", MB_OK);
}

void UserAuthView::onLoginFailed()
{
	MessageBox(hWnd, L"Login Fail.", L"", MB_OK);
}

void UserAuthView::onLoginSuccess(User user)
{
	WCHAR msg[300];
	wsprintf(msg, L"Welcome");
	MessageBox(hWnd, msg, L"", MB_OK);

	nextState = user.isAdmin ? ViewState::ATTENDANCE:ViewState::STUDENT;
	EndDialog(hWnd, MB_OK);
}
