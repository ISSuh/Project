BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBackground(HDC hdc, POINT bP[], RECT &clientR);
void init_point(POINT bP[], POINT temp[], RECT &clientR, int size);
void drawPointLine(HDC hdc, POINT temp[], int size);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF color, POINT temp[], int size);
void drawRact(HDC hdc, RECT &drawPoint, COLORREF penC, COLORREF brushC, POINT temp[], int size);

POINT bP[3];
int lineX, lineY;

void init_point(POINT bP[], POINT temp[], RECT &clientR, int size){
	double tmpX, tmpY;

	bP[0].x = clientR.left + 50;
	bP[0].y = clientR.bottom - 26;
	bP[1].x = clientR.right - 59;
	bP[1].y = bP[0].y;
	bP[2].x = bP[0].x;
	bP[2].y = clientR.top + 20;

	lineX = bP[1].x - bP[0].x;
	lineY = bP[2].y - bP[0].y;
	tmpX = lineX / size;
	tmpY = lineY / size;

	for (int i = 0; i <= size; i++){
		temp[i].x = (i*tmpX) + bP[0].x;
		temp[i].y = (i*tmpY) + bP[0].y;
	}
}
void drawPointLine(HDC hdc, POINT temp[], int size){
	for (int i = 0; i < size-1; i++){
	MoveToEx(hdc, temp[i].x, temp[i].y, NULL);
	LineTo(hdc, temp[i+1].x, temp[i+1].y);
	}
	/*MoveToEx(hdc, temp[0].x, temp[1].y, NULL);
	LineTo(hdc, temp[2].x, temp[10].y);
	MoveToEx(hdc, temp[2].x, temp[10].y, NULL);
	LineTo(hdc, temp[3].x, temp[5].y);
	MoveToEx(hdc, temp[3].x, temp[5].y, NULL);
	LineTo(hdc, temp[4].x, temp[6].y);
	MoveToEx(hdc, temp[4].x, temp[6].y, NULL);
	LineTo(hdc, temp[7].x, temp[3].y);*/
}
void DrawBackground(HDC hdc, POINT bP[], RECT &clientR){
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[1].x, bP[1].y);
	MoveToEx(hdc, bP[0].x, bP[0].y, NULL);
	LineTo(hdc, bP[2].x, bP[2].y);
}
void drawRact(HDC hdc, RECT &Point, COLORREF color, POINT temp[], int size){
	drawRact(hdc, Point, color, color, temp, size);
}
void drawRact(HDC hdc, RECT &Point, COLORREF penC, COLORREF brushC, POINT temp[], int size){
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	hPen = CreatePen(PS_SOLID, 1, penC);
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(brushC);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	/*Ellipse(hdc, (temp[0].x - 5), (temp[1].y - 5), (temp[0].x) + 5, (temp[1].y) + 5);
	Ellipse(hdc, (temp[2].x - 5), (temp[10].y - 5), (temp[2].x) + 5, (temp[10].y) + 5);
	Ellipse(hdc, (temp[3].x - 5), (temp[5].y - 5), (temp[3].x) + 5, (temp[5].y) + 5);
	Ellipse(hdc, (temp[4].x - 5), (temp[6].y - 5), (temp[4].x) + 5, (temp[6].y) + 5);
	Ellipse(hdc, (temp[7].x - 5), (temp[3].y - 5), (temp[7].x) + 5, (temp[3].y) + 5);*/

	DeleteObject(SelectObject(hdc, hOldPen));
	DeleteObject(SelectObject(hdc, hOldBrush));
}