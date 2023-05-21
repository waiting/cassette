//////////////////////////////////////////////////////////////////////////
// purpose:  窗口控件相关
// author:   WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_WINCTRL_HPP__)
#define __WINPLUS_WINCTRL_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <commctrl.h>

namespace winplus
{
// UI相关 ----------------------------------------------------------------

/** \brief 窗口定时器 */
class WINPLUS_DLL WindowTimer
{
public:
    WindowTimer( void );
    void create( HWND window, UINT elapse, UINT_PTR id = -1 );
    void destroy( void );
    UINT_PTR getId( void ) const { return _id; }
    ~WindowTimer( void );
private:
    HWND _window;
    UINT_PTR _id;
    static UINT_PTR _idAutoIncrement;

    DISABLE_OBJECT_COPY(WindowTimer)
};

// 窗口相关 --------------------------------------------------------------

/** \brief 弹出一个信息框
 *
 *  \param msg 消息内容
 *  \param title 消息框标题
 *  \param hParentWnd 父窗口。如果是NULL则使用当前前台窗口。
 *  \param uType 额外的旗标、如果有MB_TASKMODAL则hParentWnd仅仅用作布局，传递给MessageBox的是(NULL,...)
 *  \return 返回选择按钮的ID */
WINPLUS_FUNC_DECL(int) MsgBox( String const & msg, String const & title = TEXT("WinPlus"), HWND hwndParent = NULL, UINT uType = 0 );

/** \brief 弹出一个错误框
 *
 *  \param msg 消息内容
 *  \param title 消息框标题
 *  \param hParentWnd 父窗口。如果是NULL则使用当前前台窗口。
 *  \param uType 额外的旗标、如果有MB_TASKMODAL则hParentWnd仅仅用作布局，传递给MessageBox的是(NULL,...)
 *  \return 返回选择按钮的ID */
WINPLUS_FUNC_DECL(int) ErrBox( String const & msg, String const & title = TEXT("WinPlus Error"), HWND hwndParent = NULL, UINT uType = 0 );

/** \brief 弹出一个警告框
 *
 *  \param msg 消息内容
 *  \param title 消息框标题
 *  \param hParentWnd 父窗口。如果是NULL则使用当前前台窗口。
 *  \param uType 额外的旗标、如果有MB_TASKMODAL则hParentWnd仅仅用作布局，传递给MessageBox的是(NULL,...)
 *  \return 返回选择按钮的ID */
WINPLUS_FUNC_DECL(int) WarnBox( String const & msg, String const & title = TEXT("WinPlus Warning"), HWND hwndParent = NULL, UINT uType = 0 );

/** \brief 客户坐标到屏幕坐标（RECT） */
WINPLUS_FUNC_DECL(BOOL) ClientToScreen( HWND hWnd, LPRECT lpRect );

/** \brief 屏幕坐标到客户坐标（RECT） */
WINPLUS_FUNC_DECL(BOOL) ScreenToClient( HWND hWnd, LPRECT lpRect );

/** \brief 获取客户区矩形 */
WINPLUS_FUNC_DECL(RECT) Window_GetClient( HWND hWnd );
/** \brief 获取窗口矩形，父窗口客户区坐标 */
WINPLUS_FUNC_DECL(RECT) Window_GetRect( HWND hWnd );
/** \brief 设置窗口矩形，父窗口客户区坐标 */
WINPLUS_FUNC_DECL(void) Window_SetRect( HWND hWnd, LPCRECT lpRect, bool isRedraw = true, bool isActivate = true );
/** \brief 膨胀窗口 */
WINPLUS_FUNC_DECL(void) Window_Inflate( HWND hWnd, int dx, int dy );
/** \brief 以相对窗口为准居中窗口 */
WINPLUS_FUNC_DECL(void) Window_Center( HWND hWnd, HWND hwndRelative, bool isInRelative = false, bool isRedraw = true, bool isActivate = true );
/** \brief 计算窗口居中是否显示完全 */
WINPLUS_FUNC_DECL(bool) Window_CalcCenter( HWND hWnd, HWND hwndRelative, bool isInRelative, RECT* rectWnd, RECT* rectRelative, POINT* ptOffset );
/** \brief 获取窗口文本 */
WINPLUS_FUNC_DECL(String) Window_GetText( HWND hWnd );
/** \brief 设置窗口文本 */
WINPLUS_FUNC_DECL(void) Window_SetText( HWND hWnd, String const & text );
/** \brief 获得窗口是否显示 */
WINPLUS_FUNC_DECL(bool) Window_IsShow( HWND hWnd );
/** \brief 让窗口可见 */
WINPLUS_FUNC_DECL(void) Window_Show( HWND hWnd, bool wantShow = true );
/** \brief 判断是否是顶层窗口 */
WINPLUS_FUNC_DECL(bool) Window_IsTopLevel( HWND hWnd );

class WinWrap
{
public:
    WinWrap( HWND hwnd ) : _hwnd(hwnd) { }

    BOOL clientToScreen( LPRECT lpRect ) { return ClientToScreen( _hwnd, lpRect ); }
    BOOL screenToClient( LPRECT lpRect ) { return ScreenToClient( _hwnd, lpRect ); }

    RECT getClient() { return Window_GetClient(_hwnd); }
    RECT getRect() { return Window_GetRect(_hwnd); }
    void setRect( LPCRECT lpRect, bool isRedraw = true, bool isActivate = true ) { return Window_SetRect( _hwnd, lpRect, isRedraw, isActivate ); }
    void inflate( int dx, int dy ) { return Window_Inflate( _hwnd, dx, dy ); }
    void center( HWND hwndRelative, bool isInRelative = false, bool isRedraw = true, bool isActivate = true ) { return Window_Center( _hwnd, hwndRelative, isInRelative, isRedraw, isActivate ); }
    String getText() { return Window_GetText(_hwnd); }
    void setText( String const & text ) { return Window_SetText( _hwnd, text ); }
    bool isShow() { return Window_IsShow(_hwnd); }
    void show( bool wantShow = true ) { return Window_Show( _hwnd, wantShow ); }
    bool isTopLevel() { return Window_IsTopLevel(_hwnd); }
private:
    HWND _hwnd;
};

// ListCtrl - Report操作 -------------------------------------------------
WINPLUS_FUNC_DECL(INT) Report_AddStrings( HWND report, StringArray const & cols );
WINPLUS_FUNC_DECL(INT) Report_InsertStrings( HWND report, INT item_index, StringArray const & cols );
WINPLUS_FUNC_DECL(void) Report_SetStrings( HWND report, INT item_index, StringArray const & cols );
WINPLUS_FUNC_DECL(INT) Report_GetStrings( HWND report, INT item_index, StringArray * cols );

// ImageList control相关 -------------------------------------------------
#ifdef _GDIPLUS_H
/** \brief 添加一张含Alpha通道的图片进ImageList via Gdiplus::Bitmap。ImageList必须创建为ILC_COLOR32 */
WINPLUS_FUNC_DECL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, Gdiplus::Bitmap const & image );
/** \brief 添加一张含Alpha通道的图片进ImageList via resource。ImageList必须创建为ILC_COLOR32 */
WINPLUS_FUNC_DECL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, UINT nId, LPCTSTR lpType );
/** \brief 添加一张含Alpha通道的图片进ImageList via image file。ImageList必须创建为ILC_COLOR32 */
WINPLUS_FUNC_DECL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, String const & imagePath );
#endif
/* 添加一张含Alpha通道的32位DIB进ImageList via LoadImage(),ImageList必须创建为ILC_COLOR32 */
WINPLUS_FUNC_DECL(int) ImageList_Add32bpp( HIMAGELIST hImageList, UINT nId, UINT uType );
WINPLUS_FUNC_DECL(int) ImageList_Add32bpp( HIMAGELIST hImageList, String const & imagePath, UINT uType );

// HotkeyCtrl相关 --------------------------------------------------------

/* HOTKEYF标识转为MOD标识
   便于从Hotkey控件输入的按键信息用RegisterHotkey()注册 */
WINPLUS_FUNC_DECL(UINT) HOTKEYF_to_MOD( UINT hkf );

/* MOD标识转为HOTKEYF标识
   与HOTKEYF_to_MOD()相反 */
WINPLUS_FUNC_DECL(UINT) MOD_to_HOTKEYF( UINT mod );


// 通用对话框相关 ---------------------------------------------------------------------------

/** \brief 文件夹选择对话框，基于SHBrowseForFolder() */
class WINPLUS_DLL FolderDialog
{
public:
    FolderDialog( HWND hWndOwner, LPCTSTR lpszWindowTitle = NULL, LPCTSTR lpszPromptText = NULL, BOOL bIncludedFiles = FALSE );
    // 显示对话框，成功获取路径返回TRUE，否则返回FALSE
    BOOL doModal( LPCTSTR lpszRootDir = NULL, LPCTSTR lpszSelectionDir = NULL );
    // 选择的文件夹显示名（显示名并不一定就是文件夹名：例如"我的文档" => "My Documents"）
    String getDisplayName() const;
    // 选择的路径
    String getPath() const;

private:
    BROWSEINFO _bi;
    String _strPath;
    String _strDisplay;
    LPCTSTR _lpszWindowTitle;
    LPCTSTR _lpszSelectionDir;
    // 回调函数
    static int CALLBACK BrowseCallback( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData );

    DISABLE_OBJECT_COPY(FolderDialog)
};

/** \brief 文件选择对话框 */
class WINPLUS_DLL FileDialog
{
public:
    FileDialog( HWND hwndOwner, BOOL bOpenDialog = TRUE, LPCTSTR lpszWindowTitle = NULL, LPCTSTR lpszDefExt = NULL, BOOL bMultiSelected = FALSE, DWORD dwAdditionalFlags = 0, DWORD dwRemoveFlags = 0 );
    BOOL doModal( LPCTSTR lpszInitialDir = NULL, LPCTSTR lpFilter = NULL );
    // 选择的文件路径
    String getFilePath() const;
    // 获取目录路径
    String getDirPath() const;
    // 当多选时通过此获得选择的文件
    int getFilePaths( StringArray * pArr ) const;

private:
    String _buffer;
    OPENFILENAME _ofn;
    BOOL _isOpen;

    DISABLE_OBJECT_COPY(FileDialog)
};

/** \brief 系统托盘图标通知类，包括气球 */
class WINPLUS_DLL Notification
{
private:
    NOTIFYICONDATA _nid;
    UINT _uVersion; // 0, 3, 4 版本，影响通知消息的参数表示的是什么
    bool _isAdd;
protected:
    void _construct();

public:
    Notification( UINT uVersion = 0 );
    ~Notification();
    bool add( HWND hwnd, UINT notificationId, Icon const & ico, String const & strTip );

    bool modify();

    void setMessage( UINT msg );

    void setBalloonInfo( String const & infoTitle, String const & infoText, DWORD infoFlags = NIIF_USER | NIIF_RESPECT_QUIET_TIME );

    void setIconInfo( Icon const & ico, String const & strTip );

    bool del();
};


} // namespace winplus

#endif // !defined(__WINPLUS_WINCTRL_HPP__)
