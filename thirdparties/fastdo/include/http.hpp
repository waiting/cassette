#ifndef __HTTPWRAPPER_HPP__
#define __HTTPWRAPPER_HPP__

#include "winux.hpp"

#ifdef  HTTPWRAPPER_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  HTTPWRAPPER_DLL_EXPORTS
            #define HTTPWRAPPER_DLL  __declspec(dllexport)
        #else
            #define HTTPWRAPPER_DLL  __declspec(dllimport)
        #endif

        #define HTTPWRAPPER_API __stdcall
    #else
        #define HTTPWRAPPER_DLL
        #define HTTPWRAPPER_API
    #endif
#else
    #define HTTPWRAPPER_DLL
    #define HTTPWRAPPER_API
#endif

#define HTTPWRAPPER_FUNC_DECL(ret) HTTPWRAPPER_DLL ret HTTPWRAPPER_API
#define HTTPWRAPPER_FUNC_IMPL(ret) ret HTTPWRAPPER_API

#include "http_misc.hpp"
#include "http_url.hpp"

/** \brief http协议的相关简单类封装 */
namespace http
{

}

#endif // __HTTPWRAPPER_HPP__
