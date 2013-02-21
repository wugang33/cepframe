/* 
 * 
 * char* -> short, unsigned short, int, unsigned int, long, unsigned long
 * char, unsigned char 为数字也可使用
 * 其中：*cstr 字符串，default_value 转换出错默认值
 * 工具认为：
 * 最大、最小值判断同库函数一致，不一致则不合法
 * 输入字符串中间有空格为不合法
 * 数字字符串中间存在非数字字符，不合法
 * 
 * 注意：
 * 字符串结束标志为'\0'
 * 字符串长度不超过20位，可正确安全转换。但对于浮点型存贮长度超过一定长度，输出截断值
 * 
 * File:   utils_cstr2nbr.hpp
 * Author: liukai
 *
 * Created on 2011-06-21 PM 01:38
 */

#ifndef UTILS_CSTR2NBR_HPP
#define	UTILS_CSTR2NBR_HPP

namespace cep {
    extern bool cstrtonbr(const char* cstr, char& target_value);
    extern bool cstrtonbr(const char* cstr, unsigned char& target_value);

    extern bool cstrtonbr(const char* cstr, short& target_value);
    extern bool cstrtonbr(const char* cstr, unsigned short& target_value);

    extern bool cstrtonbr(const char* cstr, int& target_value);
    extern bool cstrtonbr(const char* cstr, unsigned int& target_value);

    extern bool cstrtonbr(const char* cstr, long& target_value);
    extern bool cstrtonbr(const char* cstr, unsigned long& target_value);

    extern bool cstrtonbr(const char* cstr, float& target_value);

    extern bool cstrtonbr(const char* cstr, double& target_value);

    ////////////////////////////////////////////////////////////////////////////

    extern char cstr2nbr(const char* cstr, char default_value);
    extern unsigned char cstr2nbr(const char* cstr, unsigned char default_value);

    extern short cstr2nbr(const char* cstr, short default_value);
    extern unsigned short cstr2nbr(const char* cstr, unsigned short default_value);

    extern int cstr2nbr(const char* cstr, int default_value);
    extern unsigned int cstr2nbr(const char* cstr, unsigned int default_value);

    extern long cstr2nbr(const char* cstr, long default_value);
    extern unsigned long cstr2nbr(const char* cstr, unsigned long default_value);

    extern float cstr2nbr(const char* cstr, float default_value);
    // extern unsigned float cstr2nbr(const char* cstr, unsigned float default_value);

    extern double cstr2nbr(const char* cstr, double default_value);
    // extern unsigned double cstr2nbr(const char* cstr, unsigned double default_value);
}

#endif	/* UTILS_CSTR2NBR_HPP */
