//////////////////////////////////////////////////////////////////////////
// purpose: Shell相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_SHELL_HPP__)
#define __WINPLUS_SHELL_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

// 外壳(Shell)相关 --------------------------------------------------------

/* 从一个路径获得ItemIDList,路径必须存在
   调用者负责用SHGetMalloc(),IMalloc::Free()释放返回的ItemIDList */
WINPLUS_FUNC_DECL(LPITEMIDLIST) ItemIdListFromPath( String const & filePath );
/* 从一个路径获得ItemIDList,不必须存在
   调用者负责用SHGetMalloc(),IMalloc::Free()释放返回的ItemIDList */
WINPLUS_FUNC_DECL(LPITEMIDLIST) ItemIdListFromPathSimple( String const & filePath );
/* 从一个路径获得ItemIDList,可定制
   调用者负责用SHGetMalloc(),IMalloc::Free()释放返回的ItemIDList */
WINPLUS_FUNC_DECL(HRESULT) ItemIdListFromPathCustom( LPWSTR path, WIN32_FIND_DATAW * pfd, LPITEMIDLIST * ppidl );

} // namespace winplus

#endif // !defined(__WINPLUS_SHELL_HPP__)
