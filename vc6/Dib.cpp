#include "stdafx.h"
#include "Dib.h"
#include <stdio.h>
#include <png.h>
#include <exception>

void DIBInit(struct dib_struct *dib, struct dib_struct *dib2)
{
	dib->hBitmap = NULL;
	dib->Bits = NULL;
	if (dib2)
	{
		if (DIBCreate(dib, DIBWidth(dib2), DIBHeight(dib2), DIBDepth(dib2)))
		{
			DIBCopy(dib, dib2);
		}
	}
}

void DIBRelease(struct dib_struct *dib)
{
	DIBDestroy(dib);
}

BOOL DIBCreate(struct dib_struct *dib, int width, int height, int depth)
{
	DIBDestroy(dib);
	dib->bytes_per_line = DIBScanBytes(width, depth);
	dib->bytes_per_pixel = DIBPixelBytes(depth);
	dib->Header.Info.biSize = sizeof(BITMAPINFOHEADER);
	dib->Header.Info.biWidth = width;
	dib->Header.Info.biHeight = height;
	dib->Header.Info.biBitCount = depth;
	dib->Header.Info.biPlanes = 1;
	dib->Header.Info.biXPelsPerMeter = 0;
	dib->Header.Info.biYPelsPerMeter = 0;
	dib->Header.Info.biClrUsed = 0;
	dib->Header.Info.biClrImportant	= 0;
	dib->Header.Info.biCompression = depth == 24 ? BI_RGB : BI_BITFIELDS;
	dib->Header.Info.biSizeImage = dib->bytes_per_line * height;
	switch (depth) 
	{
	case 16:
		dib->Header.BitField[0] = 0x7c00;
		dib->Header.BitField[1] = 0x03e0;
		dib->Header.BitField[2] = 0x001f;
		break;

	case 32:
		dib->Header.BitField[0] = 0xff0000;
		dib->Header.BitField[1] = 0x00ff00;
		dib->Header.BitField[2] = 0x0000ff;
		break;

	default:
		dib->Header.BitField[0] = 0;
		dib->Header.BitField[1] = 0;
		dib->Header.BitField[2] = 0;
		break;
	}
	HDC dc = ::GetDC(0);
	dib->hBitmap = CreateDIBSection(dc, (BITMAPINFO *)&dib->Header, DIB_RGB_COLORS, &dib->Bits, NULL, 0);
	::ReleaseDC(0, dc);
	return dib->hBitmap != 0;
}

void DIBDestroy(struct dib_struct *dib)
{
	if (dib->hBitmap) 
	{
		::GdiFlush();
		::DeleteObject(dib->hBitmap);
		dib->hBitmap = 0;
	}
}

BOOL DIBLoadBmp(struct dib_struct *dib, const char *path)
{
	DIBDestroy(dib);
	dib->hBitmap = (HBITMAP)::LoadImage(::GetModuleHandle(0), path, IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (!dib->hBitmap)
	{
		return FALSE;
	}
	DIBSECTION dib_;
	if (::GetObject(dib->hBitmap, sizeof(DIBSECTION), &dib_) != sizeof(DIBSECTION)) 
	{
		::DeleteObject(dib->hBitmap);
		dib->hBitmap = 0;
		return FALSE;
	}
	dib->Header.Info = dib_.dsBmih;
	for (int i = 0; i < 3; i++)
	{
		dib->Header.BitField[i] = dib_.dsBitfields[i];
	}
	dib->bytes_per_pixel = DIBPixelBytes(dib_.dsBmih.biBitCount);
	dib->bytes_per_line = DIBScanBytes(dib_.dsBmih.biWidth, dib_.dsBmih.biBitCount);
	dib->Bits = dib_.dsBm.bmBits;
	return TRUE;
}

BOOL DIBSaveBmp(struct dib_struct *dib, const char *path)
{
	FILE *fp = fopen(path, "wb+");
	if (fp)
	{
		int length = dib->bytes_per_line * DIBHeight(dib);
		BITMAPFILEHEADER header;
		memset(&header, 0, sizeof(header));
		header.bfType = (WORD)('M' << 8) | 'B';
		header.bfSize = sizeof(header) + dib->Header.Info.biSize + length;
		header.bfOffBits = sizeof(header) + dib->Header.Info.biSize;
		fwrite(&header, 1, sizeof(header), fp);
		fwrite(&dib->Header.Info, 1, dib->Header.Info.biSize, fp);
		if (dib->Header.Info.biCompression == BI_BITFIELDS)
		{
			fwrite(dib->Header.BitField, 1, sizeof(dib->Header.BitField), fp);
		}
		fwrite(dib->Bits, 1, length, fp);
		fclose(fp);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DIBLoadPng(struct dib_struct *dib, const char *path, bool alpha_include)
{
	png_bytepp row = NULL;
	FILE *fp = fopen(path, "rb");
	if (!fp)
	{
		return FALSE;
	}
	try 
	{
		png_byte buf[8];
		if (fread(buf, 1, 8, fp) != 8 || !png_check_sig(buf, 8)) 
		{
			fclose(fp);
			return FALSE;
		}
		png_structp png_ptr;
		png_infop info_ptr;
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
		if (png_ptr == 0)
		{
			throw exception("not enough memory");
		}
		else
		{
			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == 0)
			{
				if (png_ptr)
				{
					png_destroy_write_struct(&png_ptr, info_ptr? &info_ptr: 0);
				}
				png_ptr = 0;
				info_ptr = 0;
				throw exception("not enough memory");
			}
		}
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		png_uint_32	width, height;
		int depth, color_type;
		png_get_IHDR(png_ptr, info_ptr,
			&width, &height, &depth, &color_type, 
			0, 0, 0);
		if (alpha_include) 
		{	
			if ((color_type & PNG_COLOR_MASK_ALPHA) == 0) 
			{
				fclose(fp);
				return false;
			}
		}
		else 
		{					
			if (color_type & PNG_COLOR_MASK_ALPHA)		
			{
				png_set_strip_alpha(png_ptr);
			}
		}
		if (color_type == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_expand(png_ptr);
		}
		if (color_type == PNG_COLOR_TYPE_GRAY && depth < 8)
		{
			png_set_expand(png_ptr);
		}
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		{
			png_set_expand(png_ptr);
		}
		if (depth > 8)
		{
			png_set_strip_16(png_ptr);
		}
		if (color_type == PNG_COLOR_TYPE_GRAY)
		{
			png_set_gray_to_rgb(png_ptr);
		}
		png_read_update_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr,
			&width, &height, &depth, &color_type, 
			0, 0, 0);
		if (color_type != PNG_COLOR_TYPE_RGB) 
		{
			fclose(fp);
			return FALSE;
		}
		if (!DIBCreate(dib, width, height, alpha_include? 32: 24))
		{
			fclose(fp);
			return FALSE;
		}
		row = new png_byte *[DIBHeight(dib)];
		for (png_uint_32 y = 0; y < DIBHeight(dib); y++)
		{
			row[y] = (png_byte *)DIBGetBits(dib, 0, y);
		}
		png_set_bgr(png_ptr);
		png_read_image(png_ptr, row);
		png_read_end(png_ptr, info_ptr);	
	}
	catch (...) 
	{
		if (row)
		{
			delete[] row;
			row = NULL;
		}
		fclose(fp);
		return FALSE;
	}
	if (row)
	{
		delete[] row;
		row = NULL;
	}
	fclose(fp);
	return TRUE;
}

BOOL DIBSavePng(struct dib_struct *dib, const char *path)
{
	png_bytepp row = NULL;
	FILE *fp = fopen(path, "wb");
	if (!fp)
	{
		return FALSE;
	}
	try 
	{
		png_structp png_ptr;
		png_infop info_ptr;	
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
		if (png_ptr == 0)
		{
			throw exception("not enough memory");
		}
		else
		{
			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == 0)
			{
				if (png_ptr)
				{
					png_destroy_write_struct(&png_ptr, info_ptr? &info_ptr: 0);
				}
				png_ptr = 0;
				info_ptr = 0;
				throw exception("not enough memory");
			}
		}
		png_init_io(png_ptr, fp);
		if (false)
		{
			png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
			png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);
		}
		png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_SUB);
		png_set_IHDR(png_ptr, info_ptr,
			DIBWidth(dib), DIBHeight(dib), 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		png_write_info(png_ptr, info_ptr);
		row = new png_byte*[DIBHeight(dib)];
		for (png_uint_32 y = 0; y < DIBHeight(dib); y++)
		{
			row[y] = (png_byte *)DIBGetBits(dib, 0, y);
		}
		png_set_bgr(png_ptr);
		png_write_image(png_ptr, row);
		png_write_end(png_ptr, info_ptr);
	}
	catch (...)
	{
		if (row)
		{
			delete[] row;
			row = NULL;
		}
		fclose(fp);
		return FALSE;
	}
	if (row)
	{
		delete[] row;
		row = NULL;
	}
	fclose(fp);
	return TRUE;
}

void DIBMix(struct dib_struct *dib, struct dib_struct *dib2, POINT to, SIZE size, POINT from, COLORREF tc)
{
	if (DIBDepth(dib) == DIBDepth(dib2)) 
	{
		switch (DIBDepth(dib)) 
		{
		case 16:
			{
				BYTE r = GetRValue(tc);
				BYTE g = GetGValue(tc);
				BYTE b = GetBValue(tc);
				WORD color = (r << 7) & 0x7c00 | (g << 2) & 0x03e0 | (b >> 3) & 0x001f;
				for (int y = 0; y < size.cy; y++) 
				{
					WORD *p = (WORD *)DIBGetBits(dib, to.x, to.y + y);
					const WORD *q = (const WORD *)DIBGetBits(dib2, from.x, from.y + y);
					for (int x = 0; x < size.cx; x++) 
					{
						if (*q != color)
						{
							*p = *q;
						}
						p++;
						q++;
					}
				}
			}
			break;

		case 24:
			{
				BYTE r = GetRValue(tc);
				BYTE g = GetGValue(tc);
				BYTE b = GetBValue(tc);
				for (int y = 0; y < size.cy; y++) 
				{
					BYTE *p = (BYTE *)DIBGetBits(dib, to.x, to.y + y);
					const BYTE *q = (const BYTE *)DIBGetBits(dib2, from.x, from.y + y);
					for (int x = 0; x < size.cx; x++) 
					{
						if (q[0] != b || q[1] != g || q[2] != r) 
						{
							p[0] = q[0];
							p[1] = q[1];
							p[2] = q[2];
						}
						p += 3;
						q += 3;
					}
				}
			}
			break;

		case 32:
			{
				for (int y = 0; y < size.cy; y++) 
				{
					DWORD *p = (DWORD *)DIBGetBits(dib, to.x, to.y + y);
					const DWORD *q = (const DWORD *)DIBGetBits(dib2, from.x, from.y + y);
					for (int x = 0; x < size.cx; x++) 
					{
						if ((*q & 0xffffff) != tc)
							*p = *q;
						p++;
						q++;
					}
				}
			}
			break;
		}
	}
	else 
	{
		//FIXME:
	}
}

void DIBFillRect(struct dib_struct *dib, const RECT &rect, COLORREF color)
{
	HDC	_hScreenDC = ::GetDC(NULL); 
	HDC	_memDC = ::CreateCompatibleDC(_hScreenDC);
	HGDIOBJ _hOldBitmap = ::SelectObject(_memDC, DIBHandle(dib));
	::SetBkColor(_memDC, color);
	::ExtTextOut(_memDC, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
	::GdiFlush();
	::SelectObject(_memDC, _hOldBitmap);
	::DeleteDC(_memDC);
	::DeleteDC(_hScreenDC);
}

void DIBClone(struct dib_struct *dib, struct dib_struct *dib2)
{
	if (dib != dib2) 
	{
		if (DIBCreate(dib, DIBWidth(dib2), DIBHeight(dib2), DIBDepth(dib2)))
		{
			DIBCopy(dib, dib2);
		}
	}
}

void *DIBGetBits(struct dib_struct *dib) 
{ 
	return dib->Bits; 
}

void *DIBGetBits(struct dib_struct *dib, int x, int y)
{
	return (void *)((char *)DIBGetBits(dib)
		+ (DIBHeight(dib) - y - 1) * dib->bytes_per_line + x * dib->bytes_per_pixel);
}

BOOL DIBIsOK(struct dib_struct *dib) 
{ 
	return dib->Bits != 0; 
}

int DIBWidth(struct dib_struct *dib)
{ 
	return dib->Header.Info.biWidth; 
}

int DIBHeight(struct dib_struct *dib)
{ 
	return dib->Header.Info.biHeight; 
}

int DIBDepth(struct dib_struct *dib)
{ 
	return dib->Header.Info.biBitCount; 
}

int DIBBytesPerLine(struct dib_struct *dib) 
{ 
	return dib->bytes_per_line; 
}

int DIBBytesPerPixel(struct dib_struct *dib)
{ 
	return dib->bytes_per_pixel; 
}

HBITMAP DIBHandle(struct dib_struct *dib) 
{
	return dib->hBitmap;
}

int DIBPixelBytes(int depth)
{
	return (depth + 7) / 8;
}

int DIBScanBytes(int width, int depth)
{
	return ((width * DIBPixelBytes(depth) + 3) / 4) * 4;
}

void DIBDraw(struct dib_struct *dib, HDC dc, int x, int y, int w, int h, int ox, int oy)
{
	HDC	_memDC = ::CreateCompatibleDC(dc);
	HGDIOBJ _hOldBitmap = ::SelectObject(_memDC, DIBHandle(dib));
	BitBlt(dc, x, y, w, h, _memDC, ox, oy, SRCCOPY);
	::GdiFlush();
	::SelectObject(_memDC, _hOldBitmap);
	::DeleteDC(_memDC);
}

void DIBDraw(struct dib_struct *dib, HDC dc, const RECT &rect, POINT point)
{
	DIBDraw(dib, dc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, point.x, point.y);
}

void DIBDraw(struct dib_struct *dib, HDC dc, const RECT &rect)
{
	DIBDraw(dib, dc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, rect.left, rect.top);
}

void DIBCopy(struct dib_struct *dib, struct dib_struct *dib2)
{
	HDC	_hScreenDC = ::GetDC(NULL); 
	HDC	_memDC_1 = ::CreateCompatibleDC(_hScreenDC);
	HGDIOBJ _hOldBitmap_1 = ::SelectObject(_memDC_1, DIBHandle(dib));
	HDC	_memDC_2 = ::CreateCompatibleDC(_hScreenDC);
	HGDIOBJ _hOldBitmap_2 = ::SelectObject(_memDC_2, DIBHandle(dib2));
	::BitBlt(_memDC_1, 0, 0, DIBWidth(dib2), DIBHeight(dib2), _memDC_2, 0, 0, SRCCOPY);
	::GdiFlush();
	::SelectObject(_memDC_1, _hOldBitmap_1);
	::SelectObject(_memDC_2, _hOldBitmap_2);
	::DeleteDC(_memDC_1);
	::DeleteDC(_memDC_2);
	::DeleteDC(_hScreenDC);
}

void DIBCopy(struct dib_struct *dib, struct dib_struct *dib2, POINT to, SIZE size, POINT from)
{
	HDC	_hScreenDC = ::GetDC(NULL); 
	HDC	_memDC_1 = ::CreateCompatibleDC(_hScreenDC);
	HGDIOBJ _hOldBitmap_1 = ::SelectObject(_memDC_1, DIBHandle(dib));
	HDC	_memDC_2 = ::CreateCompatibleDC(_hScreenDC);
	HGDIOBJ _hOldBitmap_2 = ::SelectObject(_memDC_2, DIBHandle(dib2));
	::BitBlt(_memDC_1, to.x, to.y, size.cx, size.cy, _memDC_2, from.x, from.y, SRCCOPY);
	::GdiFlush();
	::SelectObject(_memDC_1, _hOldBitmap_1);
	::SelectObject(_memDC_2, _hOldBitmap_2);
	::DeleteDC(_memDC_1);
	::DeleteDC(_memDC_2);
	::DeleteDC(_hScreenDC);
}
