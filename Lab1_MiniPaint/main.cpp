#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <string>
#include <vector>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

#define IDC_TOOLBAR (HMENU) 101
#define IDC_TOOLBAR_IMGLIST (HMENU) 102
#define IDC_BUTTON_DRAWLINE (HMENU) 103
#define IDC_BUTTON_DRAWCURV (HMENU) 104
#define IDC_BUTTON_DRAWRECT (HMENU) 105
#define IDC_BUTTON_DRAWPOLY (HMENU) 106
#define IDC_BUTTON_DRAWELPS (HMENU) 107
#define IDC_BUTTON_DRAWTEXT (HMENU) 108
#define IDC_BUTTON_CNGCOLOR (HMENU) 109
#define IDC_BUTTON_CNGBRUSH (HMENU) 110
#define IDC_BUTTON_CNGWIDTH (HMENU) 111
#define IDC_TRACKBAR (HMENU) 112

enum class Figures {Line = 0, Curv = 1, Rect = 2, Poly = 3, Elps = 4, Text = 5};
std::vector<Point> poly;

typedef struct TMainWindow{
	HWND hWnd;
	std::wstring text;
	
	RECT canvas;
	HDC memDC;
	HBITMAP memBitmap;
	HWND hWndToolbar;
	HWND hWndTrackbar;
	HIMAGELIST hImageList;
	HICON imgLine;
	HICON imgCurved;
	HICON imgRect;
	HICON imgPentagon;
	HICON imgEllipse;
	HICON imgText;
	HICON imgColor;
	HICON imgWidth;
	HICON imgBrush;
	Color drawColor;
	Color brushColor;
	DWORD lineWidth;
	Figures selectedFigure;
	Point from;
	Point to;
	bool isDrawing;

}TMainWindow, *PMainWindow;

void CreateToolbar(PMainWindow p) {
	const int ImageListID = 0;
	const int numButtons = 8;
	const int bitmapSize = 32;
	const DWORD buttonStyles = BTNS_AUTOSIZE;

	p->hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_FLAT | TBSTYLE_WRAPABLE | WS_VISIBLE , 0, 0, 0, 0, p->hWnd, IDC_TOOLBAR,0 , NULL);
	p->hImageList = ImageList_Create(bitmapSize, bitmapSize, ILC_MASK, numButtons, 0);	
	SendMessage(p->hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)p->hImageList);

	p->imgLine = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\line.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgCurved = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\curv.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgRect = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\rect.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgPentagon = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\poly.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgEllipse = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\elps.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgText = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\text.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgColor = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\color.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	p->imgBrush = (HICON)LoadImage(NULL, _T("..\\Lab1_MiniPaint\\Icons\\brush.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	ImageList_AddIcon(p->hImageList, p->imgLine);
	ImageList_AddIcon(p->hImageList, p->imgCurved);
	ImageList_AddIcon(p->hImageList, p->imgRect);
	ImageList_AddIcon(p->hImageList, p->imgPentagon);
	ImageList_AddIcon(p->hImageList, p->imgEllipse);
	ImageList_AddIcon(p->hImageList, p->imgText);
	ImageList_AddIcon(p->hImageList, p->imgColor);
	ImageList_AddIcon(p->hImageList, p->imgBrush);	

	TBBUTTON tbButtons[numButtons] =
	{
		{0,(int)IDC_BUTTON_DRAWLINE,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Line"},
		{1,(int)IDC_BUTTON_DRAWCURV,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Curv"},
		{2,(int)IDC_BUTTON_DRAWRECT,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Rect"},
		{3,(int)IDC_BUTTON_DRAWPOLY,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Poly"},
		{4,(int)IDC_BUTTON_DRAWELPS,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Elps"},
		{5,(int)IDC_BUTTON_DRAWTEXT,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Text"},
		{6,(int)IDC_BUTTON_CNGCOLOR,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Color"},
		{7,(int)IDC_BUTTON_CNGBRUSH,TBSTATE_ENABLED,buttonStyles,{0},0,(INT_PTR)L"Brush"}
	};

	SendMessage(p->hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(p->hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);
	SendMessage(p->hWndToolbar, TB_AUTOSIZE, 0, 0);
	
	p->hWndTrackbar = CreateWindowEx(NULL, TRACKBAR_CLASS, L"Trackbar", WS_CHILD | WS_VISIBLE | TBS_FIXEDLENGTH | TBS_AUTOTICKS,
		315, 0, 300, 50, p->hWnd, (HMENU)IDC_TRACKBAR, NULL, NULL);
	SendMessage(p->hWndTrackbar, TBM_SETRANGE, TRUE, 0b1010<<16);
	SendMessage(p->hWndTrackbar, TBM_SETTHUMBLENGTH, 35, 0);
	p->lineWidth = SendMessage(p->hWndTrackbar, TBM_GETPOS, 0, 0);
	SetParent(p->hWndTrackbar, p->hWndToolbar);
}

void ChoosePaintColor(HWND owner, Color* c) {
	COLORREF arcCustClr[16];
	CHOOSECOLOR color = {};
	color.lStructSize = sizeof(CHOOSECOLOR);
	color.lpCustColors = (LPDWORD)arcCustClr;
	color.hwndOwner = owner;
	color.rgbResult = 0;
	color.Flags = CC_RGBINIT | CC_FULLOPEN;
	if (ChooseColor(&color)== TRUE) {
		c->SetFromCOLORREF(color.rgbResult);
	}
}

void DrawFigure(PMainWindow p, bool isPreview) {
	if (p->isDrawing) {
		HDC hdc;
		if (isPreview) {
			hdc = GetDC(p->hWnd);
		}
		else {
			hdc = p->memDC;
		}

		Graphics graphics(hdc);
		Pen pen(Color(p->drawColor), (Gdiplus::REAL)p->lineWidth);
		SolidBrush brush(Color(p->brushColor));
		std::wstring tmp = p->text;
		switch (p->selectedFigure)
		{
		case Figures::Line:
		case Figures::Curv:
			graphics.DrawLine(&pen, p->from, p->to);
			break;
		case Figures::Rect:			
			graphics.DrawRectangle(&pen, p->from.X,p->from.Y, p->to.X-p->from.X,p->to.Y-p->from.Y);
			graphics.FillRectangle(&brush, p->from.X+1, p->from.Y+1, p->to.X - p->from.X-1, p->to.Y - p->from.Y-1);
			break;
		case Figures::Poly:
			graphics.DrawLine(&pen, p->from, p->to);
				graphics.FillPolygon(&brush, &poly[0], poly.size());
			
			break;
		case Figures::Elps:
			graphics.DrawEllipse(&pen, p->from.X, p->from.Y, p->to.X - p->from.X, p->to.Y - p->from.Y);
			graphics.FillEllipse(&brush, p->from.X + 1, p->from.Y + 1, p->to.X - p->from.X - 1, p->to.Y - p->from.Y - 1);
			break;
		case Figures::Text:
			graphics.DrawString(tmp.c_str(), tmp.length(), &Font(L"Arial", (Gdiplus::REAL)(p->lineWidth*4)), PointF(p->from.X, p->from.Y), &SolidBrush(Color(p->drawColor)));
			break;
		default:
			break;
		}
		if (isPreview) {
			ReleaseDC(p->hWnd, hdc);
		}
	}
		
	
}

LRESULT CALLBACK WndProc(_In_ HWND   hWnd, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {	
	PMainWindow pSelf;

	if (uMsg == WM_CREATE) {
		pSelf = (PMainWindow)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TMainWindow));
		if (pSelf != NULL) {
			pSelf->hWnd = hWnd;			
			CreateToolbar(pSelf);
			pSelf->drawColor = Color(255, 0, 0, 0);
			pSelf->brushColor = Color(255, 255, 255, 255);
			pSelf->lineWidth = 3;
			pSelf->isDrawing = false;	
			pSelf->text.push_back('a');
			pSelf->text.pop_back();
			poly.clear();

			GetClientRect(pSelf->hWnd, &pSelf->canvas);
			
			HDC tmp = GetWindowDC(hWnd);
			pSelf->memDC = CreateCompatibleDC(tmp);
			pSelf->memBitmap = CreateCompatibleBitmap(tmp, pSelf->canvas.right-pSelf->canvas.left, pSelf->canvas.bottom-pSelf->canvas.top);			
			SelectObject(pSelf->memDC, pSelf->memBitmap);
			RECT r = pSelf->canvas;	
			FillRect(pSelf->memDC, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
			ReleaseDC(hWnd,tmp);

			SetWindowLong(hWnd, 0, (LONG)pSelf);
			
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	PAINTSTRUCT ps;
	HDC hdc;

	pSelf = (PMainWindow)GetWindowLong(hWnd, 0);
	if (!pSelf) {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	switch(uMsg) {
		case WM_PAINT:			
			hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
				ps.rcPaint.bottom - ps.rcPaint.top, pSelf->memDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
			
			DrawFigure(pSelf, true);

			EndPaint(hWnd, &ps);
			break;
		case WM_KEYDOWN:
			if (pSelf->isDrawing && pSelf->selectedFigure == Figures::Text) {
				if ((wParam >= 0x30 && wParam <= 0x39) || (wParam >= 0x41 && wParam <= 0x5A)) {
					pSelf->text.push_back(wParam);
				}
				if (wParam == VK_BACK && pSelf->text.length() != 0) {
					pSelf->text.pop_back();					
				}
				InvalidateRect(hWnd, &pSelf->canvas, TRUE);
			}
			
			break;
		case WM_COMMAND:
			if ((HMENU)LOWORD(wParam) == IDC_BUTTON_CNGCOLOR && HIWORD(wParam) == BN_CLICKED) {
				ChoosePaintColor(hWnd,&pSelf->drawColor);
				
			}
			if ((HMENU)LOWORD(wParam) == IDC_BUTTON_CNGBRUSH && HIWORD(wParam) == BN_CLICKED) {
				ChoosePaintColor(hWnd, &pSelf->brushColor);
				
			}
			if ((HMENU)LOWORD(wParam) >= IDC_BUTTON_DRAWLINE && (HMENU)LOWORD(wParam) <= IDC_BUTTON_DRAWTEXT && !pSelf->isDrawing) {
				pSelf->selectedFigure = static_cast<Figures>(LOWORD(wParam) - (int)IDC_BUTTON_DRAWLINE);
				
			}
			break;
		case WM_HSCROLL:
			if ((HANDLE)lParam == pSelf->hWndTrackbar && LOWORD(wParam) == TB_THUMBPOSITION) {
				pSelf->lineWidth = (HIWORD(wParam) + 1)*3;
				SetFocus(hWnd);
			}
			break;
		case WM_MOUSEMOVE:
			if (pSelf->isDrawing) {				
				pSelf->to.X = LOWORD(lParam);
				pSelf->to.Y = HIWORD(lParam);
				InvalidateRect(hWnd, &pSelf->canvas, FALSE);				
			}
			break;
		case WM_LBUTTONDOWN:
			if (pSelf->isDrawing && (pSelf->selectedFigure == Figures::Curv || pSelf->selectedFigure == Figures::Poly)) {				
				pSelf->to.X = LOWORD(lParam);
				pSelf->to.Y = HIWORD(lParam);
				DrawFigure(pSelf, false);				
				pSelf->from.X = LOWORD(lParam);
				pSelf->from.Y = HIWORD(lParam);
				if (pSelf->selectedFigure == Figures::Poly) {
					poly.push_back(pSelf->from);
				}
							
			}
			if (!(pSelf->isDrawing)) { 
				pSelf->from.X = LOWORD(lParam);
				pSelf->from.Y = HIWORD(lParam);
				if (pSelf->selectedFigure == Figures::Poly) {
					poly.push_back(pSelf->from);
				}
				pSelf->isDrawing = true;				
			}			
			break;
		case WM_RBUTTONDOWN:
			if (pSelf->isDrawing) {
				if (pSelf->selectedFigure == Figures::Poly) {
					if (poly.size()>=3) {
						pSelf->to.X = LOWORD(lParam);
						pSelf->to.Y = HIWORD(lParam);
						poly.push_back(pSelf->to);
						pSelf->from = poly[poly.size() - 1];
						pSelf->to.X = poly[0].X;
						pSelf->to.Y = poly[0].Y;
						DrawFigure(pSelf, false);
						pSelf->isDrawing = false;
						poly.clear();
						InvalidateRect(hWnd, &pSelf->canvas, FALSE);
						
					}
					break;
					
				}
				pSelf->to.X = LOWORD(lParam);
				pSelf->to.Y = HIWORD(lParam);				
				DrawFigure(pSelf, false);				
				pSelf->isDrawing = false;
				if (pSelf->selectedFigure == Figures::Text) {
					pSelf->text.clear();
				}				
			}
			break;
		case WM_DESTROY:
			DestroyIcon(pSelf->imgLine);
			DestroyIcon(pSelf->imgCurved);
			DestroyIcon(pSelf->imgRect);
			DestroyIcon(pSelf->imgPentagon);
			DestroyIcon(pSelf->imgEllipse);
			DestroyIcon(pSelf->imgText);
			DestroyIcon(pSelf->imgColor);
			DestroyIcon(pSelf->imgBrush);
			DestroyIcon(pSelf->imgWidth);
			DeleteObject(pSelf->memBitmap);
			DeleteDC(pSelf->memDC);
			HeapFree(GetProcessHeap(), 0, pSelf);
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int CALLBACK wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd ) {

	const wchar_t* className = L"MainWindowClass";
	const wchar_t* title = L"MiniPaint";

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof icc;
	icc.dwICC = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icc);

	WNDCLASSEX wcex = { };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	//self pointer memory reservation
	wcex.cbWndExtra = 4;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, _T("Failed to register main class"), title,NULL);
		return -1;
	}
	HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_COMPOSITED | WS_EX_CONTROLPARENT | WS_EX_OVERLAPPEDWINDOW,
		className, title, WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
	
	if (!hWnd) {
		
		HRESULT tmp = HRESULT_FROM_WIN32(GetLastError());
		MessageBox(NULL, _T("Failed to create main window"), title, NULL);
		return -1;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		int tmp = CommDlgExtendedError();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;

}

