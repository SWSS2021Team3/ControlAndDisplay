#include "framework.h"
#include "resource.h"
#include "client.h"

Client client;
BITMAPINFO* m_pBitmapInfo = nullptr;

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	return (int) DialogBoxW(hInstance, MAKEINTRESOURCE(IDD_ATTENDANCE_DIALOG), nullptr, (DLGPROC)DlgProc);
}

BOOL CALLBACK RecvVideo(HWND hWnd, UINT message, UINT_PTR upParam, DWORD dwParam)
{
	if (!client.recvVideo()) return FALSE;

	cv::Mat& frame = client.getVideoFrame();

	int bpp = 8 * (int) frame.elemSize();

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

	return TRUE;
}

BOOL CALLBACK DlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if (m_pBitmapInfo == nullptr)
			m_pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		KillTimer(hWndDlg, 1000);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (!client.isConnected())
			{
				int res = MessageBox(hWndDlg, L"Connect ?", L"", MB_YESNO);
				if (res == 6)
				{
					bool connected = client.connect("127.0.0.1", "5000");
					if (connected)
					{
						SetTimer(hWndDlg, 1000, 30, (TIMERPROC)RecvVideo);
					}
				}
			}
			else
			{
				int res = MessageBox(hWndDlg, L"Disconnect ?", L"", MB_YESNO);
				if (res == 6)
				{
					KillTimer(hWndDlg, 1000);
					client.disconnect();
				}
			}
			break;
		case IDCANCEL:
			EndDialog(hWndDlg, wParam);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}
