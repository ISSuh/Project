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
		SetDlgItemInt(hDlg, IDC_EndX, EndX, FALSE); // �ʱ� ���������� �޾ƿ´�.
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
		case IDC_Pre: // ��ǥ x,y�� �ε����� �ٿ� ��ǥ�� �ش� �ε����� ���� ����
			index--;

			if (index < 0){
				tmp = MessageBox(hDlg, _T("����! ��ǥ�� �������� �����ϴ�!"), _T("confirm"), MB_OK);
				EndDialog(hDlg, IDCANCEL);
			}

			SetDlgItemInt(hDlg, IDC_X, indexX[index], FALSE);
			SetDlgItemInt(hDlg, IDC_Y, indexY[index], FALSE);
			return TRUE;

		case IDC_Next: // ��ǥ x,y�� �ε����� ���� ��ǥ�� �ش� �ε����� ���� ����
			index++;

			if (index > Num){
				tmp = MessageBox(hDlg, _T("����! ��ǥ�� �������� �����ϴ�!"), _T("confirm"), MB_OK);
				EndDialog(hDlg, IDCANCEL);
			}

			SetDlgItemInt(hDlg, IDC_X, indexX[index], FALSE);
			SetDlgItemInt(hDlg, IDC_Y, indexY[index], FALSE);
			return TRUE;

		case IDC_Enter: // ��ư�� Ŭ���ϸ� �ش� ���������� �����Ѵ�
			for (int i = 0; i < Num; i++){
				indexX[index] = GetDlgItemInt(hDlg, IDC_X, NULL, FALSE) ;
				indexY[index] = GetDlgItemInt(hDlg, IDC_Y, NULL, FALSE) ;
			}
			return TRUE;

		case IDC_Reset: // ���¹�ư�� Ŭ���ϸ� ������ ������ 0���� �ʱ�ȭ �Ѵ�
			for (int i = 0; i < Num; i++){
				indexX[i] = 0;
				indexY[i] = 0;
			}
			return TRUE;

		case IDC_EnterNum: // ��ǥ�� ������ �����Ͽ� �����Ѵ�.
			Num = GetDlgItemInt(hDlg, IDC_Num, NULL, FALSE);
			return TRUE;

		case IDOK: // OK ��ư�� Ŭ���ϸ� ������ ������ �����Ѵ�.
			EndX = GetDlgItemInt(hDlg, IDC_EndX, NULL, FALSE);
			EndY = GetDlgItemInt(hDlg, IDC_EndY, NULL, FALSE);
			GetDlgItemText(hDlg, IDC_NAME, Name, 128);
			GetDlgItemText(hDlg, IDC_XNAME, XNAME, 128);
			GetDlgItemText(hDlg, IDC_YNAME, YNAME, 128);

			EndDialog(hDlg, IDOK);
			return TRUE;
			
		case IDCANCEL: // CANCEL ��ư�� Ŭ���ϸ� ��ȭ���ڸ� �ݴ´�

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
	static RECT clientR,Point; // ȭ��� ��ǥ�� ���� �׸� RECT���� ����
	static POINT *tempX = new POINT[(EndX-StartX)]; // ��ǥ X���� ������ ���� ����.
	static POINT *tempY = new POINT[(EndY-StartY)]; // ��ǥ Y���� ������ ���� ����. ���Ǹ� ���� POINT �Լ��� 2�� ���
	static POINT tempX_z, tempY_z;
	TCHAR strX[80] , strY[80]; // ��ǥ�� ���ڸ� ǥ���ϱ� ���� ���� ����

	RECT Cap_rt; // ĸ�ĸ� ���� ������ ����.
	HBITMAP oldBit;
	HDC memDC, hSrcDC;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH] = _T("");

	switch (iMessage) {
	case WM_CREATE: // �����찡 ���������� �������� �ʱⰪ�� �����Ͽ� �ʱ�ȭ �Ѵ�.
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

	case WM_SIZE: // ������ �������� ��ȭ�� �־ ȭ�鿡 �׷��� �׸��� �״�� �����Բ� �Ѵ�.
		GetClientRect(hWnd, &clientR);
		init_point(bP, tempX, tempY, clientR, Num);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case ID_40001: // �޴��� '����'�� �������� ��ȭ���ڰ� ����.
			
			if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc) == IDOK)
				init_point(bP, tempX, tempY, clientR, Num);
			
			SetWindowText(hWnd,Name);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case ID_40005: // �޴��� 'ĸ��'�� ��������, API���� 2�� ��Ʈ���� Ȱ���� ȭ�� ĸ�� �ڵ带 ���
			GetWindowRect(GetForegroundWindow(), &Cap_rt); // ȭ���� ĸ���ϱ� ���� �������� ũ�⸦ �޾ƿ´�.

			Cap_rt.left = max(0, Cap_rt.left);
			Cap_rt.top = max(0, Cap_rt.top);
			Cap_rt.right = min(Cap_rt.right, GetSystemMetrics(SM_CXSCREEN));
			Cap_rt.bottom = min(Cap_rt.bottom, GetSystemMetrics(SM_CYSCREEN)); // �޾ƿ� ũ�⸦ �����Ͽ� ĸ���� ȭ���� ����

			hSrcDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL); // ��Ʈ������ �����ϱ� ���� DC����
			memDC = CreateCompatibleDC(hSrcDC); // ��Ʈ���� ������ �޸� DC����
			hBit = CreateCompatibleBitmap(hSrcDC,Cap_rt.right - Cap_rt.left, Cap_rt.bottom - Cap_rt.top); // DDB���� �������� ��Ʈ���� ������ ũ��� �����.
			oldBit = (HBITMAP)SelectObject(memDC, hBit);

			BitBlt(memDC, 0, 0, Cap_rt.right - Cap_rt.left, Cap_rt.bottom - Cap_rt.top,hSrcDC, Cap_rt.left, Cap_rt.top, SRCCOPY); //�޸� DC�� �ִ� ��Ʈ���� 1:1�� �����ؿ´�.
			SelectObject(memDC, oldBit);
			DeleteDC(memDC); 
			DeleteDC(hSrcDC); // ����� ������ �ٽ� ������ GDI������Ʈ�� oldBit�� �����ϰ�, ����� ���� ������Ʈ���������Ѵ�.

			memset(&OFN, 0, sizeof(OPENFILENAME)); // ĸ������ ������ �����ϱ����� �����ȭ���� ���
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = _T("Bmp File(*.Bmp)\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = MAX_PATH;
			OFN.lpstrDefExt = _T("bmp");
			OFN.lpstrTitle = _T("������ ���� �̸��� �Է��Ͻÿ�");
			if (GetSaveFileName(&OFN) != 0) 
				DDB2DIB(hBit, lpstrFile); // ���Ϸ� �����ϱ� ���� DDB ������ DIB�������� �����Ͽ� ��Ʈ�������� �����Ѵ�.
			
			break;
	
		case ID_40004: // �޴��� '����'�� ��������. �����츦 �����Ų��.
			DestroyWindow(hWnd);
			break;

		}

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBackground(hdc, bP,clientR);

		hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("Roman")); // x,y ��ǥ�� �Ӽ��̸��� ���ϱ� ���� ��Ʈ������ ��ȭ���ڿ��� �޾ƿ� ���ڿ��� ���. 
		oldFont = (HFONT)SelectObject(hdc, hFont);
		TextOut(hdc, (clientR.left + clientR.right) / 2, (clientR.bottom + bP[0].y)/2, XNAME, lstrlen(XNAME));
		DeleteObject(SelectObject(hdc, oldFont));
		hFont = CreateFont(30,0,900,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_SWISS,_T("Roman"));
		oldFont = (HFONT)SelectObject(hdc, hFont);
		TextOut(hdc, ((clientR.left + bP[0].x) / 2) - 20, (clientR.bottom + clientR.top) / 2, YNAME, lstrlen(YNAME));
		DeleteObject(SelectObject(hdc,oldFont));

		wsprintf(strX, _T("%d"), StartX); // ������ ��ǥ���� ������ ���ڸ� �Է��Ѵ�.
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

		
		drawRact(hdc, Point, pointCol, tempX, tempY, Num); // �׷����� ������ �׸��� �Լ� ȣ��
		drawPointLine(hdc, hWnd, tempX, tempY, Num); // �׷����� ��ǥ���̸� �մ� ���ΰ� ����Ʈ���� ���� �׸��� �Լ� ȣ��
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		Destroy_array(indexX, indexY, tempX, tempY);
		PostQuitMessage(0);
		return 0;

	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}