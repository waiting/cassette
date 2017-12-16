#ifndef __FCGISERV_HPP__
#define __FCGISERV_HPP__

#include "eientpl.hpp"

struct FcgiServerData
{
    // 内存式模板缓存结构
    eientpl::TemplateMemoryCache::CacheDataStruct cacheStruct;
    // fcgiserv主程序路径
    winux::String fcgiservPath;
    // fcgiserv进程id
    winux::ulong pid;
    // fastdo版本
    winux::String fastdoVer;
};


#endif // __FCGISERV_HPP__

