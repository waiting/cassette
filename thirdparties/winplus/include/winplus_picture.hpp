//////////////////////////////////////////////////////////////////////////
// purpose: 图片相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_PICTURE_HPP__)
#define __WINPLUS_PICTURE_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

// 图片相关 -----------------------------------------------------------

/* 保存位图到指定文件 */
WINPLUS_FUNC_DECL(bool) Bitmap_SaveFile( HBITMAP hBitmap, String const & filename );

// IPicturePtr

/* 从文件加载图片(bmp,gif,jpg,png),获得IPicture接口 */
WINPLUS_FUNC_DECL(IPicturePtr) Picture_Load( String const & picFile );
/* IPicturePtr to HBITMAP */
inline HBITMAP Picture_GetBitmap( IPicturePtr pic )
{
    HBITMAP bitmap;
    pic->get_Handle( (OLE_HANDLE *)&bitmap );
    return bitmap;
}
/* 获得IPicture图片尺寸(pixels) */
WINPLUS_FUNC_DECL(SIZE) Picture_GetDimensions( IPicturePtr pic );

// 与mem_dc相关
class MemDC;
/* 从文件加载图片(bmp,gif,jpg,png),并获得mem_dc对象 */
WINPLUS_FUNC_DECL(bool) Picture_Load( String const & picFile, MemDC * memdc );

// 与mem_image相关
class MemImage;
/* 从文件加载图片(bmp,gif,jpg,png),并获得MemImage对象 */
WINPLUS_FUNC_DECL(bool) Picture_Load( String const & picFile, MemImage * memImg );


#define HIMETRIC_INCH   2540    // HIMETRIC units per inch

/* HIMETRIC转换为设备坐标(即像素) */
WINPLUS_FUNC_DECL(void) HIMETRIC_to_DP( HDC dc, LPPOINT point );
WINPLUS_FUNC_DECL(void) HIMETRIC_to_DP( HDC dc, LONG * x, LONG * y );
/* 设备坐标(即像素)转换为HIMETRIC */
WINPLUS_FUNC_DECL(void) DP_to_HIMETRIC( HDC dc, LPPOINT point );
WINPLUS_FUNC_DECL(void) DP_to_HIMETRIC( HDC dc, LONG * x, LONG * y );

/* 图标类 */
class Icon
{
public:
    Icon()
    {
        _construct();
    }
    Icon( String const & icoPath, uint cx = 0, uint cy = 0 )
    {
        _construct( icoPath.c_str(), LR_LOADFROMFILE | LR_SHARED, cx, cy );
    }
    Icon( uint icoResource, uint cx = 0, uint cy = 0 )
    {
        _construct( MAKEINTRESOURCE(icoResource), LR_SHARED, cx, cy );
    }
    Icon( LPCTSTR name, ulong flag, uint cx = 0, uint cy = 0 )
    {
        _construct( name, flag, cx, cy );
    }
    Icon( HMODULE module, LPCTSTR name, ulong flag, uint cx = 0, uint cy = 0 )
    {
        _construct( module, name, flag, cx, cy );
    }
    ~Icon()
    {
        _free();
    }
    void attach( HICON ico )
    {
        _free();
        _ico = ico;
    }
    HICON detach()
    {
        HICON ico = _ico;
        _ico = NULL;
        return ico;
    }
    operator HICON() const { return _ico; }
protected:
    void _free()
    {
        if ( _ico )
        {
            DestroyIcon(_ico);
            _ico = NULL;
        }
    }
    void _construct()
    {
        _ico = NULL;
    }
    void _construct( HMODULE module, LPCTSTR name, ulong flag, uint cx = 0, uint cy = 0 )
    {
         _ico = NULL;
         this->attach( (HICON)LoadImage( module, name, IMAGE_ICON, cx, cy, flag ) );
    }
    void _construct( LPCTSTR name, ulong flag, uint cx = 0, uint cy = 0 )
    {
        _construct( GetModuleHandle(NULL), name, flag, cx, cy );
    }

private:
    HICON _ico;
    Icon( Icon const & other );
    Icon & operator = ( Icon const & other );
};

// gdi+ 图片相关
#if defined(_GDIPLUS_H)

/* 根据MIME获取EncoderClsid */
WINPLUS_FUNC_DECL(INT) ImageEncoderFromMIME( String const & mime_type, CLSID * encoder_clsid );
/* 根据扩展名获取EncoderClsid */
WINPLUS_FUNC_DECL(INT) ImageEncoderFromExtName( String const & extname, CLSID * encoder_clsid );

#endif

} // namespace winplus

#endif // !defined(__WINPLUS_PICTURE_HPP__)
