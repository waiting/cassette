#ifndef __EIENNET_HPP__
#define __EIENNET_HPP__

#include "winux.hpp"
#include "http.hpp"

#ifdef  EIENNET_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  EIENNET_DLL_EXPORTS
            #define EIENNET_DLL  __declspec(dllexport)
        #else
            #define EIENNET_DLL  __declspec(dllimport)
        #endif

        #define EIENNET_API __stdcall
    #else
        #define EIENNET_DLL
        #define EIENNET_API
    #endif
#else
    #define EIENNET_DLL
    #define EIENNET_API
#endif

#define EIENNET_FUNC_DECL(ret) EIENNET_DLL ret EIENNET_API
#define EIENNET_FUNC_IMPL(ret) ret EIENNET_API

#include "curl.hpp"
#include "socket.hpp"

/** \brief 网络资源访问库 */
namespace eiennet
{



}

#endif // __EIENNET_HPP__
