// Screenshot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#define FALSE 0
#define TRUE  1



void save_bitmap(char *szFilename, HBITMAP hBitmap)
{
	HDC hdc = NULL;
	FILE* fp = NULL;
	LPVOID pBuf = NULL;
	BITMAPINFO bmpInfo = {0};
	BITMAPFILEHEADER bmpFileHeader;

	do
	{
		hdc = GetDC(NULL);
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

		// Set the image's size in the bitmap header
		if(bmpInfo.bmiHeader.biSizeImage <= 0)
		{
			bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth *
				                            abs(bmpInfo.bmiHeader.biHeight) *
											(bmpInfo.bmiHeader.biBitCount + 7)/8;
		}

		// Allocate space for the bitmap
		if(NULL == (pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)))
		{
			break;
		}

		// Set the compression level to 24-bit
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);	

		// Open the filename passed in
		if(NULL == (fp = fopen(szFilename, "wb")))
		{
			break;
		}

		// Set the remaining header values
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType = 'MB';
		bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// Write the bitmap header and buffer to file
		fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp);
	}while(FALSE);

	// Free objects
	if(hdc)
	{
		ReleaseDC(NULL,hdc);
	}

	if(pBuf)
	{
		free(pBuf);
	}

	if(fp)
	{
		fclose(fp);
	}
}

void capture_screen(int index)
{
	FILE* fp = NULL;
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	char filename[100];
	

	// Create the bitmap object
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC,
													nScreenWidth,
													nScreenHeight);
	
	SelectObject(hCaptureDC,hCaptureBitmap);

	BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,
		   hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 

	// Save the bitmap to file
	sprintf(filename, "FINDME_%d.bmp", index);
	save_bitmap(filename, hCaptureBitmap);
	
	// Clean up objects
	ReleaseDC(hDesktopWnd,hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPredInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int n = 1;
	while(1)
	{
		Sleep(10000);
		capture_screen(n++);
		
	}
	return 0;
}