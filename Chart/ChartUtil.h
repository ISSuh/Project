BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBackground(HDC hdc, POINT bP[], RECT &clientR);
void init_point(POINT bP[], POINT tempX[], POINT tempY[], RECT &clientR, int Num);
void drawPointLine(HDC hdc, HWND hWnd, POINT tempX[], POINT tempY[], int Num);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF color, POINT tempX[], POINT tempY[], int Num);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF penC, COLORREF brushC, POINT tempX[], POINT tempY[], int Num);
void Destroy_array(int ArrayX[], int ArrayY[], POINT tempX[], POINT tempY[]);
void DDB2DIB(HBITMAP hbit, LPCTSTR Path);

int StartX, StartY, EndX, EndY, Num, X, Y, index; // �Լ��� �ʿ��� ��ǥ�� ����, ��ǥ�� ����,�ε���, ��ǥ������ �������� �����ϴ� ������ ���������� ����.
int *indexX = new int[Num];
int *indexY = new int[Num];
POINT bP[3];
int lineX, lineY;
TCHAR Name[128], XNAME[128], YNAME[128];
HBITMAP hBit = NULL;


void init_point(POINT bP[], POINT tempX[], POINT tempY[], RECT &clientR, int Num){ // �׷����� �⺻�� �Ǵ� ������ ��ǥ�� ������ ����Ʈ ��ǥ�� �����Ѵ�.
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

void drawPointLine(HDC hdc, HWND hWnd, POINT tempX[], POINT tempY[], int Num){ // ��ǥ�� ������, ��ǥ�� ��ǥ�� �ձ� ���� ���� ��ǥ���� Ȯ���ϱ� ���� ���� �׸���.
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

void DrawBackground(HDC hdc, POINT bP[], RECT &clientR){ // �׷����� �⺻�� �Ǵ� ������ �׸���.
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[1].x, bP[1].y);
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[2].x, bP[2].y);
}

void drawRact(HDC hdc, RECT &Point, COLORREF color, POINT tempX[], POINT tempY[], int Num){  // ȭ�鿡 �׸��� �׸��� �Լ�.
	drawRact(hdc, Point, color, color, tempX, tempY, Num);
}

void drawRact(HDC hdc, RECT &Point, COLORREF penC, COLORREF brushC, POINT tempX[], POINT tempY[], int Num){ // ȭ�鿡 ���� x,y��ǥ�� ���� �׷� �˾ƺ��� ���� �Ѵ�.
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

void Destroy_array(int indexX[], int indexY[], POINT tempX[], POINT tempY[]){ //�����Ҵ��� �������� �����ϴ� �Լ�
	delete[]tempX;
	delete[]tempY;
	delete[]indexX;
	delete[]indexY;
}

void DDB2DIB(HBITMAP hbit, LPCTSTR Path) // API���� 2�� ��Ʈ���� Ȱ���� ȭ�� ĸ�� �ڵ带 ���
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	BITMAP bit;
	BITMAPINFO *pih;
	int PalSize;
	HANDLE hFile;
	DWORD dwWritten, Size;
	HDC hdc;

	hdc = GetDC(NULL); // ��ü ȭ�鿡 ���� DC�� ���Ѵ�.

	GetObject(hbit, sizeof(BITMAP), &bit); // ��Ʈ�� �����κ��� ���� ����ü�� �ʱ�ȭ�Ѵ�.
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

	PalSize = (ih.biBitCount > 8 ? 0 : 1 << ih.biBitCount)*sizeof(RGBQUAD); // ���� ����ü �ȷ�Ʈ ũ�⸸ŭ �޸𸮸� �Ҵ��ϰ� �� ���ۿ� ���� ����ü�� �����Ѵ�.
	pih = (BITMAPINFO *)malloc(ih.biSize + PalSize);
	pih->bmiHeader = ih;

	GetDIBits(hdc, hbit, 0, bit.bmHeight, NULL, pih, DIB_RGB_COLORS); // ��Ʈ���� ũ�⸦ ���Ѵ�.
	ih = pih->bmiHeader;

	if (ih.biSizeImage == 0)
		ih.biSizeImage = ((((ih.biWidth*ih.biBitCount) + 31) & ~31) >> 3) * ih.biHeight; // ��Ʈ�� ũ�Ⱑ �������� �ʾ��� ��� ���۾����� ���� ����Ѵ�.

	Size = ih.biSize + PalSize + ih.biSizeImage; 	// ������ �����͸� �б����� �޸� ���Ҵ��Ѵ�.
	pih = (BITMAPINFO *)realloc(pih, Size);

	GetDIBits(hdc, hbit, 0, bit.bmHeight, (PBYTE)pih + ih.biSize + PalSize, pih, DIB_RGB_COLORS); 	// ������ �����͸� �о���δ�.

	fh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PalSize; 	// ���� ����� �����.
	fh.bfReserved1 = 0;
	fh.bfReserved2 = 0;
	fh.bfSize = Size + sizeof(BITMAPFILEHEADER);
	fh.bfType = 0x4d42;

	hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 	// ������ �����ϰ� ���� ����� ���� ����ü, �ȷ�Ʈ, ������ �����͸� ����Ѵ�.
	WriteFile(hFile, &fh, sizeof(fh), &dwWritten, NULL);
	WriteFile(hFile, pih, Size, &dwWritten, NULL);

	free(pih);
	ReleaseDC(NULL, hdc);
	CloseHandle(hFile);
}