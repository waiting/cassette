#include "winplus_definitions.hpp"
#include "winplus_picture.hpp"
#include "winplus_winctrl.hpp"
#include "winplus_shell.hpp"
#include "winplus_system.hpp"
#include "winplus_compoundfile.hpp"
#include "strings.hpp"

#pragma comment( lib, "comctl32.lib" )

namespace winplus
{

UINT_PTR WindowTimer::_idAutoIncrement = 0;
WindowTimer::WindowTimer( void ) : _window(NULL), _id(0)
{
}

WindowTimer::~WindowTimer( void )
{
    destroy();
}

void WindowTimer::create( HWND window, UINT elapse, UINT_PTR id )
{
    destroy();
    _window = window;
    if ( _window )
    {
        if ( id == -1 )
        {
            _id = ++_idAutoIncrement;
        }
        else
        {
            _id = id;
            if ( _idAutoIncrement < id )
                _idAutoIncrement = id;
        }
        SetTimer( _window, _id, elapse, NULL );
    }
}

void WindowTimer::destroy( void )
{
    if ( _window )
    {
        KillTimer( _window, _id );
        _window = NULL;
    }
}

// 窗口相关 ---------------------------------------------------------------
inline static LRESULT CALLBACK __MessageBoxCbtHookProc( HWND hwndParent, int nCode, WPARAM wParam, LPARAM lParam )
{
    if ( nCode < 0 ) return CallNextHookEx( nullptr, nCode, wParam, lParam );

    switch ( nCode )
    {
    case HCBT_CREATEWND:
        {
            HWND hWnd = (HWND)wParam;
            if ( !GetParent(hWnd) )
            {
                RECT rc;
                GetWindowRect( hwndParent, &rc );
                CBT_CREATEWND & cw = *(CBT_CREATEWND*)lParam;
                cw.lpcs->x = rc.left + ( ( rc.right - rc.left ) - cw.lpcs->cx ) / 2;
                cw.lpcs->y = rc.top + ( ( rc.bottom - rc.top ) - cw.lpcs->cy ) / 2;
            }
        }
        break;
    }

    return 0;
}

thread_local static HWND __tl_hMsgBoxParent;
static LRESULT CALLBACK __MsgBoxCbtProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    return __MessageBoxCbtHookProc( __tl_hMsgBoxParent, nCode, wParam, lParam );
}

thread_local static HWND __tl_hErrBoxParent;
static LRESULT CALLBACK __ErrBoxCbtProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    return __MessageBoxCbtHookProc( __tl_hErrBoxParent, nCode, wParam, lParam );
}

thread_local static HWND __tl_hWarnBoxParent;
static LRESULT CALLBACK __WarnBoxCbtProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    return __MessageBoxCbtHookProc( __tl_hWarnBoxParent, nCode, wParam, lParam );
}

WINPLUS_FUNC_IMPL(int) MsgBox( String const & msg, String const & title, HWND hwndParent, UINT uType )
{
    if ( !hwndParent ) hwndParent = GetForegroundWindow();
    __tl_hMsgBoxParent = hwndParent;
    HHOOK hhk = SetWindowsHookEx( WH_CBT, __MsgBoxCbtProc, NULL, GetCurrentThreadId() );
    int nRet = MessageBox( ( uType & MB_TASKMODAL ) ? NULL : hwndParent, msg.c_str(), title.c_str(), MB_ICONINFORMATION | uType );
    UnhookWindowsHookEx(hhk);
    return nRet;
}

WINPLUS_FUNC_IMPL(int) ErrBox( String const & msg, String const & title, HWND hwndParent, UINT uType )
{
    if ( !hwndParent ) hwndParent = GetForegroundWindow();
    __tl_hErrBoxParent = hwndParent;
    HHOOK hhk = SetWindowsHookEx( WH_CBT, __ErrBoxCbtProc, NULL, GetCurrentThreadId() );
    int nRet = MessageBox( ( uType & MB_TASKMODAL ) ? NULL : hwndParent, msg.c_str(), title.c_str(), MB_ICONERROR | uType );
    UnhookWindowsHookEx(hhk);
    return nRet;
}

WINPLUS_FUNC_IMPL(int) WarnBox( String const & msg, String const & title, HWND hwndParent, UINT uType )
{
    if ( !hwndParent ) hwndParent = GetForegroundWindow();
    __tl_hWarnBoxParent = hwndParent;
    HHOOK hhk = SetWindowsHookEx( WH_CBT, __WarnBoxCbtProc, NULL, GetCurrentThreadId() );
    int nRet = MessageBox( ( uType & MB_TASKMODAL ) ? NULL : hwndParent, msg.c_str(), title.c_str(), MB_ICONEXCLAMATION | uType );
    UnhookWindowsHookEx(hhk);
    return nRet;
}

WINPLUS_FUNC_IMPL(BOOL) ClientToScreen( HWND hWnd, LPRECT lpRect )
{
    LONG dx = lpRect->left, dy = lpRect->top;
    BOOL b = ::ClientToScreen( hWnd, (LPPOINT)lpRect );
    dx = lpRect->left - dx, dy = lpRect->top - dy;
    lpRect->right += dx;
    lpRect->bottom += dy;
    return b;
}

WINPLUS_FUNC_IMPL(BOOL) ScreenToClient( HWND hWnd, LPRECT lpRect )
{
    LONG dx = lpRect->left, dy = lpRect->top;
    BOOL b = ::ScreenToClient( hWnd, (LPPOINT)lpRect );
    dx = lpRect->left - dx, dy = lpRect->top - dy;
    lpRect->right += dx;
    lpRect->bottom += dy;
    return b;
}

WINPLUS_FUNC_IMPL(RECT) Window_GetClient( HWND hWnd )
{
    RECT rc;
    GetClientRect( hWnd, &rc );
    return rc;
}

WINPLUS_FUNC_IMPL(RECT) Window_GetRect( HWND hWnd )
{
    RECT rc;
    GetWindowRect( hWnd, &rc );
    HWND hwndParent = GetParent(hWnd);
    if ( IsWindow(hwndParent) ) // 判断父窗口是否存在，把屏幕坐标换为相对父窗口客户区坐标
    {
        ScreenToClient( hwndParent, &rc );
    }
    return rc;
}

WINPLUS_FUNC_IMPL(void) Window_SetRect( HWND hWnd, LPCRECT lpRect, bool isRedraw, bool isActivate )
{
    SetWindowPos( hWnd, NULL, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, SWP_NOZORDER | ( isRedraw ? 0 : SWP_NOREDRAW ) | ( isActivate ? 0 : SWP_NOACTIVATE ) );
}

WINPLUS_FUNC_IMPL(void) Window_Inflate( HWND hWnd, int dx, int dy )
{
    RECT rc = Window_GetRect(hWnd);
    InflateRect( &rc, dx, dy );
    Window_SetRect( hWnd, &rc );
}

WINPLUS_FUNC_IMPL(void) Window_Center( HWND hWnd, HWND hwndRelative, bool isInRelative, bool isRedraw, bool isActivate )
{
    INT cx, cy, offsetLeft, offsetTop;
    if ( hwndRelative && IsWindow(hwndRelative) )
    {
        if ( isInRelative )
        {
            RECT rcWnd;
            GetClientRect( hwndRelative, &rcWnd );
            cx = rcWnd.right - rcWnd.left;
            cy = rcWnd.bottom - rcWnd.top;
            offsetLeft = 0;//rcWnd.left;
            offsetTop = 0;//rcWnd.top;
        }
        else
        {
            RECT rcWnd;
            GetWindowRect( hwndRelative, &rcWnd );
            cx = rcWnd.right - rcWnd.left;
            cy = rcWnd.bottom - rcWnd.top;
            offsetLeft = rcWnd.left;
            offsetTop = rcWnd.top;
        }
    }
    else // use screen coord
    {
        cx = GetSystemMetrics(SM_CXSCREEN);
        cy = GetSystemMetrics(SM_CYSCREEN);
        offsetLeft = 0;
        offsetTop = 0;
    }
    RECT rc;
    GetWindowRect( hWnd, &rc );
    LONG nWidth = rc.right - rc.left;
    LONG nHeight = rc.bottom - rc.top;
    LONG left, top;
    left = offsetLeft + ( cx - nWidth ) / 2;
    top = offsetTop + ( cy - nHeight ) / 2;
    SetWindowPos( hWnd, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | ( isRedraw ? 0 : SWP_NOREDRAW ) | ( isActivate ? 0 : SWP_NOACTIVATE ) );
}

WINPLUS_FUNC_IMPL(bool) Window_CalcCenter( HWND hWnd, HWND hwndRelative, bool isInRelative, RECT* rectWnd, RECT* rectRelative, POINT* ptOffset )
{
    INT cx, cy;
    if ( hwndRelative && IsWindow(hwndRelative) )
    {
        if ( isInRelative )
        {
            GetClientRect( hwndRelative, rectRelative );
            cx = rectRelative->right - rectRelative->left;
            cy = rectRelative->bottom - rectRelative->top;
            ptOffset->x = 0;
            ptOffset->y = 0;
        }
        else
        {
            GetClientRect( hwndRelative, rectRelative );
            cx = rectRelative->right - rectRelative->left;
            cy = rectRelative->bottom - rectRelative->top;
            ptOffset->x = rectRelative->left;
            ptOffset->y = rectRelative->top;
        }
    }
    else // use screen coord
    {
        cx = GetSystemMetrics(SM_CXSCREEN);
        cy = GetSystemMetrics(SM_CYSCREEN);
        ptOffset->x = 0;
        ptOffset->y = 0;
    }

    GetWindowRect( hWnd, rectWnd );
    LONG nWidth = rectWnd->right - rectWnd->left;
    LONG nHeight = rectWnd->bottom - rectWnd->top;
    LONG left, top;
    left = ptOffset->x + ( cx - nWidth ) / 2;
    top = ptOffset->y + ( cy - nHeight ) / 2;

    return left > 0 && top > 0;
}

WINPLUS_FUNC_IMPL(String) Window_GetText( HWND hWnd )
{
    String res;
    INT len = GetWindowTextLength(hWnd);
    res.resize( len + 1 );
    int n = GetWindowText( hWnd, &res[0], len + 1 );
    return String( res.c_str(), n );
}

WINPLUS_FUNC_IMPL(void) Window_SetText( HWND hWnd, String const & text )
{
    SetWindowText( hWnd, text.c_str() );
}

WINPLUS_FUNC_IMPL(bool) Window_IsShow( HWND hWnd )
{
    return ( GetWindowLong( hWnd, GWL_STYLE ) & WS_VISIBLE ) != 0;
}

WINPLUS_FUNC_IMPL(void) Window_Show( HWND hWnd, bool wantShow )
{
    LONG style = GetWindowLong( hWnd, GWL_STYLE );
    if ( wantShow )
    {
        //SetWindowLong( window, GWL_STYLE, style | WS_VISIBLE );
        ShowWindow( hWnd, SW_NORMAL );
    }
    else
    {
        //SetWindowLong( window, GWL_STYLE, style & ~WS_VISIBLE );
        ShowWindow( hWnd, SW_HIDE );
    }
}

WINPLUS_FUNC_IMPL(bool) Window_IsTopLevel( HWND hWnd )
{
    LONG exStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
    return ( exStyle & WS_EX_TOPMOST ) || ( !( exStyle & WS_EX_APPWINDOW ) && ( GetWindow( hWnd, GW_OWNER ) == NULL ) );
}

// ListCtrl - Report操作---------------------------------------------------
WINPLUS_FUNC_IMPL(INT) Report_AddStrings( HWND report, StringArray const & cols )
{
    INT itemsCount = ListView_GetItemCount(report);
    INT realColsCount = Header_GetItemCount( ListView_GetHeader(report) );
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = itemsCount;
    item.iSubItem = 0;
    item.pszText = (LPTSTR)NULL;
    item.state = 0;
    item.stateMask = 0;
    item.iImage = 0;
    item.lParam = 0;

    INT index = ListView_InsertItem( report, &item );
    INT i, colsCount = (INT)cols.size();

    for ( i = 0; i < colsCount && i < realColsCount; i++ )
    {
        ListView_SetItemText( report, itemsCount, i, (LPTSTR)cols[i].c_str() );
    }

    return index;
}

WINPLUS_FUNC_IMPL(INT) Report_InsertStrings( HWND report, INT item_index, StringArray const & cols )
{
    INT realColsCount = Header_GetItemCount( ListView_GetHeader(report) );
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = item_index;
    item.iSubItem = 0;
    item.pszText = (LPTSTR)NULL;
    item.state = 0;
    item.stateMask = 0;
    item.iImage = 0;
    item.lParam = 0;

    INT index = ListView_InsertItem( report, &item );
    INT i, colsCount = (INT)cols.size();
    
    for ( i = 0; i < colsCount && i < realColsCount; i++ )
    {
        ListView_SetItemText( report, item_index, i, (LPTSTR)cols[i].c_str() );
    }
    
    return index;
}

WINPLUS_FUNC_IMPL(void) Report_SetStrings( HWND report, INT item_index, StringArray const & cols )
{
    INT realColsCount = Header_GetItemCount( ListView_GetHeader(report) );
    INT i, colsCount = (INT)cols.size();

    for ( i = 0; i < colsCount && i < realColsCount; i++ )
    {
        ListView_SetItemText( report, item_index, i, (LPTSTR)cols[i].c_str() );
    }
}

WINPLUS_FUNC_IMPL(INT) Report_GetStrings( HWND report, INT item_index, StringArray * cols )
{
    INT real_cols_count = Header_GetItemCount( ListView_GetHeader(report) );
    INT i;
    for ( i = 0; i < real_cols_count; ++i )
    {
        LVITEM item;
        item.iItem = item_index;
        item.iSubItem = i;

        String text;
        INT len = 511;
        text.resize(len);
        item.cchTextMax = len + 1;
        item.pszText = &text[0];

        len = (INT)SendMessage( report, LVM_GETITEMTEXT, item_index, (LPARAM)&item );

        cols->push_back(item.pszText);
    }
    return i;
}

// ImageList control相关 -------------------------------------------------
#ifdef _GDIPLUS_H
WINPLUS_FUNC_IMPL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, Gdiplus::Bitmap const & image )
{
    using namespace Gdiplus;
    Bitmap & bm = const_cast<Bitmap &>(image);
    // 转成32位bpp
    HBITMAP hBitmap;
    void * pvBits;
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = bm.GetWidth();
    bmi.bmiHeader.biHeight = -(LONG)bm.GetHeight();
    //bm.RotateFlip( RotateNoneFlipY );
    hBitmap = CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0 );
    BitmapData bitmapData;
    bm.LockBits( &Rect( 0, 0, bm.GetWidth(), bm.GetHeight() ), ImageLockModeRead, PixelFormat32bppARGB, &bitmapData );
    RtlMoveMemory( pvBits, bitmapData.Scan0, bm.GetHeight() * bitmapData.Stride );
    bm.UnlockBits(&bitmapData);
    int i;
    i = ImageList_Add( hImageList, hBitmap, NULL );
    DeleteObject(hBitmap);
    return i;
}

WINPLUS_FUNC_IMPL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, UINT nId, LPCTSTR lpType )
{
    IStreamPtr pImageStream = CreateStreamFromResource( nId, lpType );
    return ImageList_AddAlphaImage( hImageList, Gdiplus::Bitmap(pImageStream) );
}

WINPLUS_FUNC_IMPL(int) ImageList_AddAlphaImage( HIMAGELIST hImageList, String const & imagePath )
{
    return ImageList_AddAlphaImage( hImageList, Gdiplus::Bitmap( StringToUnicode(imagePath).c_str() ) );
}
#endif

WINPLUS_FUNC_IMPL(int) ImageList_Add32bpp( HIMAGELIST hImageList, UINT nId, UINT uType )
{
    HBITMAP hBitmap = (HBITMAP)LoadImage( GetModuleHandle(NULL), MAKEINTRESOURCE(nId), uType, 0, 0, LR_CREATEDIBSECTION );
    int i = ImageList_Add( hImageList, hBitmap, NULL );
    DeleteObject(hBitmap);
    return i;
}

WINPLUS_FUNC_IMPL(int) ImageList_Add32bpp( HIMAGELIST hImageList, String const & imagePath, UINT uType )
{
    HBITMAP hBitmap = (HBITMAP)LoadImage( GetModuleHandle(NULL), imagePath.c_str(), uType, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
    int i = ImageList_Add( hImageList, hBitmap, NULL );
    DeleteObject(hBitmap);
    return i;
}

// HotkeyCtrl相关 --------------------------------------------------------

WINPLUS_FUNC_IMPL(UINT) HOTKEYF_to_MOD( UINT hkf )
{
    UINT mod = 0;
    UINT hkfs[3] = {HOTKEYF_SHIFT, HOTKEYF_CONTROL, HOTKEYF_ALT};
    UINT mods[3] = {MOD_SHIFT, MOD_CONTROL, MOD_ALT};
    int i;
    for (i = 0; i < 3; i++)
        (hkf & hkfs[i]) && (mod |= mods[i]);
    return mod;
}

WINPLUS_FUNC_IMPL(UINT) MOD_to_HOTKEYF( UINT mod )
{
    UINT hkf = 0;
    UINT hkfs[3] = {HOTKEYF_SHIFT, HOTKEYF_CONTROL, HOTKEYF_ALT};
    UINT mods[3] = {MOD_SHIFT, MOD_CONTROL, MOD_ALT};
    int i;
    for (i = 0; i < 3; i++)
        (mod & mods[i]) && (hkf |= hkfs[i]);
    return hkf;
}

// 通用对话框相关 --------------------------------------------------------

FolderDialog::FolderDialog( HWND hWndOwner, LPCTSTR lpszWindowTitle, LPCTSTR lpszPromptText, BOOL bIncludedFiles )
{
    ZeroMemory( &_bi, sizeof(_bi) );
    _szPath.resize(MAX_PATH);
    _szDisplay.resize(MAX_PATH);
    
    _bi.pszDisplayName = &_szDisplay[0];
    _bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_STATUSTEXT | BIF_UAHINT | BIF_NEWDIALOGSTYLE;
    if ( bIncludedFiles )
    {
        _bi.ulFlags |= BIF_BROWSEINCLUDEFILES;
    }

    _bi.hwndOwner = hWndOwner;
    if ( lpszPromptText != NULL )
        _bi.lpszTitle = lpszPromptText;
    else
        _bi.lpszTitle = TEXT("选择文件夹");

    _lpszSelectionDir = NULL;
    _lpszWindowTitle = lpszWindowTitle;

    _bi.lpfn = BrowseCallback;
    _bi.lParam = (LPARAM)this;
}

BOOL FolderDialog::doModal( LPCTSTR lpszRootDir, LPCTSTR lpszSelectionDir )
{
    LPMALLOC pMalloc;
    LPITEMIDLIST pItemIDList;
    HRESULT hr;
    BOOL bRet = FALSE;

    if ( lpszRootDir != NULL && *lpszRootDir )
        _bi.pidlRoot = ItemIdListFromPath(lpszRootDir);
    else
        _bi.pidlRoot = NULL;
    
    _lpszSelectionDir = lpszSelectionDir;

    pItemIDList = SHBrowseForFolder(&_bi);

    hr = SHGetMalloc(&pMalloc);
    
    if ( pItemIDList != NULL )
    {
        SHGetPathFromIDList( pItemIDList, &_szPath[0] );

        if ( pMalloc != NULL )
            pMalloc->Free(pItemIDList);

        bRet = TRUE;
    }
    
    if ( pMalloc != NULL && _bi.pidlRoot != NULL )
    {
        pMalloc->Free( (void *)_bi.pidlRoot );
        _bi.pidlRoot = NULL;
    }
    
    if ( pMalloc != NULL )
        pMalloc->Release();
    
    return bRet;
}

String FolderDialog::getPath() const
{
    return _szPath.c_str();
}

String FolderDialog::getDisplayName() const
{
    return _szDisplay.c_str();
}

int CALLBACK FolderDialog::BrowseCallback( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
    switch ( uMsg )
    {
    case BFFM_INITIALIZED:
        {
            //SendMessage( hwnd, BFFM_SETOKTEXT, 0, (LPARAM)L"Helloworld" );
            //SendMessage( hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)L"Helloworld" );
            FolderDialog * pDlg = reinterpret_cast<FolderDialog *>(lpData);
            // 设置对话框标题
            if ( pDlg->_lpszWindowTitle )
            {
                SetWindowText( hwnd, pDlg->_lpszWindowTitle );
            }
            // 设置被选择目录
            if ( pDlg->_lpszSelectionDir )
            {
                SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pDlg->_lpszSelectionDir );
            }
            SendMessage( hwnd, WM_SETICON, ICON_BIG, SendMessage( pDlg->_bi.hwndOwner, WM_GETICON, ICON_BIG, 0 ) );
        }
        break;
    case BFFM_SELCHANGED:
        break;
    case BFFM_VALIDATEFAILED:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////

FileDialog::FileDialog( HWND hwndOwner, BOOL bOpenDialog, LPCTSTR lpszWindowTitle, LPCTSTR lpszDefExt, BOOL bMultiSelected, DWORD dwAdditionalFlags, DWORD dwRemoveFlags )
{
    ZeroMemory( &_ofn, sizeof(_ofn) );
    _ofn.lStructSize = sizeof(_ofn);
    _ofn.hwndOwner = hwndOwner;
    _ofn.lpstrTitle = lpszWindowTitle;
    _ofn.lpstrDefExt = lpszDefExt;
    _isOpen = bOpenDialog;
    _ofn.Flags = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST | ( bOpenDialog ? OFN_FILEMUSTEXIST : OFN_OVERWRITEPROMPT );
    if ( bOpenDialog && bMultiSelected )
    {
        _ofn.Flags |= OFN_ALLOWMULTISELECT;
    }
    _ofn.Flags |= dwAdditionalFlags;
    _ofn.Flags &= ~dwRemoveFlags;
}

BOOL FileDialog::doModal( LPCTSTR lpszInitialDir, LPCTSTR lpFilter )
{
    _ofn.lpstrInitialDir = lpszInitialDir;
    _ofn.lpstrFilter = lpFilter;
    if ( _ofn.Flags & OFN_ALLOWMULTISELECT )
    {
        // 可多选的情况下,需要更多的空间来存储路径
        _buffer.resize(0x2000);
    }
    else
    {
        _buffer.resize(MAX_PATH);
    }

    _ofn.lpstrFile = &_buffer[0];
    _ofn.nMaxFile = (DWORD)_buffer.size();

    return _isOpen ? GetOpenFileName(&_ofn) : GetSaveFileName(&_ofn);
}

String FileDialog::getDirPath() const
{
    return _buffer.substr( 0, _ofn.nFileOffset - 1 );
}

String FileDialog::getFilePath() const
{
    String dirpath = getDirPath();
    int offset = _ofn.nFileOffset;
    if ( offset < _buffer.size() && _ofn.lpstrFile[offset] )
    {
        String fileName = _ofn.lpstrFile + offset;
        offset += (int)fileName.length();
        offset += 1;
        return dirpath + dirSep + fileName;
    }
    return _buffer.c_str();
}

int FileDialog::getFilePaths( StringArray * pArr ) const
{
    String dirpath = getDirPath();
    int offset = _ofn.nFileOffset;
    while ( offset < _buffer.size() && _ofn.lpstrFile[offset] )
    {
        String fileName = _ofn.lpstrFile + offset;
        offset += (int)fileName.length();
        offset += 1;
        pArr->push_back( dirpath + dirSep + fileName );
    }
    return (int)pArr->size();
}

// class Notification ---------------------------------------------------------------------
Notification::Notification( UINT uVersion ) : _uVersion(uVersion)
{
    _construct();
}

Notification::~Notification()
{
    _isAdd && this->del();
}

void Notification::_construct()
{
    _nid.cbSize = sizeof(_nid);
    _nid.uFlags = 0;
    _isAdd = false;
}

bool Notification::add( HWND hwnd, UINT notificationId, Icon const & ico, String const & strTip )
{
    _nid.hWnd = hwnd;
    _nid.uID = notificationId;

    _nid.uFlags = 0;
    this->setIconInfo( ico, strTip );

    if ( !Shell_NotifyIcon( NIM_ADD, &_nid ) )
        return false;

    _isAdd = true;
    _nid.uVersion = _uVersion;
    return Shell_NotifyIcon(NIM_SETVERSION, &_nid) != FALSE;
}

bool Notification::modify()
{
    return Shell_NotifyIcon( NIM_MODIFY, &_nid ) != FALSE;
}

void Notification::setMessage( UINT msg )
{
    _nid.uFlags |= NIF_MESSAGE;
    _nid.uCallbackMessage = msg;
}

void Notification::setBalloonInfo( String const & infoTitle, String const & infoText, DWORD infoFlags /*= NIIF_USER | NIIF_RESPECT_QUIET_TIME */ )
{
    _nid.uFlags |= NIF_INFO;

    _nid.dwInfoFlags = infoFlags;
    SZ(_nid.szInfoTitle) = infoTitle.c_str();
    SZ(_nid.szInfo) = infoText.c_str();
}

void Notification::setIconInfo( Icon const & ico, String const & strTip )
{
    _nid.uFlags |= NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    _nid.hIcon = ico;
    SZ(_nid.szTip) = strTip.c_str();
}

bool Notification::del()
{
    bool r = Shell_NotifyIcon( NIM_DELETE, &_nid ) != FALSE;
    _isAdd = false;
    return r;
}

} // namespace winplus
