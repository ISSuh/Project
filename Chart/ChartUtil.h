BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBackground(HDC hdc, POINT bP[], RECT &clientR);
void init_point(POINT bP[], POINT tempX[], POINT tempY[], RECT &clientR, int Num);
void drawPointLine(HDC hdc, HWND hWnd, POINT tempX[], POINT tempY[], int Num);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF color, POINT tempX[], POINT tempY[], int Num);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF penC, COLORREF brushC, POINT tempX[], POINT tempY[], int Num);
void Destroy_array(int ArrayX[], int ArrayY[], POINT tempX[], POINT tempY[]);
void DDB2DIB(HBITMAP hbit, LPCTSTR Path);

int StartX, StartY, EndX, EndY, Num, X, Y, index; // 함수에 필요한 좌표의 범위, 좌표의 갯수,인덱스, 좌표값등의 정보들을 저장하는 변수를 전역변수로 선언.
int *indexX = new int[Num];
int *indexY = new int[Num];
POINT bP[3];
int lineX, lineY;
TCHAR Name[128], XNAME[128], YNAME[128];
HBITMAP hBit = NULL;


void init_point(POINT bP[], POINT tempX[], POINT tempY[], RECT &clientR, int Num){ // 그래프의 기본이 되는 막대의 좌표와 각각의 포인트 좌표를 설정한다.
	double tmpX, tmpY;

	bP[0].x = clientR.left + 80;
	bP[0].y = clientR.bottom - 76;
	bP[1].x = clientR.right - 79;
	bP[1].y = bP[0].y;
	bP[2].x = bP[0].x;
	bP[2].y = clientR.top + 20;

	lineX = bP[1].x - bP[0].x;
	lineY = bP[2].y - bP[0].y;
	tmpX = lineX / EndX - StartX;
	tmpY = lineY / EndY - StartY;

	for (int i = 1; i <= EndX - StartX; i++){
		tempX[i].x = (i*tmpX) + bP[0].x;
		if (StartX != 0){
			tmpX = lineX / ((EndX - StartX) - (StartX - StartX));
			tempX[i].x = (i*tmpX) + bP[0].x;
		}
	}

	for (int i = 1; i <= EndY - StartY; i++){
		tempY[i].y = (i*tmpY) + bP[0].y;
		if (StartY != 0){
			tmpY = lineY / ((EndY - StartY) - (StartY - StartY));
			tempY[i].y = (i*tmpY) + bP[0].y;
		}
	}
}

void drawPointLine(HDC hdc, HWND hWnd, POINT tempX[], POINT tempY[], int Num){ // 좌표를 설정후, 좌표와 좌표를 잇기 위한 선과 좌표점을 확인하기 위한 원을 그린다.
	hdc = GetDC(hWnd);
	HPEN hPen, oldPen;
	hPen = CreatePen(PS_SOLID, 2, RGB(rand() % 255, rand() % 255, rand() % 255));
	oldPen = (HPEN)SelectObject(hdc, hPen);

	for (int i = 0; i < Num; i++)
		Ellipse(hdc, (tempX[indexX[i]].x - 5), (tempY[indexY[i]].y - 5), (tempX[indexX[i]].x + 5), (tempY[indexY[i]].y) + 5);

	for (int i = 0; i < Num - 1; i++){
		if (indexX[i + 1] != NULL){
			MoveToEx(hdc, tempX[indexX[i]].x, tempY[indexY[i]].y, NULL);
			LineTo(hdc, tempX[indexX[i + 1]].x, tempY[indexY[i + 1]].y);
		}
	}

	DeleteObject(SelectObject(hdc, oldPen));
	ReleaseDC(hWnd, hdc);
}

void DrawBackground(HDC hdc, POINT bP[], RECT &clientR){ // 그래프의 기본이 되는 라인을 그린다.
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[1].x, bP[1].y);
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[2].x, bP[2].y);
}

void drawRact(HDC hdc, RECT &Point, COLORREF color, POINT tempX[], POINT tempY[], int Num){  // 화면에 그림을 그리는 함수.
	drawRact(hdc, Point, color, color, tempX, tempY, Num);
}

void drawRact(HDC hdc, RECT &Point, COLORREF penC, COLORREF brushC, POINT tempX[], POINT tempY[], int Num){ // 화면에 각각 x,y좌표에 선을 그려 알아보기 쉽게 한다.
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	hPen = CreatePen(PS_DOT, 1, penC);
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(brushC);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	for (int i = 1; i < EndY - StartY; i++){
		MoveToEx(hdc, bP[0].x, tempY[i].y, NULL);
		LineTo(hdc, bP[1].x, tempY[i].y);
	}
	MoveToEx(hdc, bP[0].x, bP[2].y, NULL);
	LineTo(hdc, bP[1].x, bP[2].y);

	for (int i = 1; i < EndX - StartX; i++){
		MoveToEx(hdc, tempX[i].x, bP[0].y, NULL);
		LineTo(hdc, tempX[i].x, bP[2].y);
	}
	MoveToEx(hdc, bP[1].x, bP[1].y, NULL);
	LineTo(hdc, bP[1].x, bP[2].y);

	DeleteObject(SelectObject(hdc, hOldPen));
	DeleteObject(SelectObject(hdc, hOldBrush));
}

void Destroy_array(int indexX[], int indexY[], POINT tempX[], POINT tempY[]){ //동적할당한 변수들을 제거하는 함수
	delete[]tempX;
	delete[]tempY;
	delete[]indexX;
	delete[]indexY;
}

void DDB2DIB(HBITMAP hbit, LPCTSTR Path) // API정복 2권 비트맵을 활용한 화면 캡쳐 코드를 사용
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	BITMAP bit;
	BITMAPINFO *pih;
	int PalSize;
	HANDLE hFile;
	DWORD dwWritten, Size;
	HDC hdc;

	hdc = GetDC(NULL); // 전체 화면에 대한 DC를 구한다.

	GetObject(hbit, sizeof(BITMAP), &bit); // 비트맵 정보로부터 정보 구조체를 초기화한다.
	ih.biSize = sizeof(BITMAPINFOHEADER);
	ih.biWidth = bit.bmWidth;
	ih.biHeight = bit.bmHeight;
	ih.biPlanes = 1;
	ih.biBitCount = bit.bmPlanes*bit.bmBitsPixel;
	ih.biCompression = BI_RGB;
	ih.biSizeImage = 0;
	ih.biXPelsPerMeter = 0;
	ih.biYPelsPerMeter = 0;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;

	PalSize = (ih.biBitCount > 8 ? 0 : 1 << ih.biBitCount)*sizeof(RGBQUAD); // 정보 구조체 팔레트 크기만큼 메모리를 할당하고 이 버퍼에 정보 구조체를 복사한다.
	pih = (BITMAPINFO *)malloc(ih.biSize + PalSize);
	pih->bmiHeader = ih;

	GetDIBits(hdc, hbit, 0, bit.bmHeight, NULL, pih, DIB_RGB_COLORS); // 비트맵의 크기를 구한다.
	ih = pih->bmiHeader;

	if (ih.biSizeImage == 0)
		ih.biSizeImage = ((((ih.biWidth*ih.biBitCount) + 31) & ~31) >> 3) * ih.biHeight; // 비트맵 크기가 구해지지 않았을 경우 수작업으로 직접 계산한다.

	Size = ih.biSize + PalSize + ih.biSizeImage; 	// 래스터 데이터를 읽기위해 메모를 재할당한다.
	pih = (BITMAPINFO *)realloc(pih, Size);

	GetDIBits(hdc, hbit, 0, bit.bmHeight, (PBYTE)pih + ih.biSize + PalSize, pih, DIB_RGB_COLORS); 	// 래스터 데이터를 읽어들인다.

	fh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PalSize; 	// 파일 헤더를 만든다.
	fh.bfReserved1 = 0;
	fh.bfReserved2 = 0;
	fh.bfSize = Size + sizeof(BITMAPFILEHEADER);
	fh.bfType = 0x4d42;

	hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 	// 파일을 생성하고 파일 헤더와 정보 구조체, 팔레트, 래스터 데이터를 출력한다.
	WriteFile(hFile, &fh, sizeof(fh), &dwWritten, NULL);
	WriteFile(hFile, pih, Size, &dwWritten, NULL);

	free(pih);
	ReleaseDC(NULL, hdc);
	CloseHandle(hFile);
}