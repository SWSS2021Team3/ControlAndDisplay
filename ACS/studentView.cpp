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
		case IDC_ADD_FACE_BUTTON:
			acs->addFaceImage();
			return TRUE;
		case IDC_DELETE_FACE_BUTTON:
			acs->deleteFaceImage();
			return TRUE;
		case IDC_FINISH_FACE_BUTTON:
			stop();
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

StudentView::StudentView(HINSTANCE _hInstance, HWND _hWndParent, AttendanceChecker *ac) : View(_hInstance, _hWndParent), acs(ac)
{
	m_pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	acs->setStudentViewHandler(this);
}
StudentView::~StudentView()
{
	if (m_pBitmapInfo != nullptr)
	{
		free(m_pBitmapInfo);
		m_pBitmapInfo = nullptr;
	}
}

ViewState StudentView::start()
{
	// initialize
	acs->getFaces().clear();
	acs->fetchFaceImages();

	nextState = ViewState::END;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_STUDENT_DIALOG), hWndParent, (DLGPROC)StaticDlgProc, (LPARAM)this);

	return nextState;
}

void StudentView::stop()
{
	acs->logout();
	nextState = ViewState::USERAUTH;
	EndDialog(hWnd, NULL);
}

void StudentView::showFaceImages()
{
	if (m_pBitmapInfo == nullptr) return;

	vector<cv::Mat> & frames = acs->getFaces();

	cv::Mat empty_frame = cv::Mat::zeros(160, 160, CV_8UC1);

	int faceIDCs[] = { IDC_FACE1, IDC_FACE2, IDC_FACE3, IDC_FACE4, IDC_FACE5 };

	for (int i=0; i < sizeof(faceIDCs) / sizeof(int); i++)
	{
		cv::Mat frame;
		//cv::Mat& frame = frames[i];
		if (i >= frames.size())
		{
			frame = empty_frame;
		}
		else
		{
			frame = frames[i];
		}

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

void StudentView::onFaceImageUpdate()
{
	showFaceImages();
}
