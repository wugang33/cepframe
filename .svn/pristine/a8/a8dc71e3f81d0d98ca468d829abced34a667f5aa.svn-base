/* 
 * File:   utils_cstr2nbr.hpp
 * Author: liukai
 *
 * Created on 2011-06-21 PM 01:38
 */

#include <limits.h>
#include <string.h>
#include <ctype.h>
// #include <iostream>

#include "utils_cstr2nbr.hpp"

namespace cep {

    /**
     * 自定义结束标志判断
     * 
     * @param x
     * @return 
     */
    int islast_(int x) {
        if (x == 0)
            return 0;
        else
            return 1;
    }

    /**
     * 自定义无效字符
     * 
     * @param x
     * @return 
     */
    int isspace_(int x) {
        if (x == ' ' || x == '\t' || x == '\n' || x == '\f' || x == '\b' || x == '\r')
            return 1;
        else
            return 0;
    }

    /*
    int isdigit_(int x) {
        if (x <= '9' && x >= '0')
            return 1;
        else
            return 0;

    }
     */

    // template<typename T>
    // T cstr2nbr_(const char* cstr, T default_value) {

    long long cstr2nbr_(const char* cstr, long long default_value) {
        typedef long long T;
        if (cstr) {
            if (strlen(cstr) > 20)
                return default_value;
        } else
            return default_value;

        int c; /* current char */
        T total; /* current total */
        int sign; /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while (isspace_((int) (unsigned char) *cstr))
            ++cstr;

        c = (int) (unsigned char) *cstr++;
        sign = c; /* save sign indication */
        if (c == '-' || c == '+')
            c = (int) (unsigned char) *cstr++; /* skip sign */

        total = 0;

        while (islast_(c)) {
            // if (isdigit_(c)) {
            if (isdigit(c)) {
                total = 10 * total + (c - '0'); /* accumulate digit */
                c = (int) (unsigned char) *cstr++;
                /* get next char */
            } else {
                total = default_value;
                break;
            }
        }
        if (sign == '-')
            return -total;
        else
            return total; /* return result, negated if necessary */
    }

    bool cstrtonbr_(const char* cstr, long long& target_value) {
        typedef long long T;
        if (cstr) {
            if (strlen(cstr) > 20) return false;
            // return default_value;
        } else return false;
        // return default_value;

        int c; /* current char */
        T total; /* current total */
        int sign; /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while (isspace_((int) (unsigned char) *cstr))
            ++cstr;

        c = (int) (unsigned char) *cstr++;
        sign = c; /* save sign indication */
        if (c == '-' || c == '+')
            c = (int) (unsigned char) *cstr++; /* skip sign */

        total = 0;

        while (islast_(c)) {
            // if (isdigit_(c)) {
            if (isdigit(c)) {
                total = 10 * total + (c - '0'); /* accumulate digit */
                c = (int) (unsigned char) *cstr++;
                /* get next char */
            } else {
                // total = default_value;
                // break;
                return false;
            }
        }
        if (sign == '-') target_value = -total;
            // return -total;
        else target_value = total;
        // return total; /* return result, negated if necessary */
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////

    bool cstrtonbr(const char* cstr, char& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < CHAR_MIN || tv > CHAR_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, unsigned char& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < 0 || tv > UCHAR_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, short& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < SHRT_MIN || tv > SHRT_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, unsigned short& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < 0 || tv > USHRT_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, int& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < INT_MIN || tv > INT_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, unsigned int& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            if (tv < 0 || tv > UINT_MAX) return false;
            else {
                target_value = tv;
                return true;
            }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, long& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            // if (tv < LONG_MIN || tv > LONG_MAX) return false;
            // else {
            target_value = tv;
            return true;
            // }
        } else return false;
    }

    bool cstrtonbr(const char* cstr, unsigned long& target_value) {
        long long tv = target_value;
        if (cstrtonbr_(cstr, tv)) {
            // if (tv < 0 || tv > ULONG_MAX) return false;
            // else {
            target_value = tv;
            return true;
            // }
        } else return false;
    }

    // template<typename T2>

    bool cstrtonbr(const char* cstr, double& target_value) {
        // printf("Constructing cstr to double ...\n");
        // std::cout << "cstrtonbr(" << cstr << ", " << target_value << ')' << std::endl;
        typedef double T2;

        if (!cstr) return false;
        // return default_value;

        T2 val = (T2) 0;
        T2 power = (T2) 1;
        int i, sign;

        // for (i = 0; isspace_(cstr[i]); i++);
        for (i = 0; isspace_(cstr[i]); ++i);
        sign = (cstr[i] == '-') ? -1 : 1;

        if (cstr[i] == '+' || cstr[i] == '-') ++i;
        // i++;

        while (true) {
            // if (isdigit_(s[i])) {
            if (isdigit(cstr[i])) {
                val = 10.0 * val + (cstr[i] - '0');
                ++i;
                // i++;
            } else if (cstr[i] == '.') {
                break;
            } else if (!islast_(cstr[i])) {
                // i++;
                target_value = sign * val;
                return true;
                // return sign * val;
            } else
                return false;
        }
        if (cstr[i] == '.') ++i;
        // i++;

        while (true) {
            if (isdigit(cstr[i])) {
                val = 10.0 * val + (T2) (cstr[i] - '0');
                power *= 10.0;
                // std::cout << "value=" << val << '/' << power << std::endl;
                // i++;
                ++i;
                // } else if (cstr[i] == '\0')
            } else if (!islast_(cstr[i]))
                break;
            else
                return false;
        }
        // std::cout << "target_value=" << sign << '*' << val << '/' << power << std::endl;
        target_value = sign * val / power;
        return true;
        // return sign * val / power;
    }
    // unsigned double cstr2nbr(const char* cstr, unsigned double default_value)

    bool cstrtonbr(const char* cstr, float& target_value) {
        // printf("Constructing cstr to float ...\n");
        double dtv = target_value;
        // return cstrtonbr(cstr, (double&) target_value);
        if (cstrtonbr(cstr, dtv)) {
            target_value = dtv;
            return true;
        } else return false;
    }

    ////////////////////////////////////////////////////////////////////////////

    char cstr2nbr(const char* cstr, char default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < CHAR_MIN || result > CHAR_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    unsigned char cstr2nbr(const char* cstr, unsigned char default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < 0 || result > UCHAR_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    short cstr2nbr(const char* cstr, short default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < SHRT_MIN || result > SHRT_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    unsigned short cstr2nbr(const char* cstr, unsigned short default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < 0 || result > USHRT_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    int cstr2nbr(const char* cstr, int default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < INT_MIN || result > INT_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    unsigned int cstr2nbr(const char* cstr, unsigned int default_value) {
        long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < 0 || result > UINT_MAX) return default_value;
        else return result;
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    long cstr2nbr(const char* cstr, long default_value) {
        /*long long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < LONG_MIN || result > LONG_MAX) return default_value;
        else return result;*/
        return cstr2nbr_(cstr, default_value);
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    unsigned long cstr2nbr(const char* cstr, unsigned long default_value) {
        /*unsigned long result = cstr2nbr_(cstr, (long long) default_value);
        if (result < 0 || result > ULONG_MAX) return default_value;
        else return result;*/
        return cstr2nbr_(cstr, default_value);
        /*cstrtonbr(cstr, default_value);
        return default_value;*/
    }

    // template<typename T2>

    double cstr2nbr(const char* cstr, double default_value) {
        /*
        // printf("Constructing cstr to double ...\n");
        typedef double T2;

        if (!cstr)
            return default_value;

        T2 val = (T2) 0;
        T2 power = (T2) 1;
        int i, sign;

        for (i = 0; isspace_(cstr[i]); i++);
        sign = (cstr[i] == '-') ? -1 : 1;

        if (cstr[i] == '+' || cstr[i] == '-')
            i++;

        while (true) {
            // if (isdigit_(s[i])) {
            if (isdigit(cstr[i])) {
                val = 10.0 * val + (cstr[i] - '0');
                i++;
            } else if (cstr[i] == '.') {
                break;
            } else if (!islast_(cstr[i]))
                // i++;
                return sign * val;
            else
                return default_value;
        }
        if (cstr[i] == '.')
            i++;

        while (true) {
            if (isdigit(cstr[i])) {
                val = 10.0 * val + (cstr[i] - '0');
                power *= 10.0;
                i++;
            } else if (cstr[i] == '\0')
                break;
            else
                return default_value;
        }
        return sign * val / power;*/
        cstrtonbr(cstr, default_value);
        return default_value;
    }
    // unsigned double cstr2nbr(const char* cstr, unsigned double default_value)

    float cstr2nbr(const char* cstr, float default_value) {
        // printf("Constructing cstr to float ...\n");
        return cstr2nbr(cstr, (double) default_value);
    }
    // unsigned float cstr2nbr(const char* cstr, unsigned float default_value)
}
