#ifndef __BASEUTILS_CONFIG_H__
#define __BASEUTILS_CONFIG_H__

#ifdef _WIN32
#pragma warning (disable : 4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cwctype>
#include <vector>
#include <list>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>


#ifdef _WIN32
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x500)
#undef _WIN32_WINNT
#define   _WIN32_WINNT     0x500
#endif
#if defined(_MFC_VER) || defined(_AFXDLL)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <winsock2.h>
#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>
#else
#include <windows.h>
#include <WINSOCK2.H>
#endif
typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int8 uint8_t;
typedef __int64 int64_t;
typedef __int32 int32_t;
typedef __int16 int16_t;
typedef __int8 int8_t;
typedef int socklen_t;
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
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
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <dirent.h>
#include <signal.h>
#include <libgen.h>
#include <dlfcn.h>
#include <sys/statvfs.h>
#ifndef MAX_PATH
#define  MAX_PATH     260
#endif

#endif

#if defined(_AIX)
// #include <procsinfo.h>
#endif

#include "FSPublic.h"

#endif
