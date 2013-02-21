/* 
 * File:   utils_nbr2str.cpp
 * Author: WG
 *
 * Created on 2012-01-31 PM 03:21
 */
#include <string.h>
#ifdef __CYGWIN32__
#include <time.h>
#endif

#include "utils_nbr2str.hpp"
#include "utils_cstr2nbr.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "utils_time.h"
#include <assert.h>
namespace cep {

    string nbr2str(int nbr) {
        char value[16] = {0};
        sprintf(value, "%d", nbr);
        return string(value);
    }

    string nbr2str(unsigned int nbr) {
        char value[16] = {0};
        sprintf(value, "%u", nbr);
        return string(value);
    }

    string nbr2str(unsigned short nbr) {
        char value[16] = {0};
        sprintf(value, "%hu", nbr);
        return string(value);
    }

    string nbr2str(short nbr) {
        char value[16] = {0};
        sprintf(value, "%hd", nbr);
        return string(value);
    }

    string nbr2str(unsigned long nbr) {
        char value[32] = {0};
        sprintf(value, "%lu", nbr);
        return string(value);
    }

    string nbr2str(long nbr) {
        char value[32] = {0};
        sprintf(value, "%ld", nbr);
        return string(value);
    }

    string nbr2str(unsigned char nbr) {
        char value[32] = {0};
        sprintf(value, "%d", nbr);
        return string(value);
    }

    string nbr2str(double nbr) {
        char value[32] = {0};
        sprintf(value, "%lf", nbr);
        return string(value);
    }

    string nbr2str(float nbr) {
        char value[16] = {0};
        sprintf(value, "%f", nbr);
        return string(value);
    }

    /*
     * @param milsecond 1970年1月1日 到现在的毫秒数
     * @param format 格式字符串 默认是 "%4d-%02d-%02d %02d:%02d:%02d.%03d"\
     *  比如2011年1月1日 1点1分1秒1毫秒的输出结果 2011-01-01 01:01:01.001
     * 可以少输入格式串  比如"%4d" 那么输出年 "%4d:%2d:%2d" 输出年：月：日
     */
    string milsecond2str(long milsecond, const char *format) {
        unsigned int mils = milsecond % 1000;
        long second = milsecond / 1000;
        struct tm ptm;
        ptm.tm_year = 0;
        ptm.tm_mon = 0;
        ptm.tm_mday = 0;
        ptm.tm_hour = 0;
        ptm.tm_min = 0;
        ptm.tm_sec = 0;
        utils_time::localtime_r(&second, &ptm);
        char buffer[32] = {0};
        sprintf(buffer, format, ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, mils);
        return string(buffer);
    }

    /*
     * @param date 日期字符串
     * @param format 格式字符串 默认是 "%4d-%02d-%02d %02d:%02d:%02d."\
     * 默认格式加小数点代表有毫秒数，需要。无小数点，舍弃小数点后毫秒数。
     * 日期字符串和格式字符串必须能够匹配
     * 比如"2011-12-01 12:12:12.010" 那么会输出2011年12月1日 12点12分12秒10毫秒到1970年1月1日的毫秒数
     * 可以少输入格式串  比如"%4d" 那么输出年 "%4d:%2d:%2d" 输出年：月：日
     * 保持和数据库行为一致
     */
    unsigned long cstr2milsecond(const char *date, const char *format) {
        assert(date != 0 && format != 0);
        char point[4];
        const char * p = NULL;
        memset(point, '0', sizeof (point));
        point[3] = '\0';
        struct tm stt;
        memset(&stt, 0, sizeof (stt));
        unsigned int milsecond = 0;
        sscanf(date, format, &stt.tm_year, &stt.tm_mon, &stt.tm_mday,
                &stt.tm_hour, &stt.tm_min, &stt.tm_sec);
        stt.tm_year -= 1900;
        stt.tm_mon -= 1;
        long rt = utils_time::mktime(&stt)*1000;
        p = strstr(format, ".");
        if (!p)
            return rt;
        p = strstr(date, ".");
        if (p)
            ++p;
        else
            return rt;
        if (p) {
            for (int i = 0; i < 3; ++i) {
                if (*p != '\0') {
                    point[i] = *p;
                } else {
                    break;
                }
                ++p;
            }
        }
        milsecond = atoi(point);
        return rt + milsecond;
    }

    string ip2str(unsigned int ip) {
        string ip_str = "";
        unsigned int ip_mask = 0xFF000000;
        for (int i = 3; i >= 0; i--) {
            char buf[8] = {0};
            unsigned int tmp = (ip & ip_mask) >> (i * 8);
            ip_mask = ip_mask >> 8;
            if (i != 0) {
                sprintf(buf, "%u.", tmp);
            } else {
                sprintf(buf, "%u", tmp);
            }
            ip_str.append(buf);
        }
        return ip_str;
    }

    unsigned int str2ip(const string &ip, unsigned int default_val) { // 192.168.1.1->0xC0A80101
        string::const_iterator iter; //O(n)
        unsigned int uint_ip = 0;
        int lea_index = 24;
        string temp_val = "";
        for (iter = ip.begin(); iter != ip.end(); iter++) {
            if (*iter >= 0x30 && *iter <= 0x39) {//if is number 0-10
                temp_val.push_back(*iter);
            }
            if (*iter == 0x2E) {//if find '.'
                unsigned int temp_int = (unsigned int) cep::cstr2nbr(temp_val.c_str(), 0);
                if (temp_int < 0 || temp_int > 255) {//value out of range
                    return default_val;
                }
                if (lea_index < 8)return default_val; //if '.' is too many
                temp_int = temp_int << lea_index;
                lea_index -= 8; //move  bit index
                uint_ip = uint_ip | temp_int; //put temp_int  in the return val
                temp_val = "";
            }
            if ((iter + 1) == ip.end()) {//if the end of char
                if (lea_index != 0)return default_val; //too much '.'
                unsigned int temp_int = (unsigned int) cep::cstr2nbr(temp_val.c_str(), 0);
                if (temp_int < 0 || temp_int > 255) {
                    return default_val;
                }
                uint_ip = uint_ip | temp_int;
                return uint_ip;
            }
        }
        return uint_ip;
    }

}
