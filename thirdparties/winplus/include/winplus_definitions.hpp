//////////////////////////////////////////////////////////////////////////
// purpose: WinPlus库相关定义
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_DEFINITIONS_HPP__)
#define __WINPLUS_DEFINITIONS_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#pragma warning( disable : 4663 )
#pragma warning( disable : 4239 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4091 )

#ifdef __GNUC__
#define WINVER 0x0501
#define _WIN32_IE 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

// 以下是有DLL和无DLL的兼容性宏
// 当存在DLL时(生成DLL或者使用DLL), WINPLUS_DLL_EXPORTS宏用于决定是导出(生成)还是导入(使用).
// WINPLUS_API用于决定调用约定, 有dll时为stdcall, 无dll时为默认.
// 当无DLL时, 宏为空白, 用以兼容.
#ifndef WINPLUS_DLL_USE

    #define WINPLUS_DLL
    #define WINPLUS_API

#else
    #pragma warning( disable: 4275 )
    #pragma warning( disable: 4251 )
    #ifdef  WINPLUS_DLL_EXPORTS
        #define WINPLUS_DLL __declspec(dllexport)
    #else
        #define WINPLUS_DLL __declspec(dllimport)
    #endif

    #define WINPLUS_API __stdcall

#endif

#define WINPLUS_FUNC_DECL(ret) WINPLUS_DLL ret WINPLUS_API
#define WINPLUS_FUNC_IMPL(ret) ret WINPLUS_API

#include "utilities.hpp"

// win32 platform --------------------------------------------------------
#include <windows.h>
#include <comdef.h> // com wrapper
#include <shlobj.h> // shell 外壳
#include <gdiplus.h> // gdi+
//#include <dsclient.h> // DsClient
// cpp stl ---------------------------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <exception>
// c standard functions --------------------------------------------------
#include <math.h>
#include <io.h>
// custom macro definitions ----------------------------------------------
namespace winplus
{
using namespace winux;

#ifdef NOMINMAX

    #ifndef max
        #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
        #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif

#endif

} // namespace winplus

#endif // !defined(__WINPLUS_DEFINITIONS_HPP__)
