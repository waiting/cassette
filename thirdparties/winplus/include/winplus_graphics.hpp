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

/** \brief 从缩放到实际 */
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

/** \brief 实际到缩放 */
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

/** \brief 矩形相交 */
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

/** \brief 矩形联合 */
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

/** \brief 获取包含住一些矩形的矩形 */
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

/** \brief 正规化矩形，调用这个函数使得一个矩形正规化 */
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

/** \brief 矩形宽 */
template < typename _RECT >
inline auto RectWidth( _RECT const & rc ) -> decltype(rc.left)
{
    return rc.right - rc.left;
}

/** \brief 矩形高 */
template < typename _RECT >
inline auto RectHeight( _RECT const & rc ) -> decltype(rc.top)
{
    return rc.bottom - rc.top;
}

/** \brief 旋转之后大小 */
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

/** \brief 点 */
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
        POINT pt = { (LONG)x, (LONG)y };
        return pt;
    }
};

/** \brief 尺寸 */
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
        SIZE si = { (LONG)cx, (LONG)cy };
        return si;
    }
};

/** \brief 矩形 */
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
        RECT rect = { (LONG)left, (LONG)top, (LONG)right, (LONG)bottom };
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

/** \brief 固定比例缩放图片到一定范围内，并返回其矩形区域、宽高比、缩放比 */
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

/** \brief GDI设备环境对象选入器，自动恢复先前的对象，以scope为有效期 */
class GdiObjectSelector
{
public:
    template < typename _GdiObject >
    GdiObjectSelector( HDC hDC, _GdiObject hGdiObject ) : _hDC(hDC), _hGdiObjectOld(NULL)
    {
        _hGdiObjectOld = SelectObject( _hDC, hGdiObject );
    }
    ~GdiObjectSelector()
    {
        SelectObject( _hDC, _hGdiObjectOld );
    }
private:
    HDC _hDC;
    HGDIOBJ _hGdiObjectOld;
    DISABLE_OBJECT_COPY(GdiObjectSelector)
};

// GDI+ -------------------------------------------------------------------
#if defined(_GDIPLUS_H)
/** \brief GDI+ 初始化 */
class WINPLUS_DLL GdiplusInit
{
    Gdiplus::GdiplusStartupInput _gdiplusStartupInput;
    Gdiplus::GdiplusStartupOutput _gdiplusStartupOutput;
    ULONG_PTR _gdiplusToken;
    bool _canShutdown;
public:
    GdiplusInit();
    ~GdiplusInit();
    void canShutdown( bool b ) { _canShutdown = b; }
    DISABLE_OBJECT_COPY(GdiplusInit)
};

/** \brief 旋转后大小 */
inline Gdiplus::Size SizeAfterRotate( DOUBLE angle, Gdiplus::Size const & si )
{
    SIZE siTemp = SizeAfterRotate( angle, si.Width, si.Height );
    return Gdiplus::Size( siTemp.cx, siTemp.cy );
}

/** \brief 指定点的外切矩形 */
inline Gdiplus::RectF RectCircumscribePoints( Gdiplus::PointF const points[4] )
{
    Gdiplus::PointF ptNewRectLT, ptNewRectRB;
    ptNewRectLT.X = (((((((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) < (points[2].X)) ? ((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) < (points[3].X)) ? (((((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) < (points[2].X)) ? ((((points[0].X) < (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) : (points[3].X));
    ptNewRectLT.Y = (((((((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) < (points[2].Y)) ? ((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) < (points[3].Y)) ? (((((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) < (points[2].Y)) ? ((((points[0].Y) < (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) : (points[3].Y));
    ptNewRectRB.X = (((((((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) > (points[2].X)) ? ((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) > (points[3].X)) ? (((((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) > (points[2].X)) ? ((((points[0].X) > (points[1].X)) ? (points[0].X) : (points[1].X))) : (points[2].X))) : (points[3].X));
    ptNewRectRB.Y = (((((((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) > (points[2].Y)) ? ((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) > (points[3].Y)) ? (((((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) > (points[2].Y)) ? ((((points[0].Y) > (points[1].Y)) ? (points[0].Y) : (points[1].Y))) : (points[2].Y))) : (points[3].Y));
    
    return Gdiplus::RectF( ptNewRectLT, Gdiplus::SizeF( ptNewRectRB.X - ptNewRectLT.X, ptNewRectRB.Y - ptNewRectLT.Y ) );
}

/** \brief 将一个COLORREF转换为Color */
inline Gdiplus::Color COLORREF_to_Color( COLORREF clr, BYTE alpha = 255 )
{
    return Gdiplus::Color( alpha, GetRValue(clr), GetGValue(clr), GetBValue(clr) );
}

/** \brief 将GDI+的矩形转换到GDI矩形 */
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

/** \brief 将GDI的矩形转换到GDI+矩形 */
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

/** \brief 绘制类，扩展一些绘制方法 */
class WINPLUS_DLL Graphics : public Gdiplus::Graphics
{
public:
    using Gdiplus::Graphics::Graphics;

    /** \brief 绘制圆角矩形 */
    void DrawRoundRectangle( Gdiplus::Pen const * pen, Gdiplus::RectF const & rect, Gdiplus::REAL round );

    /** \brief 填充圆角矩形 */
    void FillRoundRectangle( Gdiplus::Brush const * brush, Gdiplus::RectF const & rect, Gdiplus::REAL round );

    /** \brief 填充矩形 */
    void FillRectangle( Gdiplus::Brush const * brush, Gdiplus::RectF const & rect );

    /** \brief 绘制一个有阴影的文字 */
    void DrawShadowString( winplus::String const & str, Gdiplus::Font const & font, Gdiplus::Brush const * brushLight, Gdiplus::Brush const * brushDark, Gdiplus::RectF const & layoutRect, Gdiplus::StringFormat const & fmt, Gdiplus::RectF * boundingRect );

    /** \brief 绘制一个阴影圆角的框架 */
    void DrawShadowFrame( Gdiplus::RectF const & rect, Gdiplus::Pen const * penLight, Gdiplus::Pen const * penDark, float round );

};

/** \brief 绘制圆角矩形 */
WINPLUS_FUNC_DECL(void) DrawRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Pen const & pen, Gdiplus::RectF const & rect, Gdiplus::REAL round );

/** \brief 填充圆角矩形 */
WINPLUS_FUNC_DECL(void) FillRoundRectangle( Gdiplus::Graphics & g, Gdiplus::Brush const & brush, Gdiplus::RectF const & rect, Gdiplus::REAL round );

#endif

/** \brief 获取HDC关联的窗口大小 */
WINPLUS_FUNC_DECL(SIZE) GetHdcWindowSize( HDC hDC );

/** \brief 获取HDC内的位图大小 */
WINPLUS_FUNC_DECL(SIZE) GetHdcBitmapSize( HDC hDC );

/** \brief 内存DC，管理绘制与位图显示 */
class WINPLUS_DLL MemDC
{
public:
    MemDC( void );
    MemDC( HDC hDC );
    MemDC( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground = 0xFFFFFF );
    MemDC( MemDC const & other );
    MemDC & operator = ( MemDC const & other );
    ~MemDC( void );

    /** \brief 传递管理权，自己放弃管理资源 */
    BOOL passTo( MemDC & other );
protected:
    /** \brief 成员初始化 */
    void _zeroInit();

public:
    /** \brief 取得HDC */
    operator HDC( void ) const { return _hMemDC; }
    /** \brief 判断是否有效 */
    operator bool( void ) const { return _hMemDC != NULL; }

    /** \brief 宽度 */
    INT width( void ) const { return _width; }
    /** \brief 高度 */
    INT height( void ) const { return _height; }

    /** \brief 取得内存位图 */
    HBITMAP getMemBitmap() const { return _hMemBitmap; }
    /** \brief 取得管理的外部位图 */
    HBITMAP getAttachBitmap() const { return _hBitmap; }
    /** \brief 取得内存DC选入的位图 */
    HBITMAP getBitmap() const { return (HBITMAP)GetCurrentObject( _hMemDC, OBJ_BITMAP ); }

    /** \brief 是否关键色透明 */
    BOOL isTransparent( void ) const { return _isTransparent; }
    /** \brief 取得关键透明色 */
    COLORREF getTransparentColor( void ) const { return _transparent; }

    /** \brief 以设备场景hDC为准创建一个内存兼容DC。如果hDC==NULL，则会用桌面设备场景。 */
    BOOL create( HDC hDC );
    /** \brief 以设备场景hDC为准创建一个内存兼容DC，并创建一个兼容位图选入内存兼容DC。如果hDC==NULL，则会用桌面设备场景。 */
    BOOL create( HDC hDC, INT nWidth, INT nHeight );
    /** \brief 以设备场景hDC为准创建一个内存兼容DC，并创建一个兼容位图选入内存兼容DC，并设置背景颜色。如果hDC==NULL，则会用桌面设备场景。 */
    BOOL create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground );
    /** \brief 以设备场景hDC为准创建一个内存兼容DC，并创建一个兼容位图选入内存兼容DC，并设置背景颜色，并设置透明色。如果hDC==NULL，则会用桌面设备场景。 */
    BOOL create( HDC hDC, INT nWidth, INT nHeight, COLORREF clrBackground, COLORREF clrTransparent );

    /** \brief 销毁管理的所有资源 */
    void destroy( void );

    /** \brief 使能关键色透明 */
    void enableTransparent( BOOL bIsTransparent, COLORREF clrTransparent = 0 );

    /** \brief 设置一个背景色，并填充这个颜色到内存兼容DC */
    void setBackground( COLORREF clrBackground, BOOL bFill = FALSE );

    /** \brief 管理一副位图，并选入内存兼容DC */
    HBITMAP attachBitmap( HBITMAP hBitmap );

    /** \brief 位图脱离管理 */
    HBITMAP detachBitmap( void );

#if defined(_GDIPLUS_H)
    /** \brief 旋转 */
    BOOL rotate( DOUBLE angle, MemDC * pMemDC );
#endif

    /** \brief 伸缩传输到目标DC */
    BOOL stretchTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;
    /** \brief 伸缩传输整个内存DC到目标DC */
    BOOL stretchEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode = HALFTONE ) const;
    /** \brief 从源DC伸缩传输到自身 */
    BOOL stretchFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;

    /** \brief 传输到目标DC */
    BOOL copyTo( HDC hDestDC, INT xDest, INT yDest, INT nWidth, INT nHeight, INT x, INT y ) const;
    /** \brief 传输整个内存DC到目标DC */
    BOOL copyEntireTo( HDC hDestDC, INT xDest, INT yDest ) const;
    /** \brief 从源DC传输到自身 */
    BOOL copyFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nWidth, INT nHeight, INT x, INT y ) const;

    /** \brief 透明传输到目标DC */
    BOOL transparentTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;
    /** \brief 透明传输整个内存DC到目标DC */
    BOOL transparentEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT nMode = HALFTONE ) const;
    /** \brief 从源DC透明传输到自身 */
    BOOL transparentFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT nMode = HALFTONE ) const;

    /** \brief alpha透明传输到目标DC */
    BOOL alphaTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT x, INT y, INT width, INT height, INT alpha = 255 ) const;
    /** \brief alpha透明传输整个内存DC到目标DC */
    BOOL alphaEntireTo( HDC hDestDC, INT xDest, INT yDest, INT nDestWidth, INT nDestHeight, INT alpha = 255 ) const;
    /** \brief alpha从源DC透明传输到自身 */
    BOOL alphaFrom( HDC hSrcDC, INT xSrc, INT ySrc, INT nSrcWidth, INT nSrcHeight, INT x, INT y, INT width, INT height, INT alpha = 255 ) const;

    /** \brief 以内存DC选入的位图内容创建一个新的GDI+位图 */
    SimplePointer<Gdiplus::Bitmap> obtainGdiplusBitmap( void ) const;
private:
    INT _width;             // 宽度
    INT _height;            // 高度
    HDC _hMemDC;            // 内存DC
    HBITMAP _hMemBitmap;    // 内存位图
    HBITMAP _hBitmap;       // 图片位图
    HBITMAP _hOldBitmap;    // 从DC中选出的位图
    COLORREF _background;   // 背景色
    COLORREF _transparent;  // 透明色
    BOOL _isTransparent;    // 是否透明
    mutable Buffer _bmBitsData;     // 转换成Gdiplus::Bitmap的位数据
};

#if defined(_GDIPLUS_H)

// 内存图片(Bitmap 32bits)。管理绘制，支持alpha通道。
class WINPLUS_DLL MemImage
{
public:
    MemImage( void );
    MemImage( int nWidth, int nHeight );
    MemImage( String const & imgFile );
    MemImage( MemImage & other );
    ~MemImage( void );
    
    MemImage & operator = ( MemImage const & other );
    operator bool() const { return _pBmpImage != NULL; }
    operator Gdiplus::Bitmap * () const { return _pBmpImage; }

    int width( void ) const { return _pBmpImage ? _pBmpImage->GetWidth() : 0; }
    int height( void ) const { return _pBmpImage ? _pBmpImage->GetHeight() : 0; }

    BOOL create( int nWidth, int nHeight );
    BOOL create( Gdiplus::Size const & size ) { return this->create( size.Width, size.Height ); };
    BOOL create( String const & imgFile );
    BOOL create( IStreamPtr streamPtr );

    void destroy( void );
    BOOL clone( MemImage const & other );
    BOOL copyFrom( Gdiplus::Image * pImage );
    /** 传递管理权，自己放弃管理资源 */
    BOOL passTo( MemImage & other );
    /** 以中心点旋转angle角度 */
    BOOL rotate( double angle, MemImage * pMemImage );

    BOOL stretch( Gdiplus::Graphics & gDest, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const;
    BOOL stretchEntire( Gdiplus::Graphics & gDest, int xDest, int yDest, int nDestWidth, int nDestHeight ) const
    {
        return this->stretch( gDest, xDest, yDest, nDestWidth, nDestHeight, 0, 0, this->width(), this->height() );
    }
    BOOL output( Gdiplus::Graphics & gDest, int xDest, int yDest, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const;
    BOOL outputEntire( Gdiplus::Graphics & gDest, int xDest, int yDest ) const
    {
        return this->output( gDest, xDest, yDest, 0, 0, this->width(), this->height() );
    }
    
    BOOL stretch( MemImage & imgDest, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
    {
        if ( this != &imgDest )
        {
            Gdiplus::Graphics g(imgDest._pBmpImage);
            return this->stretch( g, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
        }
        return FALSE;
    }
    BOOL stretchEntire( MemImage & imgDest, int xDest, int yDest, int nDestWidth, int nDestHeight ) const
    {
        return this->stretch( imgDest, xDest, yDest, nDestWidth, nDestHeight, 0, 0, this->width(), this->height() );
    }
    BOOL output( MemImage & imgDest, int xDest, int yDest, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
    {
        if ( this != &imgDest )
        {
            Gdiplus::Graphics g(imgDest._pBmpImage);
            return this->output( g, xDest, yDest, xSrc, ySrc, nSrcWidth, nSrcHeight );
        }
        return FALSE;
    }
    BOOL outputEntire( MemImage & imgDest, int xDest, int yDest ) const
    {
        return this->output( imgDest, xDest, yDest, 0, 0, this->width(), this->height() );
    }

    /** \brief 以当前Bitmap内容创建一张新32bit的位图 */
    SimpleHandle<HBITMAP> obtainHBITMAP() const;
protected:
    void _zeroInit();

private:
    Gdiplus::Bitmap * _pBmpImage;
};

#endif

} // namespace winplus

std::ostream & operator << ( std::ostream & out, RECT const & rc );
std::ostream & operator << ( std::ostream & out, Gdiplus::Rect const & rc );
std::ostream & operator << ( std::ostream & out, Gdiplus::RectF const & rc );
std::ostream & operator << ( std::ostream & out, POINT const & pt );
std::ostream & operator << ( std::ostream & out, Gdiplus::Point const & pt );
std::ostream & operator << ( std::ostream & out, Gdiplus::PointF const & pt );
std::ostream & operator << ( std::ostream & out, SIZE const & si );
std::ostream & operator << ( std::ostream & out, Gdiplus::Size const & si );
std::ostream & operator << ( std::ostream & out, Gdiplus::SizeF const & si );


#endif // !defined(__WINPLUS_GRAPHICS_HPP__)
