
#ifndef _FS_PUBLIC_H_
#define  _FS_PUBLIC_H_

#ifdef UNIT_TEST
#define private public
#define protected public 
#endif

#ifndef DECLARE_HANDLE
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif

/**
 @mainpage  include目录 说明文档

 @section Usage 简介

 include目录下所包含文件的内容支持跨平台。
 头文件中包含了一些常用的标准库、字节序转换、类型定义、前端服务器中使用到的命令字等。
 在工程中使用时需要在INCLUDE路径中包含include目录。
 
 @section Main 主要文件说明
 
 @subsection SecFarServer FSStruct.h
   - 源码链接 FSStruct.h
   - FarServer用到的公共结构定义
 
 @subsection SecFSDeclare FSDeclare.h
   - 源码链接 FSDeclare.h
   - FarServer用到的公共常量定义

 @subsection SecDataUtil DataUtil.h
   - 源码链接 DataUtil.h
   - 不同CDR格式一直不统一，可用这个类来获取结构体中的时间、号码等
   - 对应的 DataUtil.cpp 为一些结构体的时间、号码的模板特例函数实现文件 
   
 @subsection SecDataConvert DataConvert.h
   - 源码链接 DataConvert.h
   - FSStruct.h 中的各结构的字节序转换
   - 对应的 DataConvert.cpp 为一些结构体的字节序转换的模板特例函数实现文件 

 @subsection SecFSPublic FSPublic.h
   - 源码链接 FSPublic.h
   - 包含字节序转换宏
   - 包含常用的标准库
   - 包含一些类型定义
   - 包含 FSStruct.h 与 FSDeclare.h 头文件
   - 包含整型数字转换为10进制或16进制的字符串函数
   - 包含使用一些线程安全的函数，如 localtime_r 与 readdir_r

 @subsection SecMacro macro.h
   - 源码链接 macro.h
   - 一些宏定义，主要有字节序转换、单实例宏等

 @subsection SecVersionWriter VersionWriter.h
   - 源码链接 VersionWriter.h
   - 提供版本信息写入INI文件的功能，需配合静态库baseutils使用  
 */


/*! \file 
  代码要支持跨平台，因此加入了一些宏判断，这样的话，使用Doxygen加注释就有问题，有些分支注释与代码显示不出来，<br>
  所以在这里只能用注释文本的方式来呈现。<br>

  该头文件中主要实现了以下一些用途<br>
   - 包含字节序转换宏<br>
   - 包含常用的标准库(根据不同的平台添加不同的库)<br>
   - 包含一些类型定义<br>
   - 包含FSStruct.h与FSDeclare.h头文件<br>
   - 包含整型数字转换为10进制或16进制的字符串函数<br>
   - 包含使用一些线程安全的函数，如localtime_r与readdir_r<br>

  一些定义如下：<br>
   - 当没有定义WIN32宏时，会替换使用下面两个线程安全函数、一个将两个BYTE型合并为一个WORD型的宏<br>
     //// localtime_r readdir_r  is Thread Safe (pls refer to vacpp documents)<br>
     @code
     #define localtime( timer ) localtime_r( timer , &tmCurrent )
     #define readdir readdir_r
     @endcode
     //// 将两个BYTE型合并为一个WORD型,a:WORD型的低8位,b:WORD型的高8位<br>
     @code
     #define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
     @endcode

   - 当定义了WIN32宏时，会定义下面这个转换函数<br>
     //// 整数转换为10进制或16进制字符串，value:待转换的整数值, pstrRet:转换后的字符串, radix:10进制或16进制转换标志<br>
     @code
     inline char* itoa( int value, char *pstrRet, int radix  )<br>
     {
             //// 只是支持10 进制和 16进制
             if( radix == 10 )	
                    sprintf( pstrRet , "%d" , value );
             else if( radix == 16 )
                    sprintf( pstrRet , "%x" , value );
             return pstrRet ;
     }
     @endcode

  - 新框架下字节序按照标准方式处理，即发送方调用htonx, 接收方调用ntohx, 同时也
     支持定义默认字节序为小字节序的处理方式,以兼容以前的处理方式
     @code
     #ifndef DEFAULT_BYTE_ORDER
     #define DEFAULT_BYTE_ORDER BIG_ENDIAN
     #endif // #ifndef DEFAULT_BYTE_ORDER
     
     #if DEFAULT_BYTE_ORDER == BIG_ENDIAN
         #define ntohs_ex(value) ntohs(value)
         #define ntohl_ex(value) ntohl(value)
         #define htons_ex(value) htons(value)
         #define htonl_ex(value) htonl(value)
       
         #if BYTE_ORDER == BIG_ENDIAN
             #define ntohll_ex(value) (value)
             #define htonll_ex(value) (value)
         #else
             #define ntohll_ex(value) \
                 ((((UINT64)((UINT64)(value) & 0x00000000000000ff)) << 56) \
                | (((UINT64)((UINT64)(value) & 0x000000000000ff00)) << 40) \
                | (((UINT64)((UINT64)(value) & 0x0000000000ff0000)) << 24) \
                | (((UINT64)((UINT64)(value) & 0x00000000ff000000)) << 8)  \
                | (((UINT64)((UINT64)(value) & 0xff00000000000000)) >> 56) \
                | (((UINT64)((UINT64)(value) & 0x00ff000000000000)) >> 40) \
                | (((UINT64)((UINT64)(value) & 0x0000ff0000000000)) >> 24) \
                | (((UINT64)((UINT64)(value) & 0x000000ff00000000)) >> 8))

             #define htonll_ex(value) ntohll_ex(value)
         #endif // #if BYTE_ORDER == BIG_ENDIAN
             
     #else // #if DEFAULT_BYTE_ORDER == BIG_ENDIAN
               
         #if BYTE_ORDER == BIG_ENDIAN
             #define		ntohs_ex(value) \
                 ((((WORD)((WORD)(value) & 0x00ff)) << 8) \ 
                | (((WORD)((WORD)(value) & 0xff00)) >> 8))
                 
             #define		ntohl_ex(value) \
                 ((((UINT)((UINT)(value) & 0x000000ff)) << 24)  \ 
                | (((UINT)((UINT)(value) & 0x0000ff00)) << 8) \
                | (((UINT)((UINT)(value) & 0x00ff0000)) >> 8) \ 
                | (((UINT)((UINT)(value) & 0xff000000)) >> 24))
                   
             #define		ntohll_ex(value) \
                 ((((UINT64)((UINT64)(value) & 0x00000000000000ff)) << 56) \ 
                | (((UINT64)((UINT64)(value) & 0x000000000000ff00)) << 40) \ 
                | (((UINT64)((UINT64)(value) & 0x0000000000ff0000)) << 24) \
                | (((UINT64)((UINT64)(value) & 0x00000000ff000000)) << 8)  \
                | (((UINT64)((UINT64)(value) & 0xff00000000000000)) >> 56) \
                | (((UINT64)((UINT64)(value) & 0x00ff000000000000)) >> 40) \
                | (((UINT64)((UINT64)(value) & 0x0000ff0000000000)) >> 24) \
                | (((UINT64)((UINT64)(value) & 0x000000ff00000000)) >> 8))
                               
         #else // #if BYTE_ORDER == BIG_ENDIAN
                         
             #define     ntohl_ex(value)  (value)
             #define     ntohs_ex(value)  (value)
             #define		ntohll_ex(value) (value)   
                                 
         #endif // #if BYTE_ORDER == BIG_ENDIAN
           
         #define     htons_ex(value) (value)
         #define     htonl_ex(value) (value)
         #define     htonll_ex(value) (value)
                                   
     #endif // #if DEFAULT_BYTE_ORDER == BIG_ENDIAN
     @endcode

   - 包含了FarServer用到的公共结构定义与公共常量定义<br>
     @code
     #include "FSStruct.h"<br>
     #include "FSDeclare.h"
     @endcode

  具体的更详尽的一些实现请参见该文件FSPublic.h的源代码<br>
 */

/// @cond __HIDE__
#define TIMELABELLENS 8 

#ifdef _WIN32

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x500)
#undef _WIN32_WINNT
#define   _WIN32_WINNT     0x500
#endif

#pragma warning( disable : 4786 )
#if defined(_MFC_VER) || defined(_AFXDLL)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <winsock2.h>
#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>
#else
#include <windows.h>
#endif
#endif // end WIN32


#ifndef _WIN32
/// stand library
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> 
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <dirent.h>
#include <signal.h>
#include <libgen.h>
#include <errno.h>

//#ifdef _AIX
//#include <synch.h>
////#include <typeinfo.h>
//
//#include <stropts.h>
///// aix没有atoll
//#define atoll(x) strtoll(x, NULL, 10)
//#endif //END _AIX

#ifdef HPUNIX
#ifdef __LP64__
#define atoll(x) strtol(x, NULL, 10)
#else
#define atoll(x) strtoll(x, NULL, 10)
#endif

#endif //END HPUNIX

#else

#include <direct.h>
#include <io.h>
#include <fcntl.h>      
#endif

#include <assert.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <set>
#include <deque>
#include <algorithm>
#include <time.h>

using namespace std;

#pragma warning(default:4244)

#if __mips__
#include <string.h>

extern "C" void *mips_memcpy(void *s1, const void *s2, size_t n);
extern "C" void *mips_memset(void *s, int c, size_t n);

#define memmove mips_memcpy
#define memcpy  mips_memcpy
#define memset  mips_memset
//typedef mips_memcpy	memcpy;
//typedef mips_memset	memset;

#endif

/// localtime_r readdir_r  is Thread Safe (pls refer to vacpp documents)
#ifndef _WIN32

// #ifdef localtime
// #undef  localtime
// #endif
// #define localtime( timer ) localtime_r( timer , &tmCurrent )

#ifdef readdir
#undef  readdir
#define readdir readdir_r

#endif

#endif

//const int	BatchCount = 5000;

//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32 /// windows define
#define THREAD_FUN   DWORD WINAPI
#define atoll _atoi64
#define itoa _itoa
#define getpid _getpid
#define stricmp _stricmp

#define MAKE_64U(value) (value)
#define LL(value) (value)
#define I64U "%I64u"
#define I64S "%I64"
#define I64X "0x%I64x"

#else /// UNIX define
#define THREAD_FUN	  void*

#define MAKE_64U(value) (value ## ll)
#define LL(value) (value ## ll)
#define I64U "%llu"
#define I64S "%ll"
#define I64X "0x%llx"

#define stricmp strcasecmp

#ifndef MAX_PATH
#define MAX_PATH		260
#endif

#define FALSE			0
#define TRUE			1

typedef int BOOL;
typedef unsigned char BYTE;
typedef BYTE byte;
typedef short SHORT;
typedef unsigned int DWORD; ///< !!注意!! long在UNIX LP64模式下是64位。
typedef int INT;
typedef unsigned int UINT;
typedef unsigned int UINT32;
typedef unsigned short WORD;
typedef char CHAR;
typedef void* LPVOID;
typedef BYTE* LPBYTE;
typedef WORD* LPWORD;
typedef DWORD* LPDWORD;
typedef char* LPCSTR; // 这个定义不对。为了与BCM兼容只好这样写
typedef const char* LPCTSTR;
typedef UINT HANDLE;

#ifdef __LP64__
typedef unsigned long UINT64;
typedef long INT64;
#else
typedef unsigned long long UINT64;
typedef long long INT64;
#endif


#ifndef __ITOA__
#define __ITOA__

/**
 * @brief 整数转换为10进制或16进制字符串
 *
 * @param value 待转换的整数值
 * @param pstrRet 转换后的字符串
 * @param radix 转换标志，支持10进制与16进制
 */
inline char* itoa(int value, char *pstrRet, int radix) {
    // 只是支持10 进制和 16进制
    if (radix == 10)
        sprintf(pstrRet, "%d", value);
    else if (radix == 16)
        sprintf(pstrRet, "%x", value);

    return pstrRet;
}
#endif //__ITOA__


#endif // WIN32

struct __PLACEMENT {
};
typedef __PLACEMENT* PLACEMENT;

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BYTE_ORDER
#if defined(_WIN32) || defined(i386) || defined(__i386__) || defined(__i386) || \
  defined(_M_IX86) || defined(_X86_)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif

#ifdef _DEBUG
#define DEBUG_PRINT(printf_exp)(printf  printf_exp)
#else
#define DEBUG_PRINT(printf_exp)((void)0) 
#endif

#ifndef _WIN32

/**
 * @brief 将两个BYTE型合并为一个WORD型
 * 
 * @param a WORD型的低8位
 * @param b WORD型的高8位
 */
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))

/**
 * @brief 将两个WORD型合并为一个LONG型
 * 
 * @param a LONG型的低16位
 * @param b LONG型的高16位
 */
#define MAKELONG(a, b)      ((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))

/** 
 * @brief 取一个WORD的低字节
 */
#define LOBYTE(a)   ((a) & 0xFF)

/** 
 * @brief 取一个WORD的高字节
 */
#define HIBYTE(a)   ((a) >> 8)

/** 
 * @brief 取一个DWORD的低16位
 */
#define LOWORD(a)   ((a) & 0xFFFF)

/** 
 * @brief 取一个DWORD的高16位
 */
#define HIWORD(a)   ((a) >> 16)

#ifndef INFINITE
#define INFINITE (-1)
#endif

#ifdef _DEBUG
#define ASSERT(expr) assert(expr)
#define DEBUG_NEW new
#define TRACE printf
#else
#define ASSERT(expr) ((void) 0)

inline void release_trace(LPCTSTR, ...) {
}
#define TRACE release_trace 
#endif




#endif

/**
 * @brief 将四个字节合并为一个LONG型
 * 
 * @param a LONG型的低8位
 * @param b LONG型的8 - 15位
 * @param c LONG型的16 - 23位
 * @param d LONG型的24 - 31位
 */
#define MAKELONGFROMBYTE(a, b, c, d) (MAKELONG(MAKEWORD(a, b), MAKEWORD(c, d)))

/**
 * @brief 将两个DWORD型合并为一个UINT64型
 * 
 * @param a UINT64型的低32位
 * @param b UINT64型的高32位
 */
#define MAKELONGLONG(a, b)   ((UINT64)(((DWORD)(a)) | ((UINT64)((DWORD)(b))) << 32))

/** 
 * @brief 取一个UINT64的低32位
 */
#define LODWORD(a)   ((a) & 0xFFFFFFFF)

/** 
 * @brief 取一个UINT64的高32位
 */
#define HIDWORD(a)   ((a) >> 32)

/**
 * @brief 新框架下字节序按照标准方式处理，即发送方调用htonx, 接收方调用ntohx, 
   同时也支持定义默认字节序为小字节序的处理方式
 *
 * 在老框架时，前端机仅支持小端字节序，在发送给前端服务器时，没有考虑字节序问题，
 * 即默认的字节序为小端字节序，前端服务器在接收到XDR后，根据自己的字节序，
 * 用ntohx_ex将XDR进行转换，因为前端机默认的是小端字节序，所以ntohx_ex正好与ntohx
 * 行为相反。新框架中，前端机小端/大端都有可能，因此按照标准的字节序处理方式更好，
 * 即发送方调用htonx, 接收方调用ntohx, 同时也兼容以前的处理方式，此时只需定义
 * DEFAULT_BYTE_ORDER = LITTLE_ENDIAN即可
 */
#ifndef DEFAULT_BYTE_ORDER
#define DEFAULT_BYTE_ORDER BIG_ENDIAN
#endif // #ifndef DEFAULT_BYTE_ORDER

#if DEFAULT_BYTE_ORDER == BIG_ENDIAN
#define ntohs_ex(value) ntohs(value)
#define ntohl_ex(value) ntohl(value)
#define htons_ex(value) htons(value)
#define htonl_ex(value) htonl(value)
#if BYTE_ORDER == BIG_ENDIAN
#define	ntohll_ex(value) (value)
#define htonll_ex(value) (value)
#else
#define	ntohll_ex(value) \
            ((((UINT64)((UINT64)(value) & LL(0x00000000000000ff))) << 56) \
           | (((UINT64)((UINT64)(value) & LL(0x000000000000ff00))) << 40) \
           | (((UINT64)((UINT64)(value) & LL(0x0000000000ff0000))) << 24) \
           | (((UINT64)((UINT64)(value) & LL(0x00000000ff000000))) << 8)  \
           | (((UINT64)((UINT64)(value) & LL(0xff00000000000000))) >> 56) \
           | (((UINT64)((UINT64)(value) & LL(0x00ff000000000000))) >> 40) \
           | (((UINT64)((UINT64)(value) & LL(0x0000ff0000000000))) >> 24) \
           | (((UINT64)((UINT64)(value) & LL(0x000000ff00000000))) >> 8))
#define	htonll_ex(value) ntohll_ex(value)
#endif // #if BYTE_ORDER == BIG_ENDIAN

#else // #if DEFAULT_BYTE_ORDER == BIG_ENDIAN

#if BYTE_ORDER == BIG_ENDIAN
#define		ntohs_ex(value) \
            ((((WORD)((WORD)(value) & 0x00ff)) << 8) \
           | (((WORD)((WORD)(value) & 0xff00)) >> 8))

#define		ntohl_ex(value) \
			((((UINT)((UINT)(value) & 0x000000ff)) << 24) \
           | (((UINT)((UINT)(value) & 0x0000ff00)) << 8) \
           | (((UINT)((UINT)(value) & 0x00ff0000)) >> 8) \
           | (((UINT)((UINT)(value) & 0xff000000)) >> 24))

#define		ntohll_ex(value) \
		    ((((UINT64)((UINT64)(value) & LL(0x00000000000000ff))) << 56) \
           | (((UINT64)((UINT64)(value) & LL(0x000000000000ff00))) << 40) \
           | (((UINT64)((UINT64)(value) & LL(0x0000000000ff0000))) << 24) \
           | (((UINT64)((UINT64)(value) & LL(0x00000000ff000000))) << 8)  \
           | (((UINT64)((UINT64)(value) & LL(0xff00000000000000))) >> 56) \
           | (((UINT64)((UINT64)(value) & LL(0x00ff000000000000))) >> 40) \
           | (((UINT64)((UINT64)(value) & LL(0x0000ff0000000000))) >> 24) \
           | (((UINT64)((UINT64)(value) & LL(0x000000ff00000000))) >> 8))

#else // #if BYTE_ORDER == BIG_ENDIAN

#define     ntohl_ex(value)  (value)
#define     ntohs_ex(value)  (value)
#define		ntohll_ex(value) (value)   

#endif // #if BYTE_ORDER == BIG_ENDIAN    

#define     htons_ex(value) ntohs_ex(value)
#define     htonl_ex(value) ntohl_ex(value)
#define     htonll_ex(value) ntohll_ex(value)

#endif // #if DEFAULT_BYTE_ORDER == BIG_ENDIAN

/**
 * @brief 将字节序反转 
 *
 * @param value 待转换的32位整形数
 */

#define		convert_16_ex( value ) \
           ((((WORD)((WORD)(value) & 0x00ff)) << 8) \
          | (((WORD)((WORD)(value) & 0xff00)) >> 8))

#define		convert_32_ex( value ) \
		   ((((UINT)((UINT)(value) & 0x000000ff)) << 24) \
          | (((UINT)((UINT)(value) & 0x0000ff00)) << 8) \
          | (((UINT)((UINT)(value) & 0x00ff0000)) >> 8) \
          | (((UINT)((UINT)(value) & 0xff000000)) >> 24)) 

#define		convert_64_ex(value) \
           ((((UINT64)((UINT64)(value) & LL(0x00000000000000ff))) << 56) \
          | (((UINT64)((UINT64)(value) & LL(0x000000000000ff00))) << 40) \
          | (((UINT64)((UINT64)(value) & LL(0x0000000000ff0000))) << 24) \
          | (((UINT64)((UINT64)(value) & LL(0x00000000ff000000))) << 8)  \
          | (((UINT64)((UINT64)(value) & LL(0xff00000000000000))) >> 56) \
          | (((UINT64)((UINT64)(value) & LL(0x00ff000000000000))) >> 40) \
          | (((UINT64)((UINT64)(value) & LL(0x0000ff0000000000))) >> 24) \
          | (((UINT64)((UINT64)(value) & LL(0x000000ff00000000))) >> 8))


#define ntohs_ex_w(value)  convert_16_ex(value)
#define ntohl_ex_w(value)  convert_32_ex(value)
#define ntohll_ex_w(value) convert_64_ex(value)

#define swap_16_ex(value)  (value = convert_16_ex(value))
#define swap_32_ex(value)  (value = convert_32_ex(value))
#define swap_64_ex(value)  (value = convert_64_ex(value))

/// @endcond

#ifndef WIN32
#define DLLEXPORT 
#define __declspec(dllexport) 
#else
#ifndef DLLEXPORT 
#define DLLEXPORT   __declspec(dllexport)
#endif
#endif

// 宏定义
#define FRAME_SERIALIZE_FLAG       0x9e42
#define FRAME_SERIALIZE_FLAG_X     0x429e
#define MAX_NAME_LEN               256

enum ErrMsg {
    /// 标志无错误返回，但也没有成功, 例如一个函数的目的是把一个包放入到一个队列中
    /// 如果发现这个包本身不需要插入到队列中，此时需要返回，它没有出现错误，但也没有
    /// 成功的将包放入队列中，此时可返回HNDL_EOF, 表示无错误返回
    HNDL_EOF = -1, ///< 结束标志
    SUCCEED = 0, ///< 成功	

    /// 系统错误，不能明确责任方
    ERR_SYS_PARAM_NULL, ///< 参数为空
    ERR_SYS_PARAM_INVALID, ///< 参数无效

    ERR_SYS_OTHER, ///< 系统其他错误

    /// 平台错误
    ERR_FP_HANDLER_ACCESS, ///< Handler 访问失败

    ERR_FP_RESULT_SAVE, ///< 解码结果无法保存
    ERR_FP_RESULT_ACCESS, ///< 解码结果无法访问

    ERR_FP_ASSISTANT_SAVE, ///< 辅助信息无法保存
    ERR_FP_ASSISTANT_ACCESS, ///< 辅助信息无法访问

    ERR_FP_MEMPOOL, ///< 获取内存失败
    ERR_FP_OBJPOOL, ///< 获取对象失败

    ERR_FP_QUEUE, ///< 队列相关

    ERR_FP_SERIALIZE, ///< 序列化失败
    ERR_FP_DESERIALIZE, ///< 反序列化失败

    ERR_FP_BUFFER_FULL, ///< 缓冲区满

    ERR_FP_OTHER, ///< 平台其他错误

    /// Handler 错误
    ERR_HNDL_INNER, ///< 处理器内部错误
    ERR_HNDL_GET_IDLE_NODE, ///< 无空闲节点
    ERR_HNDL_FIND_NODE, ///< 查询节点时失败
    ERR_HNDL_EVENT_ASSOCIATE, ///< 事件关联失败

    ERR_HNDL_CONNECT_SERVER, ///< 链接服务器失败

    ERR_HNDL_OTHER, ///< Handler 其他错误

    /// 数据错误
    ERR_PROTO_VER, ///< 协议版本不支持
    ERR_DATA_FORMAT, ///< 数据格式错误
    ERR_DATA_RESEND, ///< 重发包
    ERR_DATA_OUT_OF_ORDER, ///< 乱序
    ERR_DATA_LOSE, ///< 数据丢失(丢失分片)

    ERR_DATA_OTHER, ///< 数据其他错误

    /// 配置错误
    ERR_CONFIG_NONE, ///< 没有发现配置
    ERR_CONFIG_INVALID, ///< 无效配置

    /// 组件错误
    ERR_COMPONENT_NULL, ///< 组件不存在
    ERR_COMPONENT_INVALID, ///< 无效组件

    /// 属性错误
    ERR_PROPERTY_NULL, ///< 属性不存在
    ERR_PROPERTY_INVALID, ///< 无效属性

    /// 业务错误
    ERR_INVALID_VALUE, ///< 无效的值
    ERR_NO_SUPPORT_FUNCTION, ///< 不支持的功能
    ERR_USER_BUFFER_TOO_SMALL, ///< 用户空间太小

    ERR_MAX
};

#define ERRMSGSIZE 128 

static struct errmsg {
    int e_code;
    char e_msg[2][ERRMSGSIZE];
} errmsgs[] = {
    { SUCCEED, "成功", "SUCCEED"},
    { ERR_SYS_PARAM_NULL, "传入参数为空", "Param is null!"},
    { ERR_SYS_PARAM_INVALID, "传入参数非法", "Param is invalid!"},
    { ERR_SYS_OTHER, "系统其他错误", "Other error from system!"},

    { ERR_FP_HANDLER_ACCESS, "Handler 访问失败", "Failed to access handler!"},

    { ERR_FP_RESULT_SAVE, "保存解码结果时失败", "Failed to save result!"},
    { ERR_FP_RESULT_ACCESS, "获取解码结果时失败", "Failed to get result!"},

    { ERR_FP_ASSISTANT_SAVE, "保存辅助信息时失败", "Failed to save assistant info!"},
    { ERR_FP_ASSISTANT_ACCESS, "获取辅助信息时失败", "Failed to get assistant info!"},

    { ERR_FP_MEMPOOL, "申请内存时失败", "Failed to get memory from memory pool!"},
    { ERR_FP_OBJPOOL, "申请对象时失败", "Failed to get object from object pool!"},

    { ERR_FP_QUEUE, "队列相关", "Failed to operator queue!"},

    { ERR_FP_SERIALIZE, "序列化失败", "Failed to serialize!"},
    { ERR_FP_DESERIALIZE, "反序列化失败", "Failed to deserialize!"},

    { ERR_FP_BUFFER_FULL, "缓冲区满", "Buffer is full!"},

    { ERR_FP_OTHER, "平台其他错误", "Other error from front_probe!"},

    { ERR_HNDL_INNER, "处理器内存错误", "Inner error from handler!"},
    { ERR_HNDL_GET_IDLE_NODE, "无空闲节点", "No idle node!"},
    { ERR_HNDL_FIND_NODE, "发现以前的节点失败", "Failed to find node!"},
    { ERR_HNDL_EVENT_ASSOCIATE, "事件关联失败", "Failed to associate event!"},

    {ERR_HNDL_CONNECT_SERVER, "链接服务器失败", "Failed to connect server!"},

    { ERR_HNDL_OTHER, "处理器其他错误", "Other error from handler!"},

    { ERR_PROTO_VER, "协议版本不支持", "Error proto version!"},
    { ERR_DATA_FORMAT, "数据格式错误", "Data format invalid!"},
    { ERR_DATA_RESEND, "重发包", "Resend data!"},
    { ERR_DATA_OUT_OF_ORDER, "乱序包", "Data out of order!"},
    { ERR_DATA_LOSE, "数据丢失(丢失分片)", "Data lose!"},

    { ERR_DATA_OTHER, "其他数据错误", "Other error from data!"},

    { ERR_CONFIG_NONE, "没有发现配置", "Config NO Found!"},
    { ERR_CONFIG_INVALID, "无效配置", "Config invalid!"},

    { ERR_COMPONENT_NULL, "组件不存在", "Component NO Found!"},
    { ERR_COMPONENT_INVALID, "无效组件" "Component invalid"},

    { ERR_PROPERTY_NULL, "属性不存在", "Attribute NO Found!"},
    { ERR_PROPERTY_INVALID, "无效属性" "Attribute invalid"},

    { ERR_INVALID_VALUE, "无效的值", "Value invalid"},
    { ERR_NO_SUPPORT_FUNCTION, "不支持的功能", "NO Support Function"},
    { ERR_USER_BUFFER_TOO_SMALL, "用户空间太小", "User buffer too small"},

    { ERR_MAX, "\0", "\0"}
};

#define ERRMSG_BUFSIZE 255

/*根据错误代码查找错误信息*/
inline char * errtomsg(int error, unsigned char language = 0) {
    register struct errmsg *perror;
    static char buf[ERRMSG_BUFSIZE + 1];

    for (perror = errmsgs; perror->e_msg[language]; perror++) {
        if (perror->e_code == error)
            return (perror->e_msg[language]);
    }
    if (error > ERR_MAX) {
        strncpy(buf, strerror(error - ERR_MAX), ERRMSG_BUFSIZE); // 获取系统错误信息
        return (buf);
    }
    strcpy(buf, "");
    return ( buf);
}

// #if defined(_UNIX) || defined(_AIX) || defined(_LINUX) || 
// #define yield (sched_yield())
// //#define yield (sleep(0))
// #endif
//#ifdef _WIN32
//#define yield (SleepEx(0,0))
//#elif defined(__SunOS_5_8) || defined(__SunOS_5_9)
//#define yield (Utility::FDelay(1))
//#else
//#define yield (sched_yield())
//#endif

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#endif //_PUBLIC_H_
