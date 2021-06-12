#include "studentView.h"
#include "resource.h"

BOOL StudentView::eventHandler(HWND hWnd, UINT message, DWORD dwParam)
{
	return TRUE;
}

INT_PTR StudentView::DlgProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		KillTimer(hWnd, 1000);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_ADD_FACE_BUTTON:
			acs->fetchFaceImages();
			return TRUE;
		case IDC_FINISH_FACE_BUTTON:
			showFaceImages();
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

StudentView::StudentView(HINSTANCE hInstance, HWND _hWndParent, AttendanceChecker *ac) : View(_hWndParent), acs(ac)
{
	hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_STUDENT_FORMVIEW), _hWndParent, (DLGPROC)StaticDlgProc, (LPARAM) this);
}

void StudentView::start()
{
	show();
}

void StudentView::showFaceImages()
{
	vector<cv::Mat> & frames = acs->getFaces();

	int faceIDCs[] = { IDC_FACE1, IDC_FACE2, IDC_FACE3, IDC_FACE4, IDC_FACE5 };

	for (int i=0; i < min(sizeof(faceIDCs) / sizeof(int), frames.size()); i++)
	{
		cv::Mat& frame = frames[i];

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

		HWND hWndVideo = GetDlgItem(hWnd, faceIDCs[i]);
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

	}
}