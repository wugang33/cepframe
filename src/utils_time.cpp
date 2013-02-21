#include "utils_time.h"

#define	SECS_PER_HOUR	(60 * 60)
#define	SECS_PER_DAY	(SECS_PER_HOUR * 24)
#define LEAP_SECONDS_POSSIBLE 1
#define EPOCH_YEAR	1970

#define verify(name, assertion) struct name { char a[(assertion) ? 1 : -1]; }
#define INT_MAX	2147483647
#if __WORDSIZE == 64
#define LONG_MAX	9223372036854775807L
#else
#define LONG_MAX	2147483647L
#endif

#define TYPE_TWOS_COMPLEMENT(t) ((t) ~ (t) 0 == (t) -1)
#define TYPE_ONES_COMPLEMENT(t) ((t) ~ (t) 0 == 0)
#define TYPE_SIGNED_MAGNITUDE(t) ((t) ~ (t) 0 < (t) -1)

/* True if the arithmetic type T is signed.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#define TYPE_MINIMUM(t) \
  ((t) (! TYPE_SIGNED (t) \
	? (t) 0 \
	: TYPE_SIGNED_MAGNITUDE (t) \
	? ~ (t) 0 \
	: ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1)))
#define TYPE_MAXIMUM(t) \
  ((t) (! TYPE_SIGNED (t) \
	? (t) -1 \
	: ~ (~ (t) 0 << (sizeof (t) * CHAR_BIT - 1))))

#ifndef TIME_T_MIN
#define TIME_T_MIN TYPE_MINIMUM (time_t)
#endif
#ifndef TIME_T_MAX
#define TIME_T_MAX TYPE_MAXIMUM (time_t)
#endif

#define UINT_MAX	4294967295U
#define SHR(a, b)	\
  (-1 >> 1 == -1	\
   ? (a) >> (b)		\
   : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

const unsigned short int __cep__mon_yday[2][13] = {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

time_t utils_time::mktime(const struct tm* tp) {
    time_t t;
    /* The maximum number of probes (calls to CONVERT) should be enough
       to handle any combinations of time zone rule changes, solar time,
       leap seconds, and oscillations around a spring-forward gap.
       POSIX.1 prohibits leap seconds, but some hosts have them anyway.  */

    /* Time requested.  Copy it in case CONVERT modifies *TP; this can
       occur if TP is localtime's returned value and CONVERT is localtime.  */
    int sec = tp->tm_sec;
    int min = tp->tm_min;
    int hour = tp->tm_hour;
    int mday = tp->tm_mday;
    int mon = tp->tm_mon;
    int year_requested = tp->tm_year;
    /* Normalize the value.  */

    /* Ensure that mon is in range, and set year accordingly.  */
    int mon_remainder = mon % 12;
    int negative_mon_remainder = mon_remainder < 0;
    int mon_years = mon / 12 - negative_mon_remainder;
    long int lyear_requested = year_requested;
    long int year = lyear_requested + mon_years;

    /* The other values need not be in range:
       the remaining code handles minor overflows correctly,
       assuming int and time_t arithmetic wraps around.
       Major overflows are caught at the end.  */

    /* Calculate day of year from year, month, and day of month.
       The result need not be in range.  */
    int mon_yday = ((__cep__mon_yday[leapyear(year)]
            [mon_remainder + 12 * negative_mon_remainder])
            - 1);
    long int lmday = mday;
    long int yday = mon_yday + lmday;
    time_t guessed_offset = _CEPTIMEZONE*SECS_PER_HOUR;
    if (LEAP_SECONDS_POSSIBLE) {
        /* Handle out-of-range seconds specially,
           since ydhms_tm_diff assumes every minute has 60 seconds.  */
        if (sec < 0)
            sec = 0;
        if (59 < sec)
            sec = 59;
    }

    /* Invert CONVERT by probing.  First assume the same offset as last
       time.  */

    t = ydhms_diff(year, yday, hour, min, sec,
            EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, guessed_offset);

    return t;

}

#ifndef __set_errno
#define __set_errno(Val) errno = (Val)
#endif

#ifdef __CYGWIN32__
#ifndef __isleap // /usr/include/time.h
#define __isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif // __isleap
#endif // __CYGWIN32__
#ifdef _AIX
#ifndef __isleap // /usr/include/time.h
#define __isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif // __isleap
#endif // _AIX

int utils_time::localtime_r(time_t *t, struct tm*tp) {
    long int days, rem, y;
    const unsigned short int *ip;

    days = *t / SECS_PER_DAY;
    rem = *t % SECS_PER_DAY;
    rem += _CEPTIMEZONE*SECS_PER_HOUR;
    while (rem < 0) {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY) {
        rem -= SECS_PER_DAY;
        ++days;
    }
    tp->tm_hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tp->tm_min = rem / 60;
    tp->tm_sec = rem % 60;
    /* January 1, 1970 was a Thursday.  */
    tp->tm_wday = (4 + days) % 7;
    if (tp->tm_wday < 0)
        tp->tm_wday += 7;
    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (__isleap(y) ? 366 : 365)) {
        /* Guess a corrected year, assuming 365 days per year.  */
        long int yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ((yg - y) * 365
                + LEAPS_THRU_END_OF(yg - 1)
                - LEAPS_THRU_END_OF(y - 1));
        y = yg;
    }
    tp->tm_year = y - 1900;
    if (tp->tm_year != y - 1900) {
        /* The year cannot be represented due to overflow.  */
        __set_errno(EOVERFLOW);
        return 0;
    }
    tp->tm_yday = days;
    ip = __cep__mon_yday[__isleap(y)];
    for (y = 11; days < (long int) ip[y]; --y)
        continue;
    days -= ip[y];
    tp->tm_mon = y;
    tp->tm_mday = days + 1;
    return 1;
}

time_t utils_time::ydhms_diff(long int year1, long int yday1, int hour1, int min1, int sec1,
        int year0, int yday0, int hour0, int min0, int sec0) {
    verify(C99_integer_division, -1 / 2 == 0);
    verify(long_int_year_and_yday_are_wide_enough,
            INT_MAX <= LONG_MAX / 2 || TIME_T_MAX <= UINT_MAX);

    /* Compute intervening leap days correctly even if year is negative.
       Take care to avoid integer overflow here.  */
    int a4 = SHR(year1, 2) + SHR(TM_YEAR_BASE, 2) - !(year1 & 3);
    int b4 = SHR(year0, 2) + SHR(TM_YEAR_BASE, 2) - !(year0 & 3);
    int a100 = a4 / 25 - (a4 % 25 < 0);
    int b100 = b4 / 25 - (b4 % 25 < 0);
    int a400 = SHR(a100, 2);
    int b400 = SHR(b100, 2);
    int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

    /* Compute the desired time in time_t precision.  Overflow might
       occur here.  */
    time_t tyear1 = year1;
    time_t years = tyear1 - year0;
    time_t days = 365 * years + yday1 - yday0 + intervening_leap_days;
    time_t hours = 24 * days + hour1 - hour0;
    time_t minutes = 60 * hours + min1 - min0;
    time_t seconds = 60 * minutes + sec1 - sec0;
    return seconds;
}
