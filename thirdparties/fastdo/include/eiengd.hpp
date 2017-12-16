#ifndef __EIENGD_HPP__
#define __EIENGD_HPP__

#include "winux.hpp"

/** \brief 图形库，提供简单的图片操作和绘图功能 */
namespace eiengd
{
#ifdef  EIENGD_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  EIENGD_DLL_EXPORTS
            #define EIENGD_DLL  __declspec(dllexport)
        #else
            #define EIENGD_DLL  __declspec(dllimport)
        #endif

        #define EIENGD_API __stdcall
    #else
        #define EIENGD_DLL
        #define EIENGD_API
    #endif
#else
    #define EIENGD_DLL
    #define EIENGD_API
#endif

#define EIENGD_FUNC_DECL(ret) EIENGD_DLL ret EIENGD_API
#define EIENGD_FUNC_IMPL(ret) ret EIENGD_API

/** \brief gd错误 */
class EIENGD_DLL GdError : public winux::Error
{
public:
    enum
    {
        gdeNone,
        gdeImageNotFound,
        gdeUnsupportedType
    };

    GdError( int errNo, winux::AnsiString const & err ) throw() : winux::Error( errNo, err ) { }
};

struct Point
{
    int x;
    int y;

    Point( int x = 0, int y = 0 ): x(x), y(y)
    {
    }
};

/** \brief GD初始化 */
class EIENGD_DLL GdInit
{
private:
    winux::MembersWrapper<struct GdInit_Data> _self;
public:
    GdInit();
    ~GdInit();
    DISABLE_OBJECT_COPY(GdInit)
};

class Image;

/** \brief 颜色类 */
class EIENGD_DLL Color
{
private:
    Image * _im;
    winux::MembersWrapper<struct Color_Data> _self;

public:
    Color( Image & im );

    Color( Image & im, int red, int green, int blue, winux::Mixed const & mixed = winux::Mixed() );

    ~Color();

    /** \brief 分配颜色,如果是调色板图则增加颜色数,如果是真彩图,则什么都不做。
     *  \param red int
     *  \param green int
     *  \param blue int
     *  \param mixed Mixed 若为true,则指定透明色;若为数字,则分配alpha色;默认是null分配普通色 */
    Color & alloc( int red, int green, int blue, winux::Mixed const & mixed = winux::Mixed() );

    Color & transparent( int red, int green, int blue )
    {
        return this->alloc( red, green, blue, true );
    }

    Color & alpha( int red, int green, int blue, int alpha = 0 )
    {
        return this->alloc( red, green, blue, alpha );
    }

    void dealloc();

    Color( Color const & other );

    Color & operator = ( Color const & other );

    friend class Graphics;
};

/** \brief 图片类 */
class EIENGD_DLL Image
{
private:
    winux::MembersWrapper<struct Image_Data> _self;

public:
    Image(); // 默认

    Image( int width, int height, bool isTrueColor = true ); // 指定宽高创建

    Image( winux::String const & filename ); // 从图片文件创建

    ~Image();

    void create( int width, int height ); // 创建指定大小的256色调色板图片

    void createTrueColor( int width, int height ); // 创建指定大小的真彩图

    void createFromFile( winux::String const & filename );

    /** \brief 输出图片
     *
     * 类型判断，1.参数type指定 2.filename扩展名 3.内部_type */
    bool output( winux::String const & filename, winux::String const & type = "" );

    bool output( winux::Buffer * buf, winux::String const & type = "" );

    void destroy();

    void copy( Image & srcImg, int srcX, int srcY, int srcWidth, int srcHeight, int x, int y );

    void copyResized( Image & srcImg, int srcX, int srcY, int srcWidth, int srcHeight, int x, int y, int width, int height );

    void copyResampled( Image & srcImg, int srcX, int srcY, int srcWidth, int srcHeight, int x, int y, int width, int height );

    void debug();

    /** \brief 分配一个颜色 */
    Color color( int red, int green, int blue, winux::Mixed const & mixed = winux::Mixed() );

    // attributes
    int getWidth() const;

    int getHeight() const;

    winux::String getType() const;

    /** \brief 设置标记以在保存PNG图像时保存完整的alpha通道信息（与单一透明色相反）。
     *
     *  要使用本函数，必须将alphablending清位（Image::alphaBlending(false)）。 */
    void saveAlpha( bool isSave );

    /** \brief 设定图像的混色模式 */
    void alphaBlending( bool isBlend );

    /** \brief 相当于getpixel() */
    int colorAt( int x, int y ) const;

    /** \brief 获取指定颜色索引的颜色，调色板图用 */
    int colorsForIndex( int index ) const;

    /** \brief 获取颜色总数，调色板图用 */
    int colorsTotal() const;

    DISABLE_OBJECT_COPY(Image)

    friend class Color;
    friend class Graphics;
};

/** \brief 绘画类 */
class EIENGD_DLL Graphics
{
private:
    Image * _im;
    winux::MembersWrapper<struct Graphics_Data> _self;

public:
    Graphics( Image & im );

    ~Graphics();

    /** \brief 在指定坐标用指定颜色填充
     *  \param x int
     *  \param y int
     *  \param color Color */
    void fill( int x, int y, Color const & color );

    void filledRectangle( int x, int y, int width, int height, Color const & color );

    void arc( int centerX, int centerY, int width, int height, int start, int end, Color const & color );

    void line( int x1, int y1, int x2, int y2, Color const & color );

    void rectangle( int x, int y, int width, int height, Color const & color );

    void ellipse( int centerX, int centerY, int width, int height, Color const & color );

    void filledEllipse( int centerX, int centerY, int width, int height, Color const & color );

    /** \brief TrueTypeFont文本输出
     *  \param size 字体大小
     *  \param angle 角度
     *  \param x X
     *  \param y Y
     *  \param color 颜色
     *  \param fontFile TrueType字体文件
     *  \param text 待画的UTF8编码的文本
     *  \param boxOut[4] 返回一个四边形框表示绘画的范围 */
    void ttfTextOut(
        double size,
        double angle,
        int x,
        int y,
        Color const & color,
        winux::String const & fontFile,
        winux::String const & text,
        Point * boxOut = NULL
    );

    /** \brief 取得文本输出的四边形范围
     *  \param boxOut[4] 依次表示：左下角，右下角，右上角，左上角 */
    void ttfBBox( double size, double angle, winux::String const & fontFile, winux::String const & text, Point boxOut[4] ) const;

    /** \brief 输出英文字符串,fontInx={0~4} */
    void stringOut( int fontInx, int x, int y, winux::String const & str, Color const & color );

    /** \brief 分配一个颜色 */
    Color color( int red, int green, int blue, winux::Mixed const & mixed = winux::Mixed() );

    DISABLE_OBJECT_COPY(Graphics)
};

}

#endif //__EIENGD_HPP__
