#ifndef __EIENWEBX_HPP__
#define __EIENWEBX_HPP__

#if _MSC_VER > 0
#pragma warning( disable: 4244 )
#endif

namespace eien
{
namespace webx
{

#ifdef  EIENWEBX_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  EIENWEBX_DLL_EXPORTS
            #define EIENWEBX_DLL  __declspec(dllexport)
        #else
            #define EIENWEBX_DLL  __declspec(dllimport)
        #endif

        #define EIENWEBX_API __stdcall
    #else
        #define EIENWEBX_DLL
        #define EIENWEBX_API
    #endif
#else
    #define EIENWEBX_DLL
    #define EIENWEBX_API
#endif

#define EIENWEBX_FUNC_DECL(ret) EIENWEBX_DLL ret EIENWEBX_API
#define EIENWEBX_FUNC_IMPL(ret) ret EIENWEBX_API

class EIENWEBX_DLL App
{

};

class EIENWEBX_DLL CgiApp : public App
{

};

class EIENWEBX_DLL FcgiApp : public App
{

};

class EIENWEBX_DLL SessionServer
{

};

class EIENWEBX_DLL MemorySessionServer : public SessionServer
{

};

class EIENWEBX_DLL FileSessionServer : public SessionServer
{

};

class EIENWEBX_DLL DatabaseSessionServer : public SessionServer
{

};

class EIENWEBX_DLL Session
{

};

// ----------------------------------------------------------------------------------------

class EIENWEBX_DLL Request
{

};

class EIENWEBX_DLL CgiRequest : public Request
{

};

class EIENWEBX_DLL FcgiRequest : public Request
{

};

class EIENWEBX_DLL IoManager
{

};

class EIENWEBX_DLL CgiIoManager : public IoManager
{

};

class EIENWEBX_DLL FcgiIoManager : public IoManager
{

};

class EIENWEBX_DLL Response
{

};


}
}





#endif // __EIENWEBX_HPP__