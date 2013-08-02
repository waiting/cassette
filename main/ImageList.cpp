
#include "Cassette.h"
#include "CassetteApp.h"
#include "ImageList.h"

using namespace Gdiplus;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ImageList::ImageList()
{

}

ImageList::~ImageList()
{

}

int ImageList::AddAlphaImage( Gdiplus::Bitmap & bm )
{
	// ×ª³É32Î»bpp
	HBITMAP hBitmap;
	void * pvBits;
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = bm.GetWidth();
	bmi.bmiHeader.biHeight = bm.GetHeight();
	bm.RotateFlip( RotateNoneFlipY );
	hBitmap = CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0 );
	BitmapData bitmapData;
	bm.LockBits( &Rect( 0, 0, bm.GetWidth(), bm.GetHeight() ), ImageLockModeRead, PixelFormat32bppARGB, &bitmapData );
	RtlMoveMemory( pvBits, bitmapData.Scan0, bm.GetHeight() * bitmapData.Stride );
	bm.UnlockBits(&bitmapData);
	int i;
	i = ImageList_Add( GetSafeHandle(), hBitmap, NULL );
	DeleteObject(hBitmap);

	return i;
}