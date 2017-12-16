#ifndef __EIENML_HPP__
#define __EIENML_HPP__

#ifdef  EIENML_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  EIENML_DLL_EXPORTS
            #define EIENML_DLL  __declspec(dllexport)
        #else
            #define EIENML_DLL  __declspec(dllimport)
        #endif

        #define EIENML_API __stdcall
    #else
        #define EIENML_DLL
        #define EIENML_API
    #endif
#else
    #define EIENML_DLL
    #define EIENML_API
#endif

#define EIENML_FUNC_DECL(ret) EIENML_DLL ret EIENML_API
#define EIENML_FUNC_IMPL(ret) ret EIENML_API

#include "winux.hpp"
#include "elem-utils.hpp"
#include "elem-package.hpp"
#include "elem-parser.hpp"

#endif // __EIENML_HPP__
