/* 
 * File:   utils_nbr2str.hpp
 * Author: WG
 *
 * Created on 2012年1月31日, 上午11:09
 */

#ifndef UTILS_NBR2STR_HPP
#define	UTILS_NBR2STR_HPP
#include <string>

using namespace std;
namespace cep {

    extern string nbr2str(int nbr);

    extern string nbr2str(unsigned int nbr);

    extern string nbr2str(unsigned short nbr);

    extern string nbr2str(short nbr);

    extern string nbr2str(unsigned long nbr);

    extern string nbr2str(long nbr);

    extern string nbr2str(unsigned char nbr);

    extern string nbr2str(double nbr);

    extern string nbr2str(float nbr);
    /*
     * @param milsecond 1970年1月1日 到现在的毫秒数
     * @param format 格式字符串 默认是 "%4d-%02d-%02d %02d:%02d:%02d.%03d"\
     *  比如2011年1月1日 1点1分1秒1毫秒的输出结果 2011-01-01 01:01:01.001
     * 可以少输入格式串  比如"%4d" 那么输出年 "%4d:%2d:%2d" 输出年：月：日
     */
    extern string milsecond2str(long milsecond, const char *format = "%4d-%02d-%02d %02d:%02d:%02d.%03d");

    extern string ip2str(unsigned int ip);

    extern unsigned int str2ip(const string &, unsigned int);
    /*
     * @param date 日期字符串
     * @param format 格式字符串 默认是 "%4d-%02d-%02d %02d:%02d:%02d."\
     * 日期字符串和格式字符串必须能够匹配
     * 默认格式加小数点"%4d-%02d-%02d %02d:%02d:%02d."代表有毫秒数，需要。无小数点"%4d-%02d-%02d %02d:%02d:%02d"，舍弃小数点后毫秒数。
     * 比如"2011-12-01 12:12:12.010" 那么会输出2011年12月1日 12点12分12秒10毫秒到1970年1月1日的毫秒数
     * 可以少输入格式串  比如"%4d" 那么输出年 "%4d:%2d:%2d" 输出年：月：日 但是不能超过7个
     * 保持和数据库行为一致
     */
    extern unsigned long cstr2milsecond(const char *date, const char *format = "%4d-%02d-%02d %02d:%02d:%02d.");
}


#endif	/* UTILS_NBR2STR_HPP */

