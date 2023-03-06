#include "winplus_definitions.hpp"
#include "winplus_graphics.hpp"

#ifdef __GNUC__
#define WINVER 0x0501
#define _WIN32_IE 0x0501
#endif

#include <new>

#pragma comment ( lib, "msimg32" )
#pragma comment ( lib, "gdiplus" )

namespace winplus
{

// gdi+ init
GdiplusInit::GdiplusInit()
{
    // Initialize GDI+.
    Gdiplus::GdiplusStartup( &_gdiplusToken, &_gdiplusStartupInput, NULL);
}

GdiplusInit::~GdiplusInit()
{
    Gdiplus::GdiplusShutdown(_gdiplusToken);
}

WINPLUS_FUNC_IMPL(void) DrawRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Pen const & pen, Gdiplus::RectF const & rect, Gdiplus::REAL round )
{
    Gdiplus::GraphicsPath path;
    path.StartFigure();

    Gdiplus::REAL width = rect.Width/* - 1.5*/;
    Gdiplus::REAL height = rect.Height/* - 1.0*/;
    Gdiplus::Rect r ( rect.X, rect.Y, width, height );
    int dia = round;
    // diameter can't exceed width or height
    if ( dia > r.Width ) dia = r.Width;
    if ( dia > r.Height ) dia = r.Height;

    // define a corner 
    Gdiplus::Rect Corner( r.X, r.Y, dia, dia );
    // top left
    path.AddArc( Corner, 180, 90 );
    // tweak needed for radius of 10 (dia of 20)
    if ( dia == 20 )
    {
        Corner.Width += 1; 
        Corner.Height += 1; 
        r.Width -=1; r.Height -= 1;
    }
    
    // top right
    Corner.X += (r.Width - dia - 1);
    path.AddArc(Corner, 270, 90);    
    
    // bottom right
    Corner.Y += (r.Height - dia - 1);
    path.AddArc(Corner,   0, 90);    
    
    // bottom left
    Corner.X -= (r.Width - dia - 1);
    path.AddArc(Corner, 90, 90);
    
    path.CloseFigure();                

    //绘制边线
    g.DrawPath( &pen, &path );
}

WINPLUS_FUNC_IMPL(void) FillRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Brush const & brush, Gdiplus::RectF const & rect, Gdiplus::REAL round )
{
    Gdiplus::GraphicsPath path;
    path.StartFigure();

    Gdiplus::REAL width = rect.Width/* - 1.5*/;
    Gdiplus::REAL height = rect.Height /*- 1.0*/;
    Gdiplus::Rect r ( rect.X, rect.Y, width, height );
    int dia = round;
    // diameter can't exceed width or height
    if ( dia > r.Width ) dia = r.Width;
    if ( dia > r.Height ) dia = r.Height;

    // define a corner 
    Gdiplus::Rect Corner( r.X, r.Y, dia, dia );
    // top left
    path.AddArc( Corner, 180, 90 );
    // tweak needed for radius of 10 (dia of 20)
    if ( dia == 20 )
    {
        Corner.Width += 1; 
        Corner.Height += 1; 
        r.Width -=1; r.Height -= 1;
    }
    
    // top right
    Corner.X += (r.Width - dia - 1);
    path.AddArc(Corner, 270, 90);    
    
    // bottom right
    Corner.Y += (r.Height - dia - 1);
    path.AddArc(Corner,   0, 90);    
    
    // bottom left
    Corner.X -= (r.Width - dia - 1);
    path.AddArc(Corner, 90, 90);
    
    path.CloseFigure();                

    //g.FillRectangle( &brush, r );
    g.FillPath( &brush, &path );

}

//////////////////////////////////////////////////////////////////////
MemDC::MemDC( void )
{
    _construct();
}

MemDC::MemDC( HDC hDC )
{
    _construct();
    create(hDC);
}

MemDC::MemDC( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground )
{
    _construct();
    create( hDC, nWidth, nHeight, clrBackground );
}

MemDC::MemDC( MemDC & other )
{
    _construct();
    this->operator = (other);
}

MemDC::~MemDC( void )
{
    destroy();
}

MemDC & MemDC::operator = ( MemDC const & other )
{
    copy(other);
    return *this;
}

BOOL MemDC::create( HDC hDC )
{
    destroy();
    if ( hDC == NULL )
    {
        hDC = GetDC(HWND_DESKTOP);
        _hMemDC = CreateCompatibleDC(hDC);
        ReleaseDC( HWND_DESKTOP, hDC );
    }
    else
    {
        _hMemDC = CreateCompatibleDC(hDC);
    }
    return _hMemDC != NULL;
}

BOOL MemDC::create( HDC hDC, INT nWidth, INT nHeight )
{
    if ( !create(hDC) )
    {
        return FALSE;
    }
    HBITMAP hBitmap;
    if ( hDC == NULL )
    {
        hDC = GetDC(HWND_DESKTOP);
        hBitmap = CreateCompatibleBitmap( hDC, nWidth, nHeight );
        ReleaseDC( HWND_DESKTOP, hDC );
    }
    else
    {
        hBitmap = CreateCompatibleBitmap( hDC, nWidth, nHeight );
    }

    if ( hBitmap == NULL )// 位图创建失败
    {
        DWORD dwError = GetLastError();
        return FALSE;
    }

    attachBitmap(hBitmap);
    return TRUE;
}

BOOL MemDC::create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground )
{
    if ( !create( hDC, nWidth, nHeight ) )
    {
        return FALSE;
    }
    setBackground( clrBackground, TRUE );
    return TRUE;
}

BOOL MemDC::create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground, COLORREF clrTransparent )
{
    if ( !create( hDC, nWidth, nHeight, clrBackground ) )
    {
        return FALSE;
    }
    _transparent = clrTransparent;
    _isTransparent = TRUE;

    return TRUE;
}

BOOL MemDC::copy( MemDC const & other )
{
    if ( this != &other && (BOOL)other )
    {
        create( other, other.width(), other.height() );
        _isTransparent = other._isTransparent;
        _background = other._background;
        _transparent = other._transparent;
        other.copyToDC( _hMemDC, 0, 0 );
        return TRUE;
    }
    return FALSE;
}

void MemDC::destroy( void )
{
    if ( _hBitmap )
    {
        DeleteObject(_hBitmap);
    }
    if ( _hMemDC )
    {
        DeleteDC(_hMemDC);
    }
    ZeroMemory( this, sizeof(MemDC) );
}

void MemDC::enableTransparent( BOOL bIsTransparent, COLORREF clrTransparent /*= 0 */ )
{
    _isTransparent = bIsTransparent;
    _transparent = clrTransparent;
}

void MemDC::setBackground( COLORREF clrBackground, BOOL bFill )
{
    _background = clrBackground;
    if ( bFill )
    {
        HBRUSH hbr = CreateSolidBrush(clrBackground);
        RECT rc = { 0, 0, _width, _height };
        FillRect( _hMemDC, &rc, hbr ); // 粉刷
        DeleteObject(hbr);
    }

}
HBITMAP MemDC::attachBitmap( HBITMAP hBitmap )
{
    HBITMAP hOld = detachBitmap();
    _hBitmap = hBitmap;
    if ( _hMemDC && _hBitmap )
    {
        SelectObject( _hMemDC, _hBitmap );
        BITMAP bm;
        GetObject( _hBitmap, sizeof(BITMAP), &bm );
        _width = bm.bmWidth;
        _height = bm.bmHeight;
    }
    return hOld;
}

HBITMAP MemDC::detachBitmap( void )
{
    HBITMAP h = _hBitmap;
    _hBitmap = NULL;
    return h;
}

BOOL MemDC::passTo( MemDC & other )
{
    if ( &other != this )
    {
        other.destroy();
        other._isTransparent = _isTransparent;
        other._background = _background;
        other._transparent = _transparent;
        other._hBitmap = _hBitmap;
        other._hMemDC = _hMemDC;
        other._height = _height;
        other._width = _width;
        ZeroMemory( this, sizeof(MemDC) );
    }
    return FALSE;
}

#if defined(_GDIPLUS_H)
BOOL MemDC::rotate( DOUBLE angle, MemDC * pMemDC )
{
    using namespace Gdiplus;
    Bitmap bmp( _hBitmap, NULL );
    REAL fImageWidth = bmp.GetWidth(), fImageHeight = bmp.GetHeight();
    Matrix mat;
    mat.Translate( fImageWidth / 2, fImageHeight / 2 );
    mat.Rotate(angle);
    mat.Translate( -fImageWidth / 2, -fImageHeight / 2 );
    // 4个顶点
    PointF pt4v[4] = {
        PointF( 0, 0 ),
        PointF( fImageWidth - 0, 0 ),
        PointF( fImageWidth - 0, fImageHeight - 0 ),
        PointF( 0, fImageHeight - 0 )
    };
    // 算出旋转后4顶点位置
    mat.TransformPoints( pt4v, 4 );
    // 求出矩形
    RectF rcf = RectCircumscribePoints(pt4v);
    // 重新计算旋转矩阵
    mat.Reset();
    mat.Translate( -rcf.X + fImageWidth / 2, -rcf.Y + fImageHeight / 2 );
    mat.Rotate(angle);
    mat.Translate( -( -rcf.X + fImageWidth / 2 ), -( -rcf.Y + fImageHeight / 2 ) );
    // 生成新图
    pMemDC->create( _hMemDC, rcf.Width, rcf.Height, _background );
    pMemDC->enableTransparent( _isTransparent, _transparent );

    Graphics g(*pMemDC);
    g.SetTransform(&mat);

    if ( _isTransparent )
    {
        RectF rcDest( -rcf.X, -rcf.Y, fImageWidth, fImageHeight );
        ImageAttributes imgattr;
        Color clr( GetRValue(_transparent), GetGValue(_transparent), GetBValue(_transparent) );
        imgattr.SetColorKey( clr, clr );
        g.DrawImage( &bmp, rcDest, 0, 0, fImageWidth, fImageHeight, UnitPixel, &imgattr );
    }
    else
    {
        g.DrawImage( &bmp, -rcf.X, -rcf.Y, fImageWidth, fImageHeight );
    }

    g.ResetTransform();
    return TRUE;
}
#endif //defined(_GDIPLUS_H)

BOOL MemDC::stretchBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode /*= HALFTONE */ ) const
{
    if ( _hMemDC == NULL )
    {
        return FALSE;
    }
    nMode = SetStretchBltMode( hDestDC, nMode );
    BOOL b = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, width, height, SRCCOPY );
    SetStretchBltMode( hDestDC, nMode );
    return b;
}

BOOL MemDC::stretchToDC( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode ) const
{
    return stretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, _width, _height, nMode );
}

BOOL MemDC::bitBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y ) const
{
    if ( _hMemDC == NULL )
    {
        return FALSE;
    }
    return ::BitBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, SRCCOPY );
}

BOOL MemDC::copyToDC( HDC hDestDC, INT xDest, INT yDest ) const
{
    return bitBlt( hDestDC, xDest, yDest, _width, _height, 0, 0 );
}

BOOL MemDC::transparentBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode /*= HALFTONE */ ) const
{
    if ( _hMemDC == NULL )
    {
        return FALSE;
    }
    nMode = SetStretchBltMode( hDestDC, nMode );
    BOOL b;
    if ( isTransparent() )
    {
        b = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, width, height, (UINT)_transparent );
    }
    else
    {
        b = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, width, height, SRCCOPY );
    }
    SetStretchBltMode( hDestDC, nMode );
    return b;
}

BOOL MemDC::transparentToDC( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode /*= HALFTONE */ ) const
{
    return transparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, _width, _height, nMode );
}

void MemDC::_construct()
{
    _hMemDC = NULL;
    _hBitmap = NULL;
    _width = 0;
    _height = 0;
    _background = 0;
    _transparent = 0;
    _isTransparent = FALSE;
}

///////////////////////////////////////////////////////////////////////////
#if defined(_GDIPLUS_H)

MemImage::MemImage( void )
{
    _construct();
}

MemImage::MemImage( int nWidth, int nHeight )
{
    _construct();
    create( nWidth, nHeight );
}

MemImage::MemImage( MemImage & other )
{
    _construct();
    this->operator = (other);
}

MemImage::~MemImage( void )
{
    destroy();
}

MemImage & MemImage::operator = ( MemImage const & other )
{
    clone(other);
    return *this;
}

BOOL MemImage::create( int nWidth, int nHeight )
{
    destroy();
    _pBmpImage = new Gdiplus::Bitmap( nWidth, nHeight, PixelFormat32bppARGB );
    return _pBmpImage != NULL;
}

BOOL MemImage::clone( MemImage const & other )
{
    if ( this != &other )
    {
        destroy();
        _pBmpImage = (Gdiplus::Bitmap *)other._pBmpImage->Clone( 0, 0, other.width(), other.height(), other._pBmpImage->GetPixelFormat() );
        return TRUE;
    }
    return FALSE;
}

BOOL MemImage::copy( Gdiplus::Image * pImage )
{
    if ( _pBmpImage != pImage )
    {
        Gdiplus::Graphics g(_pBmpImage);
        g.DrawImage( pImage, 0, 0, pImage->GetWidth(), pImage->GetHeight() );
        return TRUE;
    }
    return FALSE;
}
void MemImage::destroy( void )
{
    if ( _pBmpImage )
    {
        delete _pBmpImage;
        _pBmpImage = NULL;
    }
}

BOOL MemImage::passTo( MemImage & other )
{
    if ( &other != this )
    {
        other.destroy();
        other._pBmpImage = _pBmpImage;

        _pBmpImage = NULL;
        return TRUE;
    }
    return FALSE;
}

BOOL MemImage::rotate( double angle, MemImage * pMemImage )
{
    Gdiplus::Matrix mat;
    Gdiplus::REAL fImageWidth = width(), fImageHeight = height();
    mat.RotateAt( angle, Gdiplus::PointF( fImageWidth / 2, fImageHeight / 2 ) );
    // 4个顶点
    Gdiplus::PointF pt4v[4] = {
        Gdiplus::PointF( 0, 0 ),
        Gdiplus::PointF( fImageWidth - 0, 0 ),
        Gdiplus::PointF( fImageWidth - 0, fImageHeight - 0 ),
        Gdiplus::PointF( 0, fImageHeight - 0 )
    };
    Gdiplus::PointF pt4v2[4] = {
        Gdiplus::PointF( 0, 0 ),
        Gdiplus::PointF( fImageWidth - 0, 0 ),
        Gdiplus::PointF( fImageWidth - 0, fImageHeight - 0 ),
        Gdiplus::PointF( 0, fImageHeight - 0 )
    };
    /*
    printf( "(%f,%f) (%f,%f) (%f,%f) (%f,%f)\n",
            pt4v[0].X, pt4v[0].Y,
            pt4v[1].X, pt4v[1].Y,
            pt4v[2].X, pt4v[2].Y,
            pt4v[3].X, pt4v[3].Y
        );*/
    
    // 算出旋转后4顶点位置
    mat.TransformPoints( pt4v, 4 );
    /*
    printf( "(%f,%f) (%f,%f) (%f,%f) (%f,%f)\n",
            pt4v[0].X, pt4v[0].Y,
            pt4v[1].X, pt4v[1].Y,
            pt4v[2].X, pt4v[2].Y,
            pt4v[3].X, pt4v[3].Y
        );*/
    
    mat.TransformVectors( pt4v2, 4 );
    /*
    printf( "(%f,%f) (%f,%f) (%f,%f) (%f,%f)\n",
            pt4v2[0].X, pt4v2[0].Y,
            pt4v2[1].X, pt4v2[1].Y,
            pt4v2[2].X, pt4v2[2].Y,
            pt4v2[3].X, pt4v2[3].Y
        );*/
    

    // 求出矩形
    Gdiplus::RectF rc = RectCircumscribePoints(pt4v);
    // 重新计算旋转矩阵
    mat.Reset();
    mat.RotateAt( angle, Gdiplus::PointF( -rc.X + fImageWidth / 2, -rc.Y + fImageHeight / 2 ) );

    pMemImage->create( rc.Width, rc.Height );
    Gdiplus::Graphics g(pMemImage->_pBmpImage);
    g.SetTransform(&mat);
    g.DrawImage( _pBmpImage, -rc.X, -rc.Y, fImageWidth, fImageHeight );

    return TRUE;
}

BOOL MemImage::stretch( Gdiplus::Graphics & gDest, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
{
    return Gdiplus::Ok == gDest.DrawImage( _pBmpImage, Gdiplus::Rect( xDest, yDest, nDestWidth, nDestHeight ), xSrc, ySrc, nSrcWidth, nSrcHeight, Gdiplus::UnitPixel );
}

BOOL MemImage::output( Gdiplus::Graphics & gDest, int xDest, int yDest, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
{
    return Gdiplus::Ok == gDest.DrawImage( _pBmpImage, Gdiplus::Rect(xDest, yDest, nSrcWidth, nSrcHeight), xSrc, ySrc, nSrcWidth, nSrcHeight, Gdiplus::UnitPixel );
}

void MemImage::_construct()
{
    _pBmpImage = NULL;
}

#endif

} // namespace winplus
