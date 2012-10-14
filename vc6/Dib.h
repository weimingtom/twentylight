// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__4D3F16D0_F140_4540_A22F_9CC6B817FAAD__INCLUDED_)
#define AFX_DIB_H__4D3F16D0_F140_4540_A22F_9CC6B817FAAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct dib_struct {
	struct Header_struct {
		BITMAPINFOHEADER Info;
		DWORD BitField[3];
	} Header;
	HBITMAP	hBitmap;
	void *Bits;
	int bytes_per_line;
	int bytes_per_pixel;
};

extern void DIBInit(struct dib_struct *dib, struct dib_struct *dib2);
extern void DIBRelease(struct dib_struct *dib);
extern BOOL DIBCreate(struct dib_struct *dib, int width, int height, int depth);
extern void DIBDestroy(struct dib_struct *dib);
extern BOOL DIBLoadBmp(struct dib_struct *dib, const char *path);
extern BOOL DIBSaveBmp(struct dib_struct *dib, const char *path);
extern BOOL DIBLoadPng(struct dib_struct *dib, const char *path, bool alpha_include=false);
extern BOOL DIBSavePng(struct dib_struct *dib, const char *path);
extern void DIBCopy(struct dib_struct *dib, struct dib_struct *dib2);
extern void DIBCopy(struct dib_struct *dib, struct dib_struct *dib2, POINT to, SIZE size, POINT from);
extern void DIBMix(struct dib_struct *dib, struct dib_struct *dib2, POINT to, SIZE size, POINT from, COLORREF transparent = RGB(0, 255, 0));
extern void DIBFillRect(struct dib_struct *dib, const RECT &rect, COLORREF color);
extern void DIBDraw(struct dib_struct *dib, HDC dc, int x, int y, int w, int h, int ox = 0, int oy = 0);
extern void DIBDraw(struct dib_struct *dib, HDC dc, const RECT &rect, POINT point);
extern void DIBDraw(struct dib_struct *dib, HDC dc, const RECT &rect);
extern void *DIBGetBits(struct dib_struct *dib);
extern void *DIBGetBits(struct dib_struct *dib, int x, int y);
extern BOOL DIBIsOK(struct dib_struct *dib);
extern int DIBWidth(struct dib_struct *dib);
extern int DIBHeight(struct dib_struct *dib);
extern int DIBDepth(struct dib_struct *dib);
extern int DIBBytesPerLine(struct dib_struct *dib);
extern int DIBBytesPerPixel(struct dib_struct *dib);
extern HBITMAP DIBHandle(struct dib_struct *dib);
extern void DIBClone(struct dib_struct *dib, struct dib_struct *dib2);
extern int DIBScanBytes(int width, int depth);
extern int DIBPixelBytes(int depth);

#endif // !defined(AFX_DIB_H__4D3F16D0_F140_4540_A22F_9CC6B817FAAD__INCLUDED_)
