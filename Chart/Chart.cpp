#include <windows.h>
#include <TCHAR.h>
#include "ChartUtil.h"
#include "resource.h"

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Chart");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	HACCEL hAccel;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;

	
	while (GetMessage(&Message, NULL, 0, 0)) {
		if (!TranslateAccelerator(hWnd, hAccel, &Message)){
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return (int)Message.wParam;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam){
	int tmp;
	switch (iMessage){
	case WM_INITDIALOG:
		index = 0;
		SetDlgItemInt(hDlg, IDC_EndX, EndX, FALSE); // 초기 설정값들을 받아온다.
		SetDlgItemInt(hDlg, IDC_EndY, EndY, FALSE);
		SetDlgItemInt(hDlg, IDC_Num, Num, FALSE);
		SetDlgItemText(hDlg, IDC_NAME, Name);
		SetDlgItemText(hDlg, IDC_XNAME, XNAME);
		SetDlgItemText(hDlg, IDC_YNAME, YNAME);

		for (int i = 0; i < Num; i++){
			SetDlgItemInt(hDlg, IDC_X, indexX[i], FALSE);
			SetDlgItemInt(hDlg, IDC_Y, indexY[i], FALSE);
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_Pre: // 좌표 x,y의 인덱스를 줄여 좌표의 해당 인덱스의 값을 설정
			index--;

			if (index < 0){
				tmp = MessageBox(hDlg, _T("오류! 좌표의 갯수보다 적습니다!"), _T("confirm"), MB_OK);
				EndDialog(hDlg, IDCANCEL);
			}

			SetDlgItemInt(hDlg, IDC_X, indexX[index], FALSE);
			SetDlgItemInt(hDlg, IDC_Y, indexY[index], FALSE);
			return TRUE;

		case IDC_Next: // 좌표 x,y의 인덱스를 더해 좌표의 해당 인덱스의 값을 설정
			index++;

			if (index > Num){
				tmp = MessageBox(hDlg, _T("오류! 좌표의 갯수보다 많습니다!"), _T("confirm"), MB_OK);
				EndDialog(hDlg, IDCANCEL);
			}

			SetDlgItemInt(hDlg, IDC_X, indexX[index], FALSE);
			SetDlgItemInt(hDlg, IDC_Y, indexY[index], FALSE);
			return TRUE;

		case IDC_Enter: // 버튼을 클릭하면 해당 설정값들을 저장한다
			for (int i = 0; i < Num; i++){
				indexX[index] = GetDlgItemInt(hDlg, IDC_X, NULL, FALSE) ;
				indexY[index] = GetDlgItemInt(hDlg, IDC_Y, NULL, FALSE) ;
			}
			return TRUE;

		case IDC_Reset: // 리셋버튼을 클릭하면 설정한 값들을 0으로 초기화 한다
			for (int i = 0; i < Num; i++){
				indexX[i] = 0;
				indexY[i] = 0;
			}
			return TRUE;

		case IDC_EnterNum: // 좌표의 갯수를 설정하여 저장한다.
			Num = GetDlgItemInt(hDlg, IDC_Num, NULL, FALSE);
			return TRUE;

		case IDOK: // OK 버튼을 클릭하면 설정된 값들을 저장한다.
			EndX = GetDlgItemInt(hDlg, IDC_EndX, NULL, FALSE);
			EndY = GetDlgItemInt(hDlg, IDC_EndY, NULL, FALSE);
			GetDlgItemText(hDlg, IDC_NAME, Name, 128);
			GetDlgItemText(hDlg, IDC_XNAME, XNAME, 128);
			GetDlgItemText(hDlg, IDC_YNAME, YNAME, 128);

			EndDialog(hDlg, IDOK);
			return TRUE;
			
		case IDCANCEL: // CANCEL 버튼을 클릭하면 대화상자를 닫는다

			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HFONT hFont, oldFont;

	static COLORREF pointCol = RGB(255,200,156);	
	static RECT clientR,Point; // 화면과 좌표의 원을 그릴 RECT변수 생성
	static POINT *tempX = new POINT[(EndX-StartX)]; // 좌표 X값을 설정할 변수 생성.
	static POINT *tempY = new POINT[(EndY-StartY)]; // 좌표 Y값을 설정할 변수 생성. 편의를 위해 POINT 함수를 2개 사용
	static POINT tempX_z, tempY_z;
	TCHAR strX[80] , strY[80]; // 좌표의 숫자를 표시하기 위한 변수 생성

	RECT Cap_rt; // 캡쳐를 위한 변수들 생성.
	HBITMAP oldBit;
	HDC memDC, hSrcDC;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH] = _T("");

	switch (iMessage) {
	case WM_CREATE: // 윈도우가 생성됬을때 변수들의 초기값을 설정하여 초기화 한다.
		hWndMain = hWnd;
		StartX = 0; StartY = 0; EndX = 10; EndY = 10; Num = 5;
		_tcscpy_s(Name,_T("Chart!"));
		_tcscpy_s(XNAME, _T("X"));
		_tcscpy_s(YNAME, _T("Y"));

		for (int i = 0; i < Num; i++){
			indexX[i] = 0;
			indexY[i] = 0;
		}
		return 0;

	case WM_SIZE: // 윈도우 사이즈의 변화가 있어도 화면에 그려진 그림이 그대로 나오게끔 한다.
		GetClientRect(hWnd, &clientR);
		init_point(bP, tempX, tempY, clientR, Num);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case ID_40001: // 메뉴중 '설정'을 눌렀을때 대화상자가 생성.
			
			if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc) == IDOK)
				init_point(bP, tempX, tempY, clientR, Num);
			
			SetWindowText(hWnd,Name);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case ID_40005: // 메뉴중 '캡쳐'를 눌렀을때, API정복 2권 비트맵을 활용한 화면 캡쳐 코드를 사용
			GetWindowRect(GetForegroundWindow(), &Cap_rt); // 화면을 캡쳐하기 위해 윈도우의 크기를 받아온다.

			Cap_rt.left = max(0, Cap_rt.left);
			Cap_rt.top = max(0, Cap_rt.top);
			Cap_rt.right = min(Cap_rt.right, GetSystemMetrics(SM_CXSCREEN));
			Cap_rt.bottom = min(Cap_rt.bottom, GetSystemMetrics(SM_CYSCREEN)); // 받아온 크기를 설정하여 캡쳐할 화면을 설정

			hSrcDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL); // 비트맵으로 저장하기 위해 DC생성
			memDC = CreateCompatibleDC(hSrcDC); // 비트맵을 저장할 메모리 DC생성
			hBit = CreateCompatibleBitmap(hSrcDC,Cap_rt.right - Cap_rt.left, Cap_rt.bottom - Cap_rt.top); // DDB파일 포멧으로 비트맵을 설정된 크기로 만든다.
			oldBit = (HBITMAP)SelectObject(memDC, hBit);

			BitBlt(memDC, 0, 0, Cap_rt.right - Cap_rt.left, Cap_rt.bottom - Cap_rt.top,hSrcDC, Cap_rt.left, Cap_rt.top, SRCCOPY); //메모리 DC에 있는 비트맵을 1:1로 복사해온다.
			SelectObject(memDC, oldBit);
			DeleteDC(memDC); 
			DeleteDC(hSrcDC); // 사용이 끝난후 다시 기존의 GDI오브젝트인 oldBit를 선탣하고, 사용이 끝난 오브젝트들을제거한다.

			memset(&OFN, 0, sizeof(OPENFILENAME)); // 캡쳐한후 파일을 저장하기위한 공용대화상자 사용
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = _T("Bmp File(*.Bmp)\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = MAX_PATH;
			OFN.lpstrDefExt = _T("bmp");
			OFN.lpstrTitle = _T("저장할 파일 이름을 입력하시오");
			if (GetSaveFileName(&OFN) != 0) 
				DDB2DIB(hBit, lpstrFile); // 파일로 저장하기 위해 DDB 포멧을 DIB포멧으로 변경하여 비트맵파일을 저장한다.
			
			break;
	
		case ID_40004: // 메뉴중 '종료'를 눌렀을때. 윈도우를 종료시킨다.
			DestroyWindow(hWnd);
			break;

		}

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBackground(hdc, bP,clientR);

		hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("Roman")); // x,y 좌표의 속성이름을 정하기 위한 폰트생성과 대화상자에서 받아온 문자열을 출력. 
		oldFont = (HFONT)SelectObject(hdc, hFont);
		TextOut(hdc, (clientR.left + clientR.right) / 2, (clientR.bottom + bP[0].y)/2, XNAME, lstrlen(XNAME));
		DeleteObject(SelectObject(hdc, oldFont));
		hFont = CreateFont(30,0,900,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_SWISS,_T("Roman"));
		oldFont = (HFONT)SelectObject(hdc, hFont);
		TextOut(hdc, ((clientR.left + bP[0].x) / 2) - 20, (clientR.bottom + clientR.top) / 2, YNAME, lstrlen(YNAME));
		DeleteObject(SelectObject(hdc,oldFont));

		wsprintf(strX, _T("%d"), StartX); // 각각의 좌표마다 정해진 숫자를 입력한다.
		TextOut(hdc, bP[0].x, bP[0].y + 5, strX, lstrlen(strX));
		wsprintf(strX, _T("%d"), EndX);
		TextOut(hdc, bP[1].x, bP[1].y + 5, strX, lstrlen(strX));
		wsprintf(strX, _T("%d"), StartY);
		TextOut(hdc, bP[0].x - 10, bP[0].y - 10, strX, lstrlen(strX));
		wsprintf(strX, _T("%d"), EndY);
		TextOut(hdc, bP[0].x - 10, bP[2].y - 10, strX, lstrlen(strX));
		
		for (int i = 1; i < EndX - StartX ; i++){
			wsprintf(strX, _T("%d"), i+StartX);
			TextOut(hdc, tempX[i].x, bP[0].y + 5, strX, lstrlen(strX));
		}
		for (int i = 1 ; i < EndY-StartY ; i++){
			wsprintf(strY, _T("%d"), i+StartY);
			TextOut(hdc, bP[0].x - 10, tempY[i].y, strY, lstrlen(strY));
		}

		
		drawRact(hdc, Point, pointCol, tempX, tempY, Num); // 그래프의 라인을 그리는 함수 호출
		drawPointLine(hdc, hWnd, tempX, tempY, Num); // 그래프의 좌표사이를 잇는 라인과 포인트마다 원을 그리는 함수 호출
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		Destroy_array(indexX, indexY, tempX, tempY);
		PostQuitMessage(0);
		return 0;

	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}