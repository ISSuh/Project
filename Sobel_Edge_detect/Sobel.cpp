#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#pragma warning(disable:4996)

#define WIDTH		10368		
#define HEIGHT		7776
#define THREAD_N	8
#define NUM			((int)ceil((HEIGHT - 2) / THREAD_N) + 1)


int *pImgSobelX, *pImgSobelY, *pImgSobel;
int min, max;

float temp1, temp2;
unsigned char *InImg;
unsigned char *OrgImg;

int MaskSobelX[3][3] = { { -1,0,1 },
						 { -2,0,2 },
						 { -1,0,1 } };

int MaskSobelY[3][3] = { { 1,2,1 },
						 { 0,0,0 },
						 { -1,-2,-1 } };

void Sequential();

void Parallel(pthread_t *gradient,pthread_t *SumAbs, pthread_t *Convert);
 
void *Conv(void *arg) {
	int i, j;
	int index = (int)arg * NUM;
	int th_Xval, th_Yval;
	int th_mr,th_mc;

	for (i = index + 1; (i <= index + NUM) && (i<HEIGHT - 1); i++) {
		for (j = 1; j < WIDTH - 1; j++) {
			
			th_Xval = 0;
			th_Yval = 0;

			for (th_mr = 0; th_mr < 3; th_mr++) {
				for (th_mc = 0; th_mc < 3; th_mc++) {
					th_Xval += (MaskSobelX[th_mr][th_mc] * InImg[(i + th_mr - 1) * WIDTH + (j + th_mc - 1)]);
					th_Yval += (MaskSobelY[th_mr][th_mc] * InImg[(i + th_mr - 1) * WIDTH + (j + th_mc - 1)]);
				}
			}

			pImgSobelX[i*WIDTH + j] = th_Xval;
			pImgSobelY[i*WIDTH + j] = th_Yval;
		}
	}

	return NULL;
}

void *Detect_edge(void *arg) {
	int i, j;
	int index = (int)arg* NUM;
	
	for (i = index + 1; (i <= index + NUM) && (i<HEIGHT - 1); i++) {
		for (j = 1; j < WIDTH - 1; j++) {
			
			if (pImgSobelX[i*WIDTH + j] < 0)
				pImgSobelX[i*WIDTH + j] = -pImgSobelX[i*WIDTH + j];
			if (pImgSobelY[i*WIDTH + j] < 0)
				pImgSobelY[i*WIDTH + j] = -pImgSobelY[i*WIDTH + j];

			pImgSobel[i*WIDTH + j] = pImgSobelX[i*WIDTH + j] + pImgSobelY[i*WIDTH + j];

			if (pImgSobel[i*WIDTH + j] < min)
				min = pImgSobel[i*WIDTH + j];
			if (pImgSobel[i*WIDTH + j] > max)
				max = pImgSobel[i*WIDTH + j];
		}
	}

	temp1 = (float)(255.0 / (max - min));
	temp2 = (float)(-255.0*min / (max - min));

	for (i = index + 1; (i <= index + NUM) && (i<HEIGHT - 1); i++) {
		for (j = 1; j < WIDTH - 1; j++) {
			//[min,max]사이의 값을 [0,255]값으로 변환 
			OrgImg[i*WIDTH + j] = (unsigned char)(temp1*pImgSobel[i*WIDTH + j] + temp2);
		}
	}

	return NULL;
}

int main(){
	pthread_t CV[THREAD_N];
	pthread_t DE[THREAD_N];
	pthread_t Convert[THREAD_N];
	clock_t SQ_start, SQ_end,PL_start,PL_end;

 	InImg = (unsigned char*)malloc(sizeof(unsigned char)*(HEIGHT*WIDTH));
 	OrgImg = (unsigned char*)malloc(sizeof(unsigned char)*(HEIGHT*WIDTH));

	FILE *infile = fopen("test2.raw", "rb");
	fread(InImg, sizeof(unsigned char), HEIGHT * WIDTH, infile);
	fclose(infile);

	//정수값을 갖는 이미지 동적 메모리 할당
	pImgSobel = (int*)malloc(sizeof(int)*(HEIGHT*WIDTH));
	pImgSobelX = (int*)malloc(sizeof(int)*(HEIGHT*WIDTH));
	pImgSobelY = (int*)malloc(sizeof(int)*(HEIGHT*WIDTH));
	
	memset(OrgImg, 0, (HEIGHT*WIDTH));
	memset(pImgSobel, 0, (HEIGHT*WIDTH));
	memset(pImgSobelX, 0, (HEIGHT*WIDTH));
	memset(pImgSobelY, 0, (HEIGHT*WIDTH));

	SQ_start = clock();
	Sequential();
	SQ_end = clock();

	PL_start = clock();
	Parallel(CV, DE, Convert);
	PL_end = clock();

	float PL = (float)(PL_end - PL_start) / CLOCKS_PER_SEC;
	float SQ = (float)(SQ_end - SQ_start) / CLOCKS_PER_SEC;

	printf("Finish\n");
	printf("Sequential Runtime	: %f\n",SQ);
	printf("Parallel Runtime 	: %f\n",PL);
	printf("Performance		: %f%\n",(SQ-PL) / PL * 100);

	//동적 할당 메모리 해제
	free(InImg);
	free(OrgImg);
	free(pImgSobel);
	free(pImgSobelX);
	free(pImgSobelY);
}

void Sequential() {
	int mr, mc;
	int newValue, Xval, Yval;

	min = (int)10e10;
	max = (int)-10e10;

	//X,Y 방향 에지 강도 계산  
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			Xval = 0; //0으로 초기화
			Yval = 0;

			for (mr = 0; mr < 3; mr++) {
				for (mc = 0; mc < 3; mc++) {
					Xval += (MaskSobelX[mr][mc] * InImg[(i + mr - 1) * WIDTH + (j + mc - 1)]);
					Yval += (MaskSobelY[mr][mc] * InImg[(i + mr - 1) * WIDTH + (j + mc - 1)]);
				}
			}

			pImgSobelX[i*WIDTH + j] = Xval;
			pImgSobelY[i*WIDTH + j] = Yval;
		}
	}

	//에지 강도 계산 절대값(X)+절대값(Y)후 pImgSobelX[]에 저장 
	//디스플레이를 위해 0에서 255사이로 값의 범위를 매핑
	//이를 위해 먼저 최대,최소값을 찾은후 이를 이용하여 매핑한다.
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			
			temp1 = pImgSobelX[i*WIDTH + j];
			temp2 = pImgSobelY[i*WIDTH + j];

			if (temp1 < 0)
				temp1 = -temp1;
			if (temp2 < 0)
				temp2 = -temp2;

			pImgSobel[i*WIDTH + j] = temp1 + temp2;

			if (pImgSobel[i*WIDTH + j]<min)
				min = pImgSobel[i*WIDTH + j];
			if (pImgSobel[i*WIDTH + j]>max)
				max = pImgSobel[i*WIDTH + j];

		}
	}
		
	//변환시 상수값을 미리 계산 
	temp1 = (float)(255.0 / (max - min));
	temp2 = (float)(-255.0*min / (max - min));

	for (int i = 1; i<HEIGHT - 1; i++) {
		for (int j = 1; j<WIDTH - 1; j++) {
			//[min,max]사이의 값을 [0,255]값으로 변환 
			newValue = pImgSobel[i*WIDTH + j];
			newValue = temp1*newValue + temp2;
			OrgImg[i*WIDTH + j] = (unsigned char)newValue;
		}
	}

	FILE *outfile = fopen("SQ.raw", "wb");
	fwrite(OrgImg, sizeof(unsigned char), HEIGHT * WIDTH, outfile);
	fclose(outfile);
}

void Parallel(pthread_t *CV, pthread_t *DE, pthread_t *Convert){
	int arg = 0,i,j;
	min = (int)10e10;
	max = (int)-10e10;

	//X,Y 방향 에지 강도 계산  
	for (i = 0; i < THREAD_N; i++) 
		pthread_create(&CV[i], NULL, &Conv, (void*)i);

	for (i = 0; i < THREAD_N; i++)
		pthread_join(CV[i], NULL);
	
	//에지 강도 계산 절대값(X)+절대값(Y)후 pImgSobelX[]에 저장
	//디스플레이를 위해 0에서 255사이로 값의 범위를 매핑
	//이를 위해 먼저 최대,최소값을 찾은후 이를 이용하여 매핑한다.
	for (i = 0; i < THREAD_N; i++)
		pthread_create(&DE[i], NULL, &Detect_edge, (void*)i);
	
	for (i = 0; i < THREAD_N; i++)	
		pthread_join(DE[i], NULL);
	
	FILE *outfile = fopen("PL.raw", "wb");
	fwrite(OrgImg, sizeof(unsigned char), HEIGHT * WIDTH, outfile);
	fclose(outfile); 

}