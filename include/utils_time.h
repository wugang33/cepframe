/* 
 * File:   TimeUtils.h
 * Author: Administrator
 *
 * Created on 2012年3月15日, 上午11:05
 */

#ifndef TIMEUTILS_H
#define	TIMEUTILS_H
#include <time.h>
#include <errno.h>
/*
 * 定义时区   默认是东八区 如果是西八区 那么就是-8
 */
#ifndef _CEPTIMEZONE
#define _CEPTIMEZONE 0
#endif
#ifndef TM_YEAR_BASE
#define TM_YEAR_BASE 1900
#endif

class utils_time {
public:

    /*
     * @param tm 时间结构体
     * @return time_t 1970年到现在经过的秒数
     */

    static time_t mktime(const struct tm* ptm);
    
    static int localtime_r(time_t *t, struct tm*tp);

private:
    static inline time_t
    ydhms_diff(long int year1, long int yday1, int hour1, int min1, int sec1,
            int year0, int yday0, int hour0, int min0, int sec0);

    static inline int
    leapyear(long int year) {
        /* Don't add YEAR to TM_YEAR_BASE, as that might overflow.
           Also, work even if YEAR is negative.  */
        return
        ((year & 3) == 0
                && (year % 100 != 0
                || ((year / 100) & 3) == (-(TM_YEAR_BASE / 100) & 3)));
    }
private:
    utils_time();
    utils_time(const utils_time& orig);
    ~utils_time();
};

#endif	/* TIMEUTILS_H */

