//////////////////////////////////////////////////////////////////////////
// purpose: 绘图相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_GRAPHICS_HPP__)
#define __WINPLUS_GRAPHICS_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable: 4244 )

namespace winplus
{

// 坐标转换系列函数 -------------------------------------------------------

/* 从缩放到实际 */
template < typename _RECT >
inline _RECT RectFromScale( _RECT const & rect, DOUBLE scale )
{
    _RECT rc;
    rc.left = rect.left / scale;
    rc.top = rect.top / scale;
    rc.right = rect.right / scale;
    rc.bottom = rect.bottom / scale;
    return rc;
}

template < typename _SIZE >
inline _SIZE SizeFromScale( _SIZE const & si, DOUBLE scale )
{
    _SIZE si2;
    si2.cx = si.cx / scale;
    si2.cy = si.cy / scale;
    return si2;
}

/* 实际到缩放 */
template < typename _RECT >
inline _RECT RectToScale( _RECT const & rect, DOUBLE scale )
{
    _RECT rc;
    rc.left = rect.left * scale;
    rc.top = rect.top * scale;
    rc.right = rect.right * scale;
    rc.bottom = rect.bottom * scale;
    return rc;
}

template < typename _SIZE >
inline _SIZE SizeToScale( _SIZE const & si, DOUBLE scale )
{
    _SIZE si2;
    si2.cx = si.cx * scale;
    si2.cy = si.cy * scale;
    return si2;
}

/* 矩形相交 */
template < typename _RECT >
inline _RECT RectIntersect( _RECT const & rc1, _RECT const & rc2 )
{
    _RECT rc;
    rc.left = rc1.left > rc2.left ? rc1.left : rc2.left;
    rc.top = rc1.top > rc2.top ? rc1.top : rc2.top;
    rc.right = rc1.right < rc2.right ? rc1.right : rc2.right;
    rc.bottom = rc1.bottom < rc2.bottom ? rc1.bottom : rc2.bottom;
    return rc;
}

/* 矩形联合 */
template < typename _RECT >
inline _RECT RectUnion( _RECT const & rc1, _RECT const & rc2 )
{
    _RECT rc;
    rc.left = rc1.left < rc2.left ? rc1.left : rc2.left;
    rc.top = rc1.top < rc2.top ? rc1.top : rc2.top;
    rc.right = rc1.right > rc2.right ? rc1.right : rc2.right;
    rc.bottom = rc1.bottom > rc2.bottom ? rc1.bottom : rc2.bottom;
    return rc;
}

/* 获取包含住一些矩形的矩形 */
template < typename _RECT >
inline _RECT RectIncludeRects( _RECT const * arrRect, INT count )
{
    _RECT rcResult;
    rcResult.left = rcResult.top = rcResult.right = rcResult.bottom = 0;
    if ( count < 1 )
    {
        return rcResult;
    }
    rcResult.left = arrRect[0].left;
    rcResult.top = arrRect[0].top;
    rcResult.right = arrRect[0].right;
    rcResult.bottom = arrRect[0].bottom;
    INT i;
    for ( i = 1; i < count; ++i )
    {
        _RECT const & rc = arrRect[i];
        if ( rc.left < rcResult.left )
        {
            rcResult.left = rc.left;
        }
        if ( rc.top < rcResult.top )
        {
            rcResult.top = rc.top;
        }
        if ( rc.right > rcResult.right )
        {
            rcResult.right = rc.right;
        }
        if ( rc.bottom > rcResult.bottom )
        {
            rcResult.bottom = rc.bottom;
        }
    }
    return rcResult;
}

/* 正规化矩形,调用这个函数使得一个矩形正规化 */
template < typename _RECT >
inline _RECT RectNormalize( _RECT const & rect )
{
    _RECT rcResult = rect;
    if ( rcResult.right < rcResult.left )
    {
        std::swap( rcResult.left, rcResult.right );
    }
    if ( rcResult.bottom < rcResult.top )
    {
        std::swap( rcResult.top, rcResult.bottom );
    }
    return rcResult;
}

/* 矩形宽 */
template < typename _RETVAL, typename _RECT >
inline _RETVAL RectWidth( _RECT const & rc )
{
    return rc.right - rc.left;
}
/* 矩形高 */
template < typename _RETVAL, typename _RECT >
inline _RETVAL RectHeight( _RECT const & rc )
{
    return rc.bottom - rc.top;
}

/* 旋转之后大小 */
inline SIZE SizeAfterRotate( DOUBLE angle, INT width, INT height )
{
    DOUBLE ang = -angle * acos(0.0f) / 90;
    SIZE si;
    //INT newWidth, newHeight;
    DOUBLE cos_angle = cos(ang);
    DOUBLE sin_angle = sin(ang);
    
    // Calculate the size of the new bitmap
    POINT p1 = { 0, 0 };
    POINT p2 = { width, 0 };
    POINT p3 = { 0, height };
    POINT p4 = { width - 1, height };
    POINT newP1, newP2, newP3, newP4, leftTop, rightTop, leftBottom, rightBottom;
    
    newP1.x = p1.x;
    newP1.y = p1.y;
    newP2.x = (long)floor( p2.x * cos_angle - p2.y * sin_angle );
    newP2.y = (long)floor( p2.x * sin_angle + p2.y * cos_angle );
    newP3.x = (long)floor( p3.x * cos_angle - p3.y * sin_angle );
    newP3.y = (long)floor( p3.x * sin_angle + p3.y * cos_angle );
    newP4.x = (long)floor( p4.x * cos_angle - p4.y * sin_angle );
    newP4.y = (long)floor( p4.x * sin_angle + p4.y * cos_angle );
    
    leftTop.x = min( min( newP1.x, newP2.x ), min( newP3.x, newP4.x ) );
    leftTop.y = min( min( newP1.y, newP2.y ), min( newP3.y, newP4.y ) );
    rightBottom.x = max( max( newP1.x, newP2.x ), max( newP3.x, newP4.x ) );
    rightBottom.y = max( max( newP1.y, newP2.y ), max( newP3.y, newP4.y ) );
    leftBottom.x = leftTop.x;
    leftBottom.y = 2 + rightBottom.y;
    rightTop.x = 2 + rightBottom.x;
    rightTop.y = leftTop.y;
    
    si.cx = rightTop.x - leftTop.x;
    si.cy = leftBottom.y - leftTop.y;

    return si;
}

inline SIZE SizeAfterRotate( DOUBLE angle, SIZE const & si )
{
    return SizeAfterRotate( angle, si.cx, si.cy );
}

/* 点 */
struct GdiPoint
{
    DOUBLE x;
    DOUBLE y;
    GdiPoint( DOUBLE x = 0, DOUBLE y = 0 ) : x(x), y(y) {}
    explicit GdiPoint( POINT const & pt )
    {
        x = pt.x;
        y = pt.y;
    }
    POINT getPOINT() const
    {
        POINT pt = { x, y };
        return pt;
    }
};

/* 尺寸 */
struct GdiSize
{
    DOUBLE cx;
    DOUBLE cy;
    GdiSize( DOUBLE cx = 0, DOUBLE cy = 0 ) : cx(cx), cy(cy) {}
    explicit GdiSize( SIZE const & si )
    {
        cx = si.cx;
        cy = si.cy;
    }
    SIZE getSIZE() const
    {
        SIZE si = { cx, cy };
        return si;
    }
};

/* 矩形 */
struct GdiRect
{
    DOUBLE left;
    DOUBLE top;
    DOUBLE right;
    DOUBLE bottom;
    GdiRect( DOUBLE left = 0, DOUBLE top = 0, DOUBLE right = 0, DOUBLE bottom = 0 ) : left(left), top(top), right(right), bottom(bottom) {}
    GdiRect( GdiPoint const & point, GdiSize const & size ) : left(point.x), top(point.y), right( point.x + size.cx ), bottom( point.y + size.cy ) {}
    GdiRect( GdiPoint const & point1, GdiPoint const & point2 ) : left(point1.x), top(point1.y), right(point2.x), bottom(point2.y) {}
    explicit GdiRect( RECT const & rc )
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
    }
    RECT getRECT() const
    {
        RECT rect = { left, top, right, bottom };
        return rect;
    }
    void offset( DOUBLE cx, DOUBLE cy )
    {
        left += cx;
        right += cx;
        top += cy;
        bottom += cy;
    }
    GdiPoint topLeft() const
    {
        GdiPoint pt;
        pt.x = left;
        pt.y = top;
        return pt;
    }
    DOUBLE width() const { return right - left; }
    DOUBLE height() const { return bottom - top; }
    GdiSize size() const { return GdiSize( width(), height() ); }
    BOOL intersect( GdiRect const & rect1, GdiRect const & rect2 )
    {
        *this = RectIntersect( rect1, rect2 );
        GdiSize const & si = size();
        if ( si.cx > 0 && si.cy > 0 )
        {
            return TRUE;
        }
        return FALSE;
    }
    void union_( GdiRect const & rect1, GdiRect const & rect2 )
    {
        *this = RectUnion( rect1, rect2 );
    }
    bool isEmpty() const
    {
        GdiSize const & si = size();
        return si.cx == 0 || si.cy == 0;
    }
    bool pointIn( GdiPoint const & pt ) const
    {
        if ( pt.x < left || pt.x > right - 1 || pt.y < top || pt.y > bottom - 1 )
        {
            return false;
        }
        return true;
    }
};

/* 固定比例缩放图片到一定范围内,并返回其矩形区域,宽高比,缩放比 */
template < typename _RECT, typename _Type >
inline _RECT RectRecalcImage( _Type ImageWidth, _Type ImageHeight, _Type Width, _Type Height, DOUBLE * pRatioWH, DOUBLE * pScale )
{
    _RECT rectClip;
    *pRatioWH = ImageWidth / (DOUBLE)ImageHeight; // 宽高比
    if ( ImageWidth <= Width && ImageHeight <= Height ) // 图片小于窗口,不需要缩放,居中即可
    {
        *pScale = 1.0;
        rectClip.left = (Width - ImageWidth) / 2;
        rectClip.right = rectClip.left + ImageWidth;
        rectClip.top = (Height - ImageHeight) / 2;
        rectClip.bottom = rectClip.top + ImageHeight;
    }
    else if ( ImageWidth > Width &&  ImageHeight <= Height ) // 图片比窗口宽,则需要以窗口宽度为准,等比例缩小图片
    {
        *pScale = Width / (DOUBLE)ImageWidth; // 计算缩放因子
        ImageWidth = Width;
        ImageHeight = ImageWidth / *pRatioWH;
        
        rectClip.left = (Width - ImageWidth) / 2;
        rectClip.right = rectClip.left + ImageWidth;
        rectClip.top = (Height - ImageHeight) / 2;
        rectClip.bottom = rectClip.top + ImageHeight;
    }
    else if ( ImageWidth <= Width && ImageHeight > Height ) // 图片比窗口高
    {
        *pScale = Height / (DOUBLE)ImageHeight; // 缩放因子
        ImageHeight = Height;
        ImageWidth = ImageHeight * *pRatioWH;
        
        rectClip.left = (Width - ImageWidth) / 2;
        rectClip.right = rectClip.left + ImageWidth;
        rectClip.top = (Height - ImageHeight) / 2;
        rectClip.bottom = rectClip.top + ImageHeight;
    }
    else if ( ImageWidth > Width && ImageHeight > Height ) // 图片宽高都比窗口大
    {
        *pScale = Width / (DOUBLE)ImageWidth;
        INT nTestWidth, nTestHeight;
        nTestWidth = Width;
        nTestHeight = nTestWidth / *pRatioWH;
        if ( nTestHeight > Height )
        {
            *pScale = Height / (DOUBLE)ImageHeight;
            ImageHeight = Height;
            ImageWidth = ImageHeight * *pRatioWH;
        }
        else
        {
            ImageWidth = nTestWidth;
            ImageHeight = nTestHeight;
        }
        
        rectClip.left = (Width - ImageWidth) / 2;
        rectClip.right = rectClip.left + ImageWidth;
        rectClip.top = (Height - ImageHeight) / 2;
        rectClip.bottom = rectClip.top + ImageHeight;
    }
    return rectClip;
}

/* GDI设备环境对象选入器，自动恢复先前的对象，以scope为有效期 */
template < typename _GdiObject >
class GdiObjectSelector
{
public:
    GdiObjectSelector( HDC hDC, HGDIOBJ hGdiObject ) : _hDC(hDC), _hGdiObjectOld(NULL)
    {
        _hGdiObjectOld = (_GdiObject)SelectObject( _hDC, hGdiObject );
    }
    ~GdiObjectSelector()
    {
        SelectObject( _hDC, _hGdiObjectOld );
    }
private:
    HDC _hDC;
    _GdiObject _hGdiObjectOld;
    GdiObjectSelector( GdiObjectSelector const & );
    GdiObjectSelector & operator = ( GdiObjectSelector const & );
};

// GDI+ -------------------------------------------------------------------
#if defined(_GDIPLUS_H)

class WINPLUS_DLL GdiplusInit
{
    Gdiplus::GdiplusStartupInput _gdiplusStartupInput;
    ULONG_PTR _gdiplusToken;
public:
    GdiplusInit();
    ~GdiplusInit();
};

/* 旋转后大小 */
inline Gdiplus::Size SizeAfterRotate( DOUBLE angle, Gdiplus::Size const & si )
{
    SIZE siTemp = SizeAfterRotate( angle, si.Width, si.Height );
    return Gdiplus::Size( siTemp.cx, siTemp.cy );
}

/* 指定点的外切矩形 */
inline Gdiplus::RectF RectCircumscribePoints( Gdiplus::PointF const points[4] )
{
    Gdiplus::PointF ptNewRectLT, ptNewRectRB;
    ptNewRectLT.X = (((((((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) < (points[2].X)) ? ((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) < (points[3].X)) ? (((((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) < (points[2].X)) ? ((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) : (points[3].X));
    ptNewRectLT.Y = (((((((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) < (points[2].Y)) ? ((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) < (points[3].Y)) ? (((((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) < (points[2].Y)) ? ((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) : (points[3].Y));
    ptNewRectRB.X = (((((((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) > (points[2].X)) ? ((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) > (points[3].X)) ? (((((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) > (points[2].X)) ? ((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) : (points[3].X));
    ptNewRectRB.Y = (((((((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) > (points[2].Y)) ? ((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) > (points[3].Y)) ? (((((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) > (points[2].Y)) ? ((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) : (points[3].Y));
    
    return Gdiplus::RectF( ptNewRectLT, Gdiplus::SizeF( ptNewRectRB.X - ptNewRectLT.X, ptNewRectRB.Y - ptNewRectLT.Y ) );
}

/* 将一个COLORREF转换为Color */
inline Gdiplus::Color COLORREF_to_Color( COLORREF clr, BYTE alpha = 255 )
{
    return Gdiplus::Color( alpha, GetRValue(clr), GetGValue(clr), GetBValue(clr) );
}
/* 将gdi+的矩形转换到gdi矩形 */
template < typename _RECT, typename _GdipRect >
inline _RECT RectGdiplusToGdi( _GdipRect const & rect )
{
    _RECT rc;
    rc.left = rect.GetLeft();
    rc.top = rect.GetTop();
    rc.right = rect.GetRight();
    rc.bottom = rect.GetBottom();
    return rc;
}
/* 将gdi的矩形转换到gdi+矩形 */
template < typename _GdipRect, typename _RECT >
inline _GdipRect RectGdiToGdiplus( _RECT const & rect )
{
    _GdipRect rc;
    rc.X = rect.left;
    rc.Y = rect.top;
    rc.Width = rect.right - rect.left;
    rc.Height = rect.bottom - rect.top;
    return rc;
}
/* 绘制圆角矩形 */
WINPLUS_FUNC_DECL(void) DrawRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Pen const & pen, Gdiplus::RectF const & rect, Gdiplus::REAL round );
/* 填充圆角矩形 */
WINPLUS_FUNC_DECL(void) FillRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Brush const & brush, Gdiplus::RectF const & rect, Gdiplus::REAL round );

#endif

//////////////////////////////////////////////////////////////////////
// 内存DC,管理绘制与位图显示
// 此类不得含有虚函数,继承此类意味着也不得含虚函数
//////////////////////////////////////////////////////////////////////
class WINPLUS_DLL MemDC
{
public:
    MemDC( void );
    MemDC( HDC hDC );
    MemDC( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground = 0xFFFFFF );
    MemDC( MemDC & other );
    ~MemDC( void );

    MemDC & operator = ( MemDC const & other );
    operator HDC( void ) const { return _hMemDC; }
    operator HBITMAP( void ) const { return _hBitmap; }
    operator BOOL( void ) const { return _hMemDC != NULL && _hBitmap != NULL; }

    INT width( void ) const { return _width; }
    INT height( void ) const { return _height; }
    //void SetSizes( SIZE si ) { _width = si.cx; _height = si.cy; }
    BOOL isTransparent( void ) const { return _isTransparent; }
    COLORREF getTransparentColor( void ) const { return _transparent; }

    BOOL create( HDC hDC );
    BOOL create( HDC hDC, INT nWidth, INT nHeight );
    BOOL create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground );
    BOOL create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground, COLORREF clrTransparent );
    BOOL copy( MemDC const & other );
    void destroy( void );

    void enableTransparent( BOOL bIsTransparent, COLORREF clrTransparent = 0 );
    void setBackground( COLORREF clrBackground, BOOL bFill = FALSE );
    /* 管理一副位图,如果只是输出现有位图,则完毕后应该调用DetachBitmap()脱离管理 */
    HBITMAP attachBitmap( HBITMAP hBitmap );
    /* 位图脱离管理 */
    HBITMAP detachBitmap( void );
    /* 传递管理权,自己放弃管理资源 */
    BOOL passTo( MemDC & other );
    /* 旋转 */
#if defined(_GDIPLUS_H)
    BOOL rotate( DOUBLE angle, MemDC * pMemDC );
#endif

    BOOL stretchBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;
    BOOL stretchToDC( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode = HALFTONE ) const;
    BOOL bitBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y ) const;
    BOOL copyToDC( HDC hDestDC, INT xDest, INT yDest ) const;
    BOOL transparentBlt( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;
    BOOL transparentToDC( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode = HALFTONE ) const;
protected:
    void _construct();

private:
    INT _width, _height;
    HDC _hMemDC;
    HBITMAP _hBitmap;
    COLORREF _background; // 背景色
    COLORREF _transparent; // 透明色
    BOOL _isTransparent; // 是否透明
};

#if defined(_GDIPLUS_H)
//////////////////////////////////////////////////////////////////////
// 内存图片,管理绘制,支持alpha通道.
//////////////////////////////////////////////////////////////////////
class WINPLUS_DLL MemImage
{
public:
    MemImage( void );
    MemImage( int nWidth, int nHeight );
    MemImage( MemImage & other );
    ~MemImage( void );
    
    MemImage & operator = ( MemImage const & other );
    operator BOOL() const { return _pBmpImage != NULL; }
    operator Gdiplus::Bitmap * () const { return _pBmpImage; }

    int width( void ) const { return _pBmpImage ? _pBmpImage->GetWidth() : 0; }
    int height( void ) const { return _pBmpImage ? _pBmpImage->GetHeight() : 0; }

    BOOL create( int nWidth, int nHeight );
    BOOL create( Gdiplus::Size const & size ) { return create( size.Width, size.Height ); };
    void destroy( void );
    BOOL clone( MemImage const & other );
    BOOL copy( Gdiplus::Image * pImage );
    /* 传递管理权,自己放弃管理资源 */
    BOOL passTo( MemImage & other );
    /* 以中心点旋转angle角度 */
    BOOL rotate( double angle, MemImage * pMemImage );
    
    BOOL stretch( Gdiplus::Graphics & gDest, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const;
    BOOL stretchEntire( Gdiplus::Graphics & gDest, int xDest, int yDest, int nDestWidth, int nDestHeight ) const
    {
        return stretch( gDest, xDest, yDest, nDestWidth, nDestHeight, 0, 0, width(), height() );
    }
    BOOL output( Gdiplus::Graphics & gDest, int xDest, int yDest, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const;
    BOOL outputEntire( Gdiplus::Graphics & gDest, int xDest, int yDest ) const
    {
        return output( gDest, xDest, yDest, 0, 0, width(), height() );
    }
    
    BOOL stretch( MemImage & imgDest, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
    {
        if ( this != &imgDest )
        {
            return stretch( Gdiplus::Graphics(imgDest._pBmpImage), xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
        }
        return FALSE;
    }
    BOOL stretchEntire( MemImage & imgDest, int xDest, int yDest, int nDestWidth, int nDestHeight ) const
    {
        return stretch( imgDest, xDest, yDest, nDestWidth, nDestHeight, 0, 0, width(), height() );
    }
    BOOL output( MemImage & imgDest, int xDest, int yDest, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
    {
        if ( this != &imgDest )
        {
            return output( Gdiplus::Graphics(imgDest._pBmpImage), xDest, yDest, xSrc, ySrc, nSrcWidth, nSrcHeight );
        }
        return FALSE;
    }
    BOOL outputEntire( MemImage & imgDest, int xDest, int yDest ) const
    {
        return output( imgDest, xDest, yDest, 0, 0, width(), height() );
    }
protected:
    void _construct();

private:
    Gdiplus::Bitmap * _pBmpImage;
};

#endif

} // namespace winplus

#endif // !defined(__WINPLUS_GRAPHICS_HPP__)
