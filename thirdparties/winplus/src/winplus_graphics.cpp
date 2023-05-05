
#include "winplus_definitions.hpp"
#include "smartptr.hpp"
#include "winplus_graphics.hpp"
#include "strings.hpp"

#ifdef __GNUC__
#define WINVER 0x0501
#define _WIN32_IE 0x0501
#endif

#include <new>

#pragma comment ( lib, "msimg32" )
#pragma comment ( lib, "gdiplus" )

namespace winplus
{

// GDI+ Init ------------------------------------------------------------------------------
GdiplusInit::GdiplusInit() : _gdiplusToken(0), _canShutdown(true)
{
    // Initialize GDI+.
    Gdiplus::GdiplusStartup( &_gdiplusToken, &_gdiplusStartupInput, &_gdiplusStartupOutput );
}

GdiplusInit::~GdiplusInit()
{
    if ( _canShutdown )
    {
        Gdiplus::GdiplusShutdown(_gdiplusToken);
    }
}

// class Graphics -------------------------------------------------------------------------
void Graphics::DrawRoundRectangle( Gdiplus::Pen const * pen, Gdiplus::RectF const & rect, Gdiplus::REAL round )
{
    Gdiplus::GraphicsPath path;
    path.StartFigure();

    Gdiplus::REAL width = rect.Width/* - 1.5*/;
    Gdiplus::REAL height = rect.Height/* - 1.0*/;
    Gdiplus::RectF r ( rect.X, rect.Y, width, height );
    Gdiplus::REAL dia = round;
    // diameter can't exceed width or height
    if ( dia > r.Width ) dia = r.Width;
    if ( dia > r.Height ) dia = r.Height;

    // define a corner 
    Gdiplus::RectF Corner( r.X, r.Y, dia, dia );
    // top left
    path.AddArc( Corner, 180, 90 );
    // tweak needed for radius of 10 (dia of 20)
    if ( dia == 20 )
    {
        Corner.Width += 1; 
        Corner.Height += 1; 
        r.Width -=1;
        r.Height -= 1;
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

    // 绘制边线
    if ( pen ) this->DrawPath( pen, &path );
}

void Graphics::FillRoundRectangle( Gdiplus::Brush const * brush, Gdiplus::RectF const & rect, Gdiplus::REAL round )
{
    Gdiplus::GraphicsPath path;
    path.StartFigure();

    Gdiplus::REAL width = rect.Width;
    Gdiplus::REAL height = rect.Height;
    Gdiplus::RectF r ( rect.X, rect.Y, width, height );
    Gdiplus::REAL dia = round;
    // diameter can't exceed width or height
    if ( dia > r.Width ) dia = r.Width;
    if ( dia > r.Height ) dia = r.Height;

    // define a corner 
    Gdiplus::RectF Corner( r.X, r.Y, dia, dia );
    // top left
    path.AddArc( Corner, 180, 90 );
    // tweak needed for radius of 10 (dia of 20)
    if ( dia == 20 )
    {
        Corner.Width += 1; 
        Corner.Height += 1; 
        r.Width -=1;
        r.Height -= 1;
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

    if ( brush ) this->FillPath( brush, &path );
}

void Graphics::FillRectangle( Gdiplus::Brush const * brush, Gdiplus::RectF const & rect )
{
    Gdiplus::PointF points[5] = {
        { rect.GetLeft(), rect.GetTop() },
        { rect.GetLeft(), rect.GetTop() },
        { rect.GetRight(), rect.GetTop() },
        { rect.GetRight(), rect.GetBottom() },
        { rect.GetLeft(), rect.GetBottom() },
    };
    this->FillPolygon( brush, points, 5 );
}

void Graphics::DrawShadowString( winplus::String const & str, Gdiplus::Font const & font, Gdiplus::Brush const * brushLight, Gdiplus::Brush const * brushDark, Gdiplus::RectF const & layoutRect, Gdiplus::StringFormat const & fmt, Gdiplus::RectF * boundingRect )
{
    winplus::UnicodeString sU = winplus::StringToUnicode(str);
    if ( boundingRect )
    {
        this->MeasureString(
            sU.c_str(),
            (INT)sU.length(),
            &font,
            layoutRect,
            &fmt,
            boundingRect
        );
    }

    if ( brushDark )
    {
        this->DrawString(
            sU.c_str(),
            (INT)sU.length(),
            &font,
            Gdiplus::RectF( layoutRect.X + 1, layoutRect.Y + 1, layoutRect.Width, layoutRect.Height ),
            &fmt,
            brushDark
        );
    }

    if ( brushLight )
    {
        this->DrawString(
            sU.c_str(),
            (INT)sU.length(),
            &font,
            layoutRect,
            &fmt,
            brushLight
        );
    }
}

void Graphics::DrawShadowFrame( Gdiplus::RectF const & rect, Gdiplus::Pen const * penLight, Gdiplus::Pen const * penDark, float round )
{
    if ( penDark ) this->DrawRoundRectangle( penDark, Gdiplus::RectF( rect.X + 1, rect.Y + 1, rect.Width, rect.Height ), round );
    if ( penLight ) this->DrawRoundRectangle( penLight, rect, round );
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

WINPLUS_FUNC_IMPL(SIZE) GetHdcWindowSize( HDC hDC )
{
    HWND hWnd = WindowFromDC(hDC);
    RECT rc;
    GetWindowRect( hWnd, &rc );
    return { rc.right - rc.left, rc.bottom - rc.top };
}

WINPLUS_FUNC_IMPL(SIZE) GetHdcBitmapSize( HDC hDC )
{
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject( hDC, OBJ_BITMAP );
    BITMAP bm;
    GetObject( hBitmap, sizeof( bm ), &bm );
    return { bm.bmWidth, bm.bmHeight };
}

// class MemDC ----------------------------------------------------------------------------
MemDC::MemDC( void )
{
    this->_zeroInit();
}

MemDC::MemDC( HDC hDC )
{
    this->_zeroInit();

    this->create(hDC);
}

MemDC::MemDC( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground )
{
    this->_zeroInit();

    this->create( hDC, nWidth, nHeight, clrBackground );
}

MemDC::MemDC( MemDC const & other )
{
    this->_zeroInit();

    this->create( other, other.width(), other.height() );
    _isTransparent = other._isTransparent;
    _background = other._background;
    _transparent = other._transparent;
    other.copyEntireTo( _hMemDC, 0, 0 );
}

MemDC & MemDC::operator = ( MemDC const & other )
{
    if ( this != &other && (BOOL)other )
    {
        this->create( other, other.width(), other.height() );
        _isTransparent = other._isTransparent;
        _background = other._background;
        _transparent = other._transparent;
        other.copyEntireTo( _hMemDC, 0, 0 );
    }
    return *this;
}

MemDC::~MemDC( void )
{
    this->destroy();
}

BOOL MemDC::passTo( MemDC & other )
{
    if ( &other != this )
    {
        other.destroy();
        other._isTransparent = _isTransparent;
        other._transparent = _transparent;
        other._background = _background;
        other._hOldBitmap = _hOldBitmap;
        other._hBitmap = _hBitmap;
        other._hMemBitmap = _hMemBitmap;
        other._hMemDC = _hMemDC;
        other._height = _height;
        other._width = _width;
        this->_zeroInit();
    }
    return FALSE;
}

void MemDC::_zeroInit()
{
    _width = 0;                     // 宽度
    _height = 0;                    // 高度
    _hMemDC = NULL;                 // 内存DC
    _hMemBitmap = NULL;             // 内存位图
    _hBitmap = NULL;                // 图片位图
    _hOldBitmap = NULL;             // 从DC中选出的位图
    _background = RGB( 0, 0, 0 );   // 背景色
    _transparent = RGB( 0, 0, 0 );  // 透明色
    _isTransparent = FALSE;         // 是否透明
}

BOOL MemDC::create( HDC hDC )
{
    this->destroy();

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
    if ( !this->create(hDC) )
    {
        return FALSE;
    }

    HBITMAP hMemBitmap;
    if ( hDC == NULL )
    {
        hDC = GetDC(HWND_DESKTOP);
        hMemBitmap = CreateCompatibleBitmap( hDC, nWidth, nHeight );
        ReleaseDC( HWND_DESKTOP, hDC );
    }
    else
    {
        hMemBitmap = CreateCompatibleBitmap( hDC, nWidth, nHeight );
    }
    if ( hMemBitmap == NULL ) // 位图创建失败
    {
        DWORD dwError = GetLastError();
        return FALSE;
    }

    // 选入内存位图
    _hMemBitmap = hMemBitmap;
    if ( _hMemDC && hMemBitmap )
    {
        _hOldBitmap = (HBITMAP)SelectObject( _hMemDC, hMemBitmap );
        BITMAP bm;
        GetObject( hMemBitmap, sizeof(BITMAP), &bm );
        _width = bm.bmWidth;
        _height = bm.bmHeight;
    }
    return TRUE;
}

BOOL MemDC::create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground )
{
    if ( !this->create( hDC, nWidth, nHeight ) )
    {
        return FALSE;
    }
    this->setBackground( clrBackground, TRUE );
    return TRUE;
}

BOOL MemDC::create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground, COLORREF clrTransparent )
{
    if ( !this->create( hDC, nWidth, nHeight, clrBackground ) )
    {
        return FALSE;
    }
    _transparent = clrTransparent;
    _isTransparent = TRUE;

    return TRUE;
}

void MemDC::destroy( void )
{
    if ( _hOldBitmap )
    {
        SelectObject( _hMemDC, _hOldBitmap );
    }
    if ( _hMemBitmap )
    {
        DeleteObject(_hMemBitmap);
    }
    if ( _hBitmap )
    {
        DeleteObject(_hBitmap);
    }
    if ( _hMemDC )
    {
        DeleteDC(_hMemDC);
    }
    this->_zeroInit();
}

void MemDC::enableTransparent( BOOL bIsTransparent, COLORREF clrTransparent )
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
    HBITMAP hPrev = this->detachBitmap();

    _hBitmap = hBitmap;
    if ( _hMemDC && hBitmap )
    {
        // 如果没有选入内存位图，则保留下内存DC内的原始位图
        if ( _hMemBitmap == NULL )
            _hOldBitmap = (HBITMAP)SelectObject( _hMemDC, hBitmap );
        else
            SelectObject( _hMemDC, hBitmap );

        BITMAP bm;
        GetObject( hBitmap, sizeof(BITMAP), &bm );
        _width = bm.bmWidth;
        _height = bm.bmHeight;
    }

    return hPrev;
}

HBITMAP MemDC::detachBitmap( void )
{
    HBITMAP h = _hBitmap;
    _hBitmap = NULL;
    return h;
}

#if defined(_GDIPLUS_H)
BOOL MemDC::rotate( DOUBLE angle, MemDC * pMemDC )
{
    using namespace Gdiplus;
    Bitmap bmp( _hMemBitmap, NULL );
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

BOOL MemDC::stretchTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    nMode = SetStretchBltMode( hDestDC, nMode );
    BOOL b = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, width, height, SRCCOPY );
    SetStretchBltMode( hDestDC, nMode );
    return b;
}

BOOL MemDC::stretchEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode ) const
{
    return this->stretchTo( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, _width, _height, nMode );
}

BOOL MemDC::stretchFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT nMode ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    nMode = SetStretchBltMode( _hMemDC, nMode );
    BOOL b = ::StretchBlt( _hMemDC, x, y, width, height, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY );
    SetStretchBltMode( _hMemDC, nMode );
    return b;
}

BOOL MemDC::copyTo( HDC hDestDC, INT xDest, INT yDest, INT nWidth, INT nHeight, INT x, INT y ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    return ::BitBlt( hDestDC, xDest, yDest, nWidth, nHeight, _hMemDC, x, y, SRCCOPY );
}

BOOL MemDC::copyEntireTo( HDC hDestDC, INT xDest, INT yDest ) const
{
    return this->copyTo( hDestDC, xDest, yDest, _width, _height, 0, 0 );
}

BOOL MemDC::copyFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nWidth, INT nHeight, INT x, INT y ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    return ::BitBlt( _hMemDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, SRCCOPY );
}

BOOL MemDC::transparentTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    nMode = SetStretchBltMode( hDestDC, nMode );
    BOOL b;
    if ( this->isTransparent() )
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

BOOL MemDC::transparentEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode ) const
{
    return this->transparentTo( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, _width, _height, nMode );
}

BOOL MemDC::transparentFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT nMode ) const
{
    if ( _hMemDC == NULL ) return FALSE;

    nMode = SetStretchBltMode( _hMemDC, nMode );
    BOOL b;
    if ( this->isTransparent() )
    {
        b = ::TransparentBlt( _hMemDC, x, y, width, height, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, (UINT)_transparent );
    }
    else
    {
        b = ::StretchBlt( _hMemDC, x, y, width, height, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY );
    }
    SetStretchBltMode( _hMemDC, nMode );
    return b;
}

BOOL MemDC::alphaTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT alpha ) const
{
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;
    return AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, _hMemDC, x, y, width, height, blend );
}

BOOL MemDC::alphaEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT alpha ) const
{
    return this->alphaTo( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, _width, _height, alpha );
}

BOOL MemDC::alphaFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT alpha ) const
{
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;
    return AlphaBlend( _hMemDC, x, y, width, height, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, blend );
}

SimplePointer<Gdiplus::Bitmap> MemDC::obtainGdiplusBitmap( void ) const
{
    HBITMAP hBitmap = this->getBitmap();
    BITMAP bm = { 0 };
    GetObject( hBitmap, sizeof(BITMAP), &bm );

    if ( bm.bmBitsPixel != 32 )
    {
        return MakeSimple( new Gdiplus::Bitmap( hBitmap, NULL ) );
    }
    else
    {
        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = bm.bmWidth;
        bmi.bmiHeader.biHeight = -bm.bmHeight;
        bmi.bmiHeader.biPlanes = bm.bmPlanes;
        bmi.bmiHeader.biBitCount = bm.bmBitsPixel;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0;

        _bmBitsData.alloc( bm.bmWidthBytes * bm.bmHeight );
        int nLines = GetDIBits( _hMemDC, hBitmap, 0, bm.bmHeight, _bmBitsData.get(), &bmi, DIB_RGB_COLORS );

        return MakeSimple( new Gdiplus::Bitmap( bm.bmWidth, bm.bmHeight, bm.bmWidthBytes, PixelFormat32bppPARGB, _bmBitsData.get<BYTE>() ) );
    }
}

#if defined(_GDIPLUS_H)
// class MemImage -------------------------------------------------------------------------
MemImage::MemImage( void )
{
    this->_zeroInit();
}

MemImage::MemImage( int nWidth, int nHeight )
{
    this->_zeroInit();
    this->create( nWidth, nHeight );
}

MemImage::MemImage( String const & imgFile )
{
    this->_zeroInit();
    this->create(imgFile);
}

MemImage::MemImage( MemImage & other )
{
    this->_zeroInit();
    this->operator = (other);
}

MemImage::~MemImage( void )
{
    this->destroy();
}

MemImage & MemImage::operator = ( MemImage const & other )
{
    this->clone(other);
    return *this;
}

BOOL MemImage::create( int nWidth, int nHeight )
{
    this->destroy();
    _pBmpImage = new Gdiplus::Bitmap( nWidth, nHeight, PixelFormat32bppPARGB );
    return _pBmpImage != NULL;
}

BOOL MemImage::create( String const & imgFile )
{
    this->destroy();
    _pBmpImage = new Gdiplus::Bitmap( StringToUnicode(imgFile).c_str(), FALSE );
    return _pBmpImage != NULL;
}

BOOL MemImage::create( IStreamPtr streamPtr )
{
    this->destroy();
    _pBmpImage = new Gdiplus::Bitmap( streamPtr, FALSE );
    return _pBmpImage != NULL;
}

BOOL MemImage::clone( MemImage const & other )
{
    if ( this != &other )
    {
        this->destroy();
        _pBmpImage = (Gdiplus::Bitmap *)other._pBmpImage->Clone( 0, 0, other.width(), other.height(), other._pBmpImage->GetPixelFormat() );
        return TRUE;
    }
    return FALSE;
}

BOOL MemImage::copyFrom( Gdiplus::Image * pImage )
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

SimpleHandle<HBITMAP> MemImage::obtainHBITMAP() const
{
    Gdiplus::Rect imgRect( 0, 0, this->width(), this->height() );
    HBITMAP hNewBitmap = NULL;
    LPVOID lpBmpBits = NULL;
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = imgRect.Width;
    bmi.bmiHeader.biHeight = -imgRect.Height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    //bmi.bmiHeader.biSizeImage = imgRect.Width * imgRect.Height * ( bmi.bmiHeader.biBitCount / 8 );
    DWORD dwImageSizeBytes = ( ( imgRect.Width * bmi.bmiHeader.biBitCount + 31 ) / 32 ) * imgRect.Height * ( bmi.bmiHeader.biBitCount / 8 );

    hNewBitmap = CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &lpBmpBits, NULL, 0 );
    Gdiplus::BitmapData bmpData;
    this->_pBmpImage->LockBits( &imgRect, Gdiplus::ImageLockModeRead, _pBmpImage->GetPixelFormat(), &bmpData );
    memcpy( lpBmpBits, bmpData.Scan0, dwImageSizeBytes );
    this->_pBmpImage->UnlockBits(&bmpData);

    return SimpleHandle<HBITMAP>( hNewBitmap, NULL, DeleteObject );
}

void MemImage::_zeroInit()
{
    _pBmpImage = NULL;
}

#endif

} // namespace winplus

std::ostream & operator << ( std::ostream & out, RECT const & rc )
{
    return out << "RECT( LT(" << rc.left << ", " << rc.top << "), RB(" << rc.right << ", " << rc.bottom << "), SIZE(" << winplus::RectWidth(rc) << ", " << winplus::RectHeight(rc) << ") )";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::Rect const & rc )
{
    return out << "Rect( LT(" << rc.GetLeft() << ", " << rc.GetTop() << "), RB(" << rc.GetRight() << ", " << rc.GetBottom() << "), SIZE(" << rc.Width << ", " << rc.Height << ") )";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::RectF const & rc )
{
    return out << "RectF( LT(" << rc.GetLeft() << ", " << rc.GetTop() << "), RB(" << rc.GetRight() << ", " << rc.GetBottom() << "), SIZE(" << rc.Width << ", " << rc.Height << ") )";
}

std::ostream & operator << ( std::ostream & out, POINT const & pt )
{
    return out << "POINT(" << pt.x << ", " << pt.y << ")";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::Point const & pt )
{
    return out << "Point(" << pt.X << ", " << pt.Y << ")";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::PointF const & pt )
{
    return out << "PointF(" << pt.X << ", " << pt.Y << ")";
}

std::ostream & operator << ( std::ostream & out, SIZE const & si )
{
    return out << "SIZE(" << si.cx << ", " << si.cy << ")";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::Size const & si )
{
    return out << "Size(" << si.Width << ", " << si.Height << ")";
}

std::ostream & operator << ( std::ostream & out, Gdiplus::SizeF const & si )
{
    return out << "SizeF(" << si.Width << ", " << si.Height << ")";
}
