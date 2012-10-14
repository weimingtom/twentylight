#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <commctrl.h>
#include <commdlg.h>
#include "Dib.h"

#define MAX_LOADSTRING 100

#define IDC_LEFTWINDOW 44
#define IDC_RIGHTWINDOW 45

// Global Variables:
HINSTANCE hInst = NULL; // current instance
TCHAR szTitle[MAX_LOADSTRING] = {0}; // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING] = {0}; // The title bar text
TCHAR szHello[MAX_LOADSTRING] = {0};

HWND hLeftWindow = NULL; 
HWND hRightWindow = NULL;
HWND hSplitter = NULL;
HWND hWndScrollBarVer = NULL;
HWND hWndScrollBarHor = NULL;
HWND hChild2 = NULL;
int CurWidth = 0;
BOOL split = FALSE;
WNDPROC wpOrigRightWindowProc = NULL;
//HBITMAP hbmp = NULL;
//HPALETTE hPalette = NULL;
TCHAR szWindowClass2[] = "MySubWin";
int iVerPos = 0;
int iHorPos = 0;
SCROLLINFO stScrollInfoVer = {0}, stScrollInfoHor = {0};

struct dib_struct bg1 = {0}, bg2 = {0};

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass2(HINSTANCE hInstance);
LRESULT CALLBACK WndProc2(HWND, UINT, WPARAM, LPARAM);
void OpenFontDialog(HWND hWnd);
void OpenFileAs(HWND hWnd);
void SaveFile(HWND hWnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_twentylight, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_HELLO, szHello, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// see SysTreeView32
	InitCommonControls();
	// register right window pane
	MyRegisterClass2(hInst);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_twentylight);
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_twentylight);
	wcex.hCursor = LoadCursor(NULL,IDC_SIZEWE); //LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1); //(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = (LPCSTR)IDC_twentylight;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	return RegisterClassEx(&wcex);
}

ATOM MyRegisterClass2(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_BYTEALIGNWINDOW;
	wcex.lpfnWndProc = (WNDPROC)WndProc2;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_twentylight);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1); //(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = (LPCSTR)IDC_twentylight;
	wcex.lpszClassName = szWindowClass2;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	// Store instance handle in our global variable
	hInst = hInstance; 
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	RECT Rct;
	int right;
	
	switch (message) 
	{
	case WM_CREATE:
		GetClientRect(hWnd, &Rct);
		right = Rct.right / 2;
		//| WS_VSCROLL | WS_HSCROLL
		//WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_NOTIFY,
		if (false)
		{
			if (false)
			{
				hLeftWindow = CreateWindowEx(
					WS_EX_CLIENTEDGE, "Edit", NULL, 
					WS_CHILD | WS_VISIBLE,
					0, 0, right, Rct.bottom,
					hWnd, (HMENU)IDC_LEFTWINDOW, hInst, 0);
			}
			else
			{
				/*
				SysTabControl32
				http://www.codeproject.com/KB/tabs/tabbingframework.aspx
				*/
				hLeftWindow = CreateWindowEx(
					WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, "SysTabControl32", NULL, 
					WS_OVERLAPPED | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					0, 0, right, Rct.bottom,
					hWnd, (HMENU)IDC_LEFTWINDOW, hInst, 0);
				if (hLeftWindow)
				{
					TC_ITEM TabItem;
					TabItem.mask = TCIF_TEXT;
					TabItem.pszText = _T("Log1");
					SendMessage(hLeftWindow, TCM_INSERTITEM, 0, (long)&TabItem); 
					TabItem.pszText = _T("Log2");
					SendMessage(hLeftWindow, TCM_INSERTITEM, 1, (long)&TabItem); 
				}
			}
		}
		else
		{
			/*
			http://www.ucancode.net/CTreeCtrl/TVS_CHECKBOXES-TVS_NOTOOLTIPS-TVS_HASLINES-TVS_HASBUTTONS-TVS_SHOWSELALWAYS-TVS_EDITLABELS-MFC-Tree-Control.htm
			http://msdn.microsoft.com/en-us/library/windows/desktop/bb760013(v=vs.85).aspx
			SysTreeView32
			http://msdn.microsoft.com/en-us/magazine/dd319593(VS.85).aspx
			FIXME: 
			when select Win32 Release
			LINK : warning LNK4089: all references to "comdlg32.dll" discarded by /OPT:REF
			CreateWindowEx return 0 !!!
			must call InitCommonControls() first (from comctl32.lib)
			*/
			hLeftWindow = CreateWindowEx(
				WS_EX_CLIENTEDGE, "SysTreeView32", NULL,
				WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | WS_CLIPSIBLINGS,
				0, 0, right, Rct.bottom,
				hWnd, (HMENU)IDC_LEFTWINDOW, hInst, 0);
			if (hLeftWindow)
			{
				TV_INSERTSTRUCT tvis;
				HTREEITEM hParent, hChild1, hChild2;
				HTREEITEM hLayer1, hLayer2, hLayer3;
				SendMessage(hLeftWindow, TVM_SETBKCOLOR, 0, 0xffffff);
				SendMessage(hLeftWindow, TVM_SETTEXTCOLOR, 0, 0x000000);
				SendMessage(hLeftWindow, TVM_SETINSERTMARKCOLOR, 0, 0xffffe0);
				//SendMessage(hLeftWindow, TVM_SETIMAGELIST, 0, hImageList);
				SendMessage(hLeftWindow, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
				tvis.hParent = NULL;
				tvis.hInsertAfter = TVI_ROOT;
				tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				tvis.item.pszText = "twentylight";
				tvis.item.iImage = 0;
				tvis.item.iSelectedImage = 1;
				//insert root
				hParent = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				tvis.hParent = hParent;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.pszText = "Info";
				//insert child
				hChild1 = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				tvis.item.pszText = "Layer";
				//insert child
				hChild2 = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				//insert Layer1
				tvis.hParent = hChild2;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.pszText = "Layer1";
				hLayer1 = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				tvis.item.pszText = "Layer2";
				hLayer2 = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				tvis.item.pszText = "Layer3";
				hLayer3 = (HTREEITEM)SendMessage(hLeftWindow, TVM_INSERTITEM, 0, (LPARAM)&tvis);
				/*
				expand
				http://msdn.microsoft.com/en-us/library/windows/desktop/bb773804(v=vs.85).aspx
				http://msdn.microsoft.com/en-us/library/windows/desktop/bb773568(v=vs.85).aspx
				*/
				SendMessage(hLeftWindow, TVM_EXPAND, TVE_EXPAND, (LPARAM)hParent);
				SendMessage(hLeftWindow, TVM_EXPAND, TVE_EXPAND, (LPARAM)hChild2);
			} 
			else 
			{
				MessageBox(NULL, "hLeftWindow is NULL!", "Error", MB_OK);
			}
		}
		//see MyRegisterClass2(hInst);
		hRightWindow = CreateWindowEx(
			WS_EX_CLIENTEDGE, szWindowClass2, "sub window",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			right + 4, 0, Rct.right - right - 4, Rct.bottom,
			hWnd, NULL, hInst, 0); 
		if (hRightWindow)
		{
			ShowWindow(hRightWindow, TRUE);
		}
		else
		{
			MessageBox(NULL, "hRightWindow is NULL!", "Error", MB_OK);
		}
		hSplitter = CreateWindowEx(
			0, "STATIC", NULL, 
			WS_CHILD | WS_VISIBLE,
			right, 0, 5, Rct.bottom,
			hWnd, 0, hInst, 0);
		UpdateWindow(hChild2);
		return TRUE;
		break;
	
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_ABOUT:
			   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			   break;
			case IDM_EXIT:
			   DestroyWindow(hWnd);
			   break;
			default:
			   return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_SIZE:
		{
			RECT ClientRect, WindowRect;
			int rw;
			CurWidth = LOWORD(lParam);
			if (CurWidth > 0)
			{
				GetClientRect(hWnd, &ClientRect);
				GetClientRect(hLeftWindow, &WindowRect);
				rw = CurWidth - WindowRect.right - 4;
				MoveWindow(hLeftWindow, 
					0, 0, 
					(rw > 0 ? WindowRect.right : CurWidth - 4), ClientRect.bottom, 
					TRUE);
				MoveWindow(hRightWindow, 
					WindowRect.right + 4, 0, 
					(rw > 0 ? rw : 0), ClientRect.bottom, 
					TRUE);
				MoveWindow(hSplitter, 
					(rw > 0 ? WindowRect.right : CurWidth - 4), 0, 
					4, ClientRect.bottom, 
					TRUE);
			}
		}
		break;

	case WM_LBUTTONDOWN:
		split = TRUE;
		SetCapture(hWnd);
		break;

	case WM_MOUSEMOVE:
		{
			POINT pPOINT; 
			RECT pRECT;
			if (split == TRUE)
			{
				GetCursorPos(&pPOINT);
				ScreenToClient(hWnd, &pPOINT);
				GetClientRect(hWnd, &pRECT);
				if (pPOINT.x > 3 && pPOINT.x < pRECT.right - 5)
				{
					MoveWindow(hLeftWindow, 
						0, 0, 
						LOWORD(lParam), pRECT.bottom,
						TRUE);
					//InvalidateRect(hLeftWindow, NULL, TRUE);
					MoveWindow(hRightWindow,
						LOWORD(lParam) + 4, 0, 
						pRECT.right - LOWORD(lParam) - 4, pRECT.bottom, 
						TRUE);
					//InvalidateRect(hRightWindow, NULL, TRUE);
					MoveWindow(hSplitter, 
						LOWORD(lParam), 0, 
						4, pRECT.bottom, 
						TRUE);
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		split = FALSE;
		ReleaseCapture();
		break;

	case WM_ERASEBKGND:
		return FALSE;

	case WM_NOTIFY:
		if (wParam == IDC_LEFTWINDOW)
		{
			LPNM_TREEVIEW treeview = (LPNM_TREEVIEW)lParam;
			HTREEITEM item = treeview->itemNew.hItem;
			switch(treeview->hdr.code)
			{
			case NM_CLICK:
				{
					//OutputDebugString("NM_CLICK\n");
				}
				break;
			
			case NM_DBLCLK:
				{
					//OutputDebugString("NM_DBLCLK\n");
					//OpenFontDialog(hLeftWindow);
					//OpenFileAs(hLeftWindow);
					//SaveFile(hLeftWindow);
				}
				break;
			
			case TVN_SELCHANGED:
				{
					//OutputDebugString("TVN_SELCHANGED\n");
				}
				break;
			}
		}
		break;

	//case WM_CLOSE:
	case WM_DESTROY:
		SetWindowLong(hRightWindow, GWL_WNDPROC, 
            (LONG) wpOrigRightWindowProc);
		PostQuitMessage(0);
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		iVerPos = 0;
		stScrollInfoVer.cbSize = sizeof(SCROLLINFO);
		stScrollInfoVer.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		stScrollInfoVer.nMin = 1;
		stScrollInfoVer.nMax = 100;
		stScrollInfoVer.nPos = 1;
		stScrollInfoVer.nPage = 10;
		SetScrollInfo(hWnd, SB_VERT, &stScrollInfoVer, FALSE);
		iHorPos = 0;
		stScrollInfoHor.cbSize = sizeof(SCROLLINFO);
		stScrollInfoHor.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		stScrollInfoHor.nMin = 1;
		stScrollInfoHor.nMax = 100;
		stScrollInfoHor.nPos = 1;
		stScrollInfoHor.nPage = 10;
		SetScrollInfo(hWnd, SB_HORZ, &stScrollInfoHor, FALSE);
		if (!DIBLoadPng(&bg1, "bg01a.png"))
		{
			MessageBox(hWnd, "load 1 Fail", "Load png", MB_OK);
		}
		if (true)
		{
			if (!DIBLoadPng(&bg2, "nm001.png")) //("images\\bg01a.png"))
			{
				MessageBox(hWnd, "load 2 Fail", "Load png", MB_OK);
			}
		}
		else
		{
			if (!DIBLoadBmp(&bg2, "image2.bmp"))
			{
				MessageBox(hWnd, "load 2 Fail", "Load image2.bmp", MB_OK);
			}
		}
		break;
		
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				PatBlt(hDC, 0, 0, rect.right, rect.bottom, BLACKNESS);

				if (false)
				{
					struct dib_struct temp;
					DIBCreate(&temp, 800, 600, 24);
					DIBCopy(&temp, &bg2);
					DIBDraw(&temp, hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
					DIBRelease(&temp);
				}
				else
				{
					POINT point;
					point.x = 0;
					point.y = 0;
					SIZE size;
					size.cx = 300;//432;
					size.cy = 400;//480;
					RECT fill_rect;
					fill_rect.left = fill_rect.top = 100;
					fill_rect.right = fill_rect.bottom = 200;
					COLORREF color = RGB(0, 255, 0); //RGB(255, 255, 255);
					DIBMix(&bg1, &bg2, point, size, point, color); 
					//DIBFillRect(&bg1, fill_rect, RGB(255, 0, 0));
					DIBDraw(&bg1, hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
				}

			}
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return FALSE;

	case WM_VSCROLL:
		//((HWND)lParam)
		switch(LOWORD(wParam))
		{
		case SB_LINEUP:
			iVerPos += 5;
			break;

		case SB_LINEDOWN:
			iVerPos -= 5;
			break;

		case SB_PAGEUP:
			iVerPos += 20;
			break;

		case SB_PAGEDOWN:
			iVerPos -= 20;
			break;

		case SB_THUMBPOSITION:
			iVerPos = 100 - HIWORD(wParam);
			//OutputDebugString("hello\n");
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam); 
		}
		if (iVerPos < 5)
		{
			iVerPos = 0;
		}
		else if (iVerPos > 100)
		{
			iVerPos = 100;
		}
		stScrollInfoVer.fMask = SIF_POS;
		stScrollInfoVer.nPos = 100 - iVerPos;
		SetScrollInfo(hWnd, SB_VERT, &stScrollInfoVer, TRUE);
		{
			char dbg[255] = {0};
			sprintf(dbg, "Ver:%d\n", stScrollInfoVer.nPos);
			OutputDebugString(dbg);
		}
		break;
	
	case WM_HSCROLL:
		//((HWND)lParam)
		switch(LOWORD(wParam))
		{
		case SB_LINEUP:
			iHorPos += 5;
			break;

		case SB_LINEDOWN:
			iHorPos -= 5;
			break;

		case SB_PAGEUP:
			iHorPos += 20;
			break;

		case SB_PAGEDOWN:
			iHorPos -= 20;
			break;

		case SB_THUMBPOSITION:
			iHorPos = 100 - HIWORD(wParam);
			//OutputDebugString("hello\n");
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam); 
		}
		if (iHorPos < 5)
		{
			iHorPos = 0;
		}
		else if (iHorPos > 95)
		{
			iHorPos = 100;
		}
		stScrollInfoHor.fMask = SIF_POS;
		stScrollInfoHor.nPos = 100 - iHorPos;
		SetScrollInfo(hWnd, SB_HORZ, &stScrollInfoHor, TRUE);
		{
			char dbg[255] = {0};
			sprintf(dbg, "Hor:%d\n", stScrollInfoHor.nPos);
			OutputDebugString(dbg);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OpenFontDialog(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);
	CHOOSEFONT chf;
	LOGFONT	lf;
	HFONT hFontNormal = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	GetObject(hFontNormal, sizeof(lf), &lf); 
	chf.hDC = CreateCompatibleDC(hDC);
	ReleaseDC(hLeftWindow, hDC);
	chf.lStructSize = sizeof (CHOOSEFONT);
	chf.hwndOwner = hWnd;
	chf.lpLogFont = &lf;
	chf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_LIMITSIZE; 
	chf.rgbColors = RGB (0, 0, 0);
	chf.lCustData = 0;
	chf.hInstance = hInst;
	chf.lpszStyle = (LPTSTR)NULL;
	chf.nFontType = SCREEN_FONTTYPE;
	chf.nSizeMin = 0;
	chf.nSizeMax = 20;
	chf.lpfnHook = (LPCFHOOKPROC)(FARPROC)NULL;
	if (ChooseFont(&chf)) 
	{
 		HFONT hFont = CreateFontIndirect(&lf); 
		SendMessage(hWnd, WM_SETFONT, (WPARAM) hFont, 0);
	}
}

void OpenFileAs(HWND hWnd)
{
	OPENFILENAME OpenFileName;
	TCHAR szFile[MAX_PATH] = _T("");
	OpenFileName.lStructSize = sizeof (OpenFileName);
	OpenFileName.hwndOwner = hWnd;
	OpenFileName.hInstance = hInst;
	OpenFileName.lpstrFilter = _T("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
	OpenFileName.lpstrCustomFilter = (LPTSTR)NULL;
	OpenFileName.nMaxCustFilter = 0L;
	OpenFileName.nFilterIndex = 1L;
	OpenFileName.lpstrFile = szFile;
	OpenFileName.nMaxFile = 256;
	OpenFileName.lpstrFileTitle = NULL;
	OpenFileName.nMaxFileTitle = 0;
	OpenFileName.lpstrInitialDir = NULL;
	OpenFileName.lpstrTitle = _T("Open File ...");
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = _T("*.txt");
	OpenFileName.lpfnHook = NULL;
 	OpenFileName.Flags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (!GetOpenFileName(&OpenFileName)) 
	{
		MessageBox(hWnd, "Cancel", "Open file", MB_OK);
		return;
	}
	MessageBox(hWnd, szFile, "Open file", MB_OK);
}

void SaveFile(HWND hWnd)
{
	OPENFILENAME SaveFileName;
	TCHAR szFile[MAX_PATH] = _T("");
	SaveFileName.lStructSize = sizeof (SaveFileName);
	SaveFileName.hwndOwner = hWnd;
	SaveFileName.hInstance = hInst;
	SaveFileName.lpstrFilter = _T("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
	SaveFileName.lpstrCustomFilter = (LPTSTR)NULL;
	SaveFileName.nMaxCustFilter = 0L;
	SaveFileName.nFilterIndex = 1L;
	SaveFileName.lpstrFile = szFile;
	SaveFileName.nMaxFile = 256;
	SaveFileName.lpstrFileTitle = NULL;
	SaveFileName.nMaxFileTitle = 0;
	SaveFileName.lpstrInitialDir = NULL;
	SaveFileName.lpstrTitle = _T("Save File ...");
	SaveFileName.nFileOffset = 0;
	SaveFileName.nFileExtension = 0;
	SaveFileName.lpstrDefExt = _T("*.txt");
	SaveFileName.lpfnHook = NULL;
 	SaveFileName.Flags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if(!GetSaveFileName(&SaveFileName))
	{
		MessageBox(hWnd, "Cancel", "Save file", MB_OK);
		return;
	}
	MessageBox(hWnd, szFile, "Save file", MB_OK);
}
