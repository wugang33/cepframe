/* 
 * File:   macro_log.hpp
 * Author: WangquN
 *
 * Created on 2011-06-15 AM 10:31
 */

#ifndef MACRO_LOG_HPP
#define	MACRO_LOG_HPP

//#if defined(TRACE_LOG)
//#define LOG_TRACE if (1) std::cout
//#else
//#define LOG_TRACE if (0) std::cout
//#endif

////! Trivial severity levels
//enum severity_level
//{
//    trace, // (,10)
//    debug, // [10, 20)
//    info, // [20, 30)
//    warning, // [30, 40)
//    error, // [40, 50)
//    fatal // [50,)
//};

#ifndef MLOG_LEVEL
#define MLOG_TRACE std::cout << "trace:"
#define MLOG_DEBUG std::cout << "debug:"
#define MLOG_INFO std::cout << "info:"
#define MLOG_WARN std::cout << "warn:"
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#else
#if MLOG_LEVEL < 10
#define MLOG_TRACE std::cout << "trace:"
#define MLOG_DEBUG std::cout << "debug:"
#define MLOG_INFO std::cout << "info:"
#define MLOG_WARN std::cout << "warn:"
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#elif MLOG_LEVEL < 20
#define MLOG_TRACE if (0) std::cout
#define MLOG_DEBUG std::cout << "debug:"
#define MLOG_INFO std::cout << "info:"
#define MLOG_WARN std::cout << "warn:"
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#elif MLOG_LEVEL < 30
#define MLOG_TRACE if (0) std::cout
#define MLOG_DEBUG if (0) std::cout
#define MLOG_INFO std::cout << "info:"
#define MLOG_WARN std::cout << "warn:"
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#elif MLOG_LEVEL < 40
#define MLOG_TRACE if (0) std::cout
#define MLOG_DEBUG if (0) std::cout
#define MLOG_INFO if (0) std::cout
#define MLOG_WARN std::cout << "warn:"
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#elif MLOG_LEVEL < 50
#define MLOG_TRACE if (0) std::cout
#define MLOG_DEBUG if (0) std::cout
#define MLOG_INFO if (0) std::cout
#define MLOG_WARN if (0) std::cout
#define MLOG_ERROR std::cout << "error:"
#define MLOG_FATAL std::cout << "fatal:"
#else
#define MLOG_TRACE if (0) std::cout
#define MLOG_DEBUG if (0) std::cout
#define MLOG_INFO if (0) std::cout
#define MLOG_WARN if (0) std::cout
#define MLOG_ERROR if (0) std::cout
#define MLOG_FATAL std::cout << "fatal:"
#endif
#endif

#include <iostream>

#endif	/* MACRO_LOG_HPP */

