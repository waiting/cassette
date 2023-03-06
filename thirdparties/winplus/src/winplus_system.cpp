#ifdef __GNUC__
#define WINVER 0x0501
#define _WIN32_IE 0x0501
#endif

#define _WIN32_WINNT 0x0501

#include "winplus_definitions.hpp"
#include "winplus_system.hpp"
#include "strings.hpp"
#include "smartptr.hpp"
#include "filesys.hpp"
#include "system.hpp"
#include <tchar.h>
#include <direct.h>
#include <algorithm>

#include <psapi.h>
#pragma comment( lib, "psapi" )

#pragma comment ( lib, "Version.lib" )

namespace winplus
{

// <WinNT.h> --------------------------------------------------------------
// Product types
// This list grows with each OS release.
//
// There is no ordering of values to ensure callers
// do an equality test i.e. greater-than and less-than
// comparisons are not useful.
//
// NOTE: Values in this list should never be deleted.
//       When a product-type 'X' gets dropped from a
//       OS release onwards, the value of 'X' continues
//       to be used in the mapping table of GetProductInfo.
//
#define PRODUCT_UNDEFINED                           0x00000000

#define PRODUCT_ULTIMATE                            0x00000001
#define PRODUCT_HOME_BASIC                          0x00000002
#define PRODUCT_HOME_PREMIUM                        0x00000003
#define PRODUCT_ENTERPRISE                          0x00000004
#define PRODUCT_HOME_BASIC_N                        0x00000005
#define PRODUCT_BUSINESS                            0x00000006
#define PRODUCT_STANDARD_SERVER                     0x00000007
#define PRODUCT_DATACENTER_SERVER                   0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER                0x00000009
#define PRODUCT_ENTERPRISE_SERVER                   0x0000000A
#define PRODUCT_STARTER                             0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE              0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE                0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE              0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64              0x0000000F
#define PRODUCT_BUSINESS_N                          0x00000010
#define PRODUCT_WEB_SERVER                          0x00000011
#define PRODUCT_CLUSTER_SERVER                      0x00000012
#define PRODUCT_HOME_SERVER                         0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER              0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER             0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER            0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER           0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS            0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM        0x00000019
#define PRODUCT_HOME_PREMIUM_N                      0x0000001A
#define PRODUCT_ENTERPRISE_N                        0x0000001B
#define PRODUCT_ULTIMATE_N                          0x0000001C
#define PRODUCT_WEB_SERVER_CORE                     0x0000001D
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    0x0000001E
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      0x0000001F
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     0x00000020
#define PRODUCT_SERVER_FOUNDATION                   0x00000021
#define PRODUCT_HOME_PREMIUM_SERVER                 0x00000022
#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V          0x00000023
#define PRODUCT_STANDARD_SERVER_V                   0x00000024
#define PRODUCT_DATACENTER_SERVER_V                 0x00000025
#define PRODUCT_ENTERPRISE_SERVER_V                 0x00000026
#define PRODUCT_DATACENTER_SERVER_CORE_V            0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V              0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V            0x00000029
#define PRODUCT_HYPERV                              0x0000002A
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE         0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE        0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE       0x0000002D
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE      0x0000002E
#define PRODUCT_STARTER_N                           0x0000002F
#define PRODUCT_PROFESSIONAL                        0x00000030
#define PRODUCT_PROFESSIONAL_N                      0x00000031
#define PRODUCT_SB_SOLUTION_SERVER                  0x00000032
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS             0x00000033
#define PRODUCT_STANDARD_SERVER_SOLUTIONS           0x00000034
#define PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE      0x00000035
#define PRODUCT_SB_SOLUTION_SERVER_EM               0x00000036
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM          0x00000037
#define PRODUCT_SOLUTION_EMBEDDEDSERVER             0x00000038
#define PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE        0x00000039
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE   0x0000003F
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT       0x0000003B
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL       0x0000003C
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC    0x0000003D
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC    0x0000003E
#define PRODUCT_CLUSTER_SERVER_V                    0x00000040
#define PRODUCT_EMBEDDED                            0x00000041
#define PRODUCT_STARTER_E                           0x00000042
#define PRODUCT_HOME_BASIC_E                        0x00000043
#define PRODUCT_HOME_PREMIUM_E                      0x00000044
#define PRODUCT_PROFESSIONAL_E                      0x00000045
#define PRODUCT_ENTERPRISE_E                        0x00000046
#define PRODUCT_ULTIMATE_E                          0x00000047

#define PRODUCT_UNLICENSED                          0xABCDABCD

#define VER_SERVER_NT                       0x80000000
#define VER_WORKSTATION_NT                  0x40000000
#define VER_SUITE_SMALLBUSINESS             0x00000001
#define VER_SUITE_ENTERPRISE                0x00000002
#define VER_SUITE_BACKOFFICE                0x00000004
#define VER_SUITE_COMMUNICATIONS            0x00000008
#define VER_SUITE_TERMINAL                  0x00000010
#define VER_SUITE_SMALLBUSINESS_RESTRICTED  0x00000020
#define VER_SUITE_EMBEDDEDNT                0x00000040
#define VER_SUITE_DATACENTER                0x00000080
#define VER_SUITE_SINGLEUSERTS              0x00000100
#define VER_SUITE_PERSONAL                  0x00000200
#define VER_SUITE_BLADE                     0x00000400
#define VER_SUITE_EMBEDDED_RESTRICTED       0x00000800
#define VER_SUITE_SECURITY_APPLIANCE        0x00001000
#define VER_SUITE_STORAGE_SERVER            0x00002000
#define VER_SUITE_COMPUTE_SERVER            0x00004000
#define VER_SUITE_WH_SERVER                 0x00008000
// <WinUser.h> ------------------------------------------------------------

#if (_WIN32_WINNT >= 0x0501)
#define SM_STARTER              88
#define SM_SERVERR2             89
#endif /* _WIN32_WINNT >= 0x0501 */

/* 目录分割符 */
String const dirSep = TEXT("\\");
String const dirSepUnix = TEXT("/");
String const dirSepMac = TEXT("/");

/* windows行分割符 */
String const lineSep = TEXT("\r\n");
/* unix行分割符 */
String const lineSepUnix = TEXT("\n");
/* mac行分割符 */
String const lineSepMac = TEXT("\r");

/* 判断一个字符串是否可展开，即是否包含%varname% */
WINPLUS_FUNC_IMPL(bool) IsExpandString( String const & str )
{
    LPCTSTR p = str.c_str();
    INT i = 0, j;
    while ( p[i] )
    {
        if ( p[i] == '%' )
        {
            j = i + 1;
            while ( p[j] && p[j] != '%' ) j++;
            if ( p[j] != 0 )
            {
                i = j + 1;
                return true;
            }
        }
        else
        {
            i++;
        }
    }
    return false;
}

/* 展开字符串中存在的'%变量%' */
WINPLUS_FUNC_IMPL(String) ExpandVars( String const & str )
{
    String strRes;
    LPCTSTR p = str.c_str();
    INT i = 0, j;
    while ( p[i] )
    {
        if ( p[i] == '%' )
        {
            j = i + 1;
            while ( p[j] && p[j] != '%' ) j++;
            if ( p[j] == 0 )
            {
                strRes += str.substr( i, j - i );
                i = j;
            }
            else
            {
                String strEnvVar = str.substr( i + 1, j - i - 1 );
                strEnvVar = _tgetenv( strEnvVar.c_str() );
                strRes += strEnvVar;
                i = j + 1;
            }
        }
        else
        {
            strRes += p[i];
            i++;
        }
    }
    return strRes;

}

WINPLUS_FUNC_IMPL(String) GetAppPathFromHWND( HWND hWnd, DWORD * processId )
{
    DWORD dwProcessId;
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    ASSIGN_PTR(processId) = dwProcessId;
    HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcessId );
    if ( !hProcess )
    {
        return _T("");
    }
    String fullName;
    fullName.resize(512);
    DWORD dw = GetModuleFileNameEx( hProcess, NULL, &fullName[0], (DWORD)fullName.size() );
    if ( !dw )
    {
        //dw = GetLastError();
        dw = (DWORD)fullName.size();
        DllLoader dll("Kernel32.dll");
        auto func = dll.func<BOOL (WINAPI *)( HANDLE, DWORD, LPSTR, PDWORD )>("QueryFullProcessImageNameA");
        func && func.call( hProcess, 0, &fullName[0], &dw );
    }
    CloseHandle(hProcess);
    return fullName.c_str();
}

WINPLUS_FUNC_IMPL(String) ModulePath( HMODULE mod, String * fileName )
{
    String sz;
    DWORD dwSize = MAX_PATH >> 1;
    DWORD dwGet = 0;
    do 
    {
        dwSize <<= 1;
        sz.resize(dwSize);
        dwGet = GetModuleFileName( mod, &sz[0], dwSize );
    } while ( dwGet == dwSize );
    return FilePath( String( sz.c_str(), dwGet ), fileName );
}

WINPLUS_FUNC_IMPL(INT) CommandArguments( StringArray * arr )
{
    // 命令行参数处理
    LPWSTR cmdLine = GetCommandLineW();
    INT numArgs, i;
    LPWSTR * argsArr = CommandLineToArgvW( cmdLine, &numArgs );
    for ( i = 0; i < numArgs; ++i )
    {
        arr->push_back( UnicodeToString(argsArr[i]) );
    }
    GlobalFree( (HGLOBAL)argsArr );
    return numArgs;
}

WINPLUS_FUNC_IMPL(bool) ShutdownPrivilege( bool enable )
{
    HANDLE token;
    bool ret = OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token ) != FALSE;
    if ( !ret ) return false;
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid );
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges( token, FALSE, &tp, 0, NULL, NULL );
    ret = ( GetLastError() == ERROR_SUCCESS );
    CloseHandle(token);
    return ret;
}

WINPLUS_FUNC_IMPL(String) GetErrorStr( DWORD err )
{
    String::pointer buf = NULL;
    DWORD dw = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        err,
        0,
        (LPSTR)&buf,
        256,
        NULL
    );
    String str( buf, dw );
    LocalFree(buf);
    return str;
}

WINPLUS_FUNC_IMPL(String) GetNtVersion( void )
{
    String version;
    OSVERSIONINFO osvi;
    SYSTEM_INFO si;
    ZeroMemory( &osvi, sizeof(OSVERSIONINFO) );
    ZeroMemory( &si, sizeof(SYSTEM_INFO) );
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    
    if ( !GetVersionEx(&osvi) )
        return _T("");
    
    if ( osvi.dwPlatformId == VER_PLATFORM_WIN32s ) // Win32s on Windows 3.1.
    {
        version = _T("Windows 3.1 ");
    }
    else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // Windows 95, Windows 98, or Windows Me.
    {
        version = _T("Windows ");
    }
    else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ) // Windows NT, Windows 2000, Windows XP, or Windows Server 2003 family.
    {
        version = _T("Windows NT ");
    }
    
    version += Format( _T("%u.%u.%u"), osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber );
    return version;
}

WINPLUS_FUNC_IMPL(String) GetOsVersion( void )
{
    String version;
    typedef void (WINAPI * PGNSI)(LPSYSTEM_INFO);
    typedef BOOL (WINAPI * PGPI)( DWORD, DWORD, DWORD, DWORD, PDWORD );
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    PGPI pGPI;
    BOOL bOsVersionInfoEx;
    DWORD dwType;

    ZeroMemory( &si, sizeof(SYSTEM_INFO) );
    ZeroMemory( &osvi, sizeof(OSVERSIONINFOEX) );

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !( bOsVersionInfoEx = GetVersionEx( (OSVERSIONINFO *)&osvi ) ) )
        return _T("");

    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
    pGNSI = (PGNSI)GetProcAddress( GetModuleHandle( _T("kernel32.dll") ), "GetNativeSystemInfo" );
    if( NULL != pGNSI )
        pGNSI(&si);
    else
        GetSystemInfo(&si);

    version += _T("Microsoft");

    if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
    {
        typedef void (__stdcall * PFN_RtlGetNtVersionNumbers)(DWORD*, DWORD*, DWORD*);
        DWORD dwMajor, dwMinor, dwBuildNumber;
        bool isHighVersion = false;
        PFN_RtlGetNtVersionNumbers pfnRtlGetNtVersionNumbers = (PFN_RtlGetNtVersionNumbers)GetProcAddress( GetModuleHandle( _T("ntdll.dll") ), "RtlGetNtVersionNumbers");
        if ( pfnRtlGetNtVersionNumbers )
        {
            pfnRtlGetNtVersionNumbers( &dwMajor, &dwMinor, &dwBuildNumber );

            osvi.dwMajorVersion = dwMajor;
            osvi.dwMinorVersion = dwMinor;
            osvi.dwBuildNumber = dwBuildNumber;
            isHighVersion = true;
        }

        // Test for the specific product.
        if ( osvi.dwMajorVersion == 10 )
        {
            if ( osvi.dwMinorVersion == 0 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                    version += _T(" Windows 10");
                else
                    version += _T(" Windows Server 2016");
            }
            goto OS_TYPE;
        }
        else if ( osvi.dwMajorVersion == 6 )
        {
            if ( osvi.dwMinorVersion == 0 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                    version += _T(" Windows Vista");
                else
                    version += _T(" Windows Server 2008");
            }
            else if ( osvi.dwMinorVersion == 1 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                    version += _T(" Windows 7");
                else
                    version += _T(" Windows Server 2008 R2");
            }
            else if ( osvi.dwMinorVersion == 2 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                    version += _T(" Windows 8");
                else
                    version += _T(" Windows Server 2012");
            }
            else if ( osvi.dwMinorVersion == 3 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    version += _T(" Windows 8.1");
                }
                else
                {
                    version += _T(" Windows Server 2012 R2");
                }
            }

        OS_TYPE:
            pGPI = (PGPI)GetProcAddress( GetModuleHandle( _T("kernel32.dll") ), "GetProductInfo" );
            pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

            switch( dwType )
            {
            case PRODUCT_ULTIMATE:
                version += _T(" Ultimate Edition");
                break;
            case PRODUCT_PROFESSIONAL:
                version += _T(" Professional");
                break;
            case PRODUCT_HOME_PREMIUM:
                version += _T(" Home Premium Edition");
                break;
            case PRODUCT_HOME_BASIC:
                version += _T(" Home Basic Edition");
                break;
            case PRODUCT_ENTERPRISE:
                version += _T(" Enterprise Edition");
                break;
            case PRODUCT_BUSINESS:
                version += _T(" Business Edition");
                break;
            case PRODUCT_STARTER:
                version += _T(" Starter Edition");
                break;
            case PRODUCT_CLUSTER_SERVER:
                version += _T(" Cluster Server Edition");
                break;
            case PRODUCT_DATACENTER_SERVER:
                version += _T(" Datacenter Edition");
                break;
            case PRODUCT_DATACENTER_SERVER_CORE:
                version += _T(" Datacenter Edition (core installation)");
                break;
            case PRODUCT_ENTERPRISE_SERVER:
                version += _T(" Enterprise Edition");
                break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
                version += _T(" Enterprise Edition (core installation)");
                break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
                version += _T(" Enterprise Edition for Itanium-based Systems");
                break;
            case PRODUCT_SMALLBUSINESS_SERVER:
                version += _T(" Small Business Server");
                break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                version += _T(" Small Business Server Premium Edition");
                break;
            case PRODUCT_STANDARD_SERVER:
                version += _T(" Standard Edition");
                break;
            case PRODUCT_STANDARD_SERVER_CORE:
                version += _T(" Standard Edition (core installation)");
                break;
            case PRODUCT_WEB_SERVER:
                version += _T(" Web Server Edition");
                break;
            }

        }
        else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
        {
            if( GetSystemMetrics(SM_SERVERR2) )
                version += _T(" Windows Server 2003 R2");
            else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
                version += _T(" Windows Storage Server 2003");
            else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
                version += _T(" Windows Home Server");
            else if( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
                version += _T(" Windows XP Professional x64 Edition");
            else
                version += _T(" Windows Server 2003");

            // Test for the server type.
            if ( osvi.wProductType != VER_NT_WORKSTATION )  
            {
                if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        version += _T(" Datacenter Edition for Itanium-based Systems");
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        version += _T(" Enterprise Edition for Itanium-based Systems");
                }
                else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        version += _T(" Datacenter x64 Edition");
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        version += _T(" Enterprise x64 Edition");
                    else
                        version += _T(" Standard x64 Edition");
                }
                else  
                {  
                    if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                        version += _T(" Compute Cluster Edition");
                    else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        version += _T(" Datacenter Edition");
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        version += _T(" Enterprise Edition");
                    else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                        version += _T(" Web Edition");
                    else
                        version += _T(" Standard Edition");
                }
            }
        }
        else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
        {
            version += _T(" Windows XP");
            if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                version += _T(" Home Edition");
            else
                version += _T(" Professional");
        }
        else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
        {
            version += _T(" Windows 2000");
            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
                version += _T(" Professional");
            }
            else
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                    version += _T(" Datacenter Server");
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                    version += _T(" Advanced Server");
                else
                    version += _T(" Server");
            }
        }

        // Include service pack (if any) and build number.  
        if( _tcslen(osvi.szCSDVersion) > 0 )  
        {  
            version += _T(" ");
            version += osvi.szCSDVersion;
        }

        version += Format( TEXT(" (build %u)"), isHighVersion ? osvi.dwBuildNumber & 0xFFFF : osvi.dwBuildNumber );

        if ( osvi.dwMajorVersion >= 6 )
        {
            if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
                version += TEXT(" 64-bit");
            else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
                version += _T(" 32-bit");
        }
    }
    else
    {
        version += _T(" Windows 9x or lower");
    }

    return version;
}

WINPLUS_FUNC_IMPL(String) GetSelfModuleVersion( void )
{
    String fullPath;
    fullPath = ModulePath( NULL, &fullPath ) + dirSep + fullPath;
    return GetModuleVersion(fullPath);
}

WINPLUS_FUNC_IMPL(String) GetModuleVersion( String const & moduleFile )
{
    String version;
    DWORD verInfoSize = 0;
    DWORD verHandle;
    VS_FIXEDFILEINFO * fileInfo;

    verInfoSize = GetFileVersionInfoSize( moduleFile.c_str(), &verHandle );
    if ( verInfoSize )
    {
        // If we were able to get the information, process it:
        HANDLE memory;
        LPVOID memoryData;
        UINT infoSize = 0;
        WORD prodVersion[4];
        
        memory = GlobalAlloc( GMEM_MOVEABLE, verInfoSize );
        memoryData = GlobalLock(memory);
        GetFileVersionInfo( moduleFile.c_str(), verHandle, verInfoSize, memoryData );
        ::VerQueryValue( memoryData, TEXT("\\"), (LPVOID *)&fileInfo, &infoSize );
        //根据参数的不同，可以获取不同的文件版本信息
        // Product version from the FILEVERSION of the version info resource
        prodVersion[0] = HIWORD(fileInfo->dwFileVersionMS);
        prodVersion[1] = LOWORD(fileInfo->dwFileVersionMS);
        prodVersion[2] = HIWORD(fileInfo->dwFileVersionLS);
        prodVersion[3] = LOWORD(fileInfo->dwFileVersionLS);
        version = Format(
            TEXT("%hu.%hu.%hu.%hu"),
            prodVersion[0],
            prodVersion[1],
            prodVersion[2],
            prodVersion[3]
        );
        
        GlobalUnlock(memory);
        GlobalFree(memory);
    }
    
    return version;
}

} // namespace winplus
