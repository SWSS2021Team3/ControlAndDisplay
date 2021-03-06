#include "attendanceView.h"
#include "resource.h"
#include <algorithm>


BOOL AttendanceView::eventHandler(HWND hWnd, UINT message, DWORD dwParam)
{
	if (!acs->recvVideo()) return FALSE;

	cv::Mat& frame = acs->getVideoFrame();

	renderVideo(frame);

	return TRUE;
}

void AttendanceView::renderVideo(cv::Mat& frame)
{
	if (m_pBitmapInfo == nullptr) return;

	int bpp = 8 * (int)frame.elemSize();

	int padding = 0;
	if (bpp < 32)
		padding = 4 - (frame.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;

	if (bpp < 32)
		border = 4 - (frame.cols % 4);

	cv::Mat mat_temp;
	if (border > 0 || frame.isContinuous() == false)
		cv::copyMakeBorder(frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	else
		mat_temp = frame;

	HWND hWndVideo = GetDlgItem(hWnd, IDC_VIDEO_PICTURE_CONTROL);
	RECT rect;

	GetClientRect(hWndVideo, &rect);
	HDC dc = GetDC(hWndVideo);

	//cimage_mfc.Create(rect.width, rect.height, 24);

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;
	m_pBitmapInfo->bmiHeader.biWidth = mat_temp.cols;
	m_pBitmapInfo->bmiHeader.biHeight = -mat_temp.rows;

	if (bpp == 8)
	{
		RGBQUAD* palette = m_pBitmapInfo->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}

	int destx = rect.left, desty = rect.top;
	int destw = rect.right;
	int desth = rect.bottom;

	int imgx = 0, imgy = 0;
	int imgWidth = mat_temp.cols - border;
	int imgHeight = mat_temp.rows;

	SetStretchBltMode(dc, COLORONCOLOR);

	StretchDIBits(dc,
		destx, desty, destw, desth,
		imgx, imgy, imgWidth, imgHeight,
		mat_temp.data, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	return;
}

void AttendanceView::updateUserAttendance(std::string username)
{
	std::wstring wide_string = std::wstring(username.begin(), username.end());
	const wchar_t* result = wide_string.c_str();
	WCHAR text[610];

	UINT c1 = GetDlgItemText(hWnd, IDC_ATTENDANCE_LIST_STATIC, text, 500);
	lstrcatW(text, L"\n");
	lstrcatW(text, result);
	SetDlgItemText(hWnd, IDC_ATTENDANCE_LIST_STATIC, text);
}

void AttendanceView::updateUserList()
{
	WCHAR text[1010] = L"Student List\n";
	for (std::string& s : studentList)
	{
		std::wstring wide_string = std::wstring(s.begin(), s.end());
		const wchar_t* result = wide_string.c_str();
		lstrcatW(text, L"\n");
		lstrcatW(text, result);
	}

	SetDlgItemText(hWnd, IDC_STUDENT_LIST_STATIC, text);
}

void AttendanceView::clearUserAttendance()
{
	SetDlgItemText(hWnd, IDC_ATTENDANCE_LIST_STATIC, L"Attendance List");
}

INT_PTR AttendanceView::DlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		acs->logout();
		EndDialog(hWnd, wParam);
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LOGOUT_BUTTON2:
			acs->logout();
			nextState = ViewState::USERAUTH;
			EndDialog(hWnd, NULL);
			break;
		case IDC_VIDEO_START_BUTTON:
			acs->startVideo();
			break;
		case IDC_VIDEO_END_BUTTON:
			acs->endVideo();
			break;
		case IDC_VIDEO_CLEAR_BUTTON:
			acs->clearAttendanceList();
			acs->fetchStudentList();
			break;
		case IDC_TEMP_LIVE_BUTTON:
			acs->switchToLive();
			break;
		case IDC_RECORDED_TEMP_BUTTON:
			acs->switchToRecord("record.avi");
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

AttendanceView::AttendanceView(HINSTANCE _hInstance, HWND _hWndParent, AttendanceChecker *ac) : View(_hInstance, _hWndParent), acs(ac)
{
	acs->setAttendanceViewHandler(this);
	m_pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
}

AttendanceView::~AttendanceView()
{
	if (m_pBitmapInfo != nullptr)
	{
		free(m_pBitmapInfo);
		m_pBitmapInfo = nullptr;
	}
}

ViewState AttendanceView::start()
{
	// initialize
	acs->fetchStudentList();

	nextState = ViewState::END;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ATTENDANCE_DIALOG), hWndParent, (DLGPROC)StaticDlgProc, (LPARAM)this);

	return nextState;
}

void AttendanceView::onVideoUpdate(cv::Mat& frame)
{
	renderVideo(frame);
}

void AttendanceView::onAttendanceUpdate(std::string username)
{
	updateUserAttendance(username);
	std::vector<std::string>::iterator it = find(studentList.begin(), studentList.end(), username);
	if (it != studentList.end())
		studentList.erase(it);
	updateUserList();
}

void AttendanceView::onStudentListUpdate(std::vector<std::string>& ss)
{
	studentList = ss;
	updateUserList();
	clearUserAttendance();
}
