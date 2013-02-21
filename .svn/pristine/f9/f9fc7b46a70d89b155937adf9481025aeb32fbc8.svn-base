/* 
 * File:   buffer_utility.hpp
 * Author: wugang
 *
 * Created on 2011-06-16 AM 11:09
 */

#ifndef BUFFER_UTILITY_HPP
#define	BUFFER_UTILITY_HPP

#include<iostream>
#include<stdio.h>
#include<string>
#include<string.h>
#include "slice.hpp"
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
using namespace std;

namespace cep {

    class buff_util {
    public:
        typedef unsigned char byte;

        template<class T>
        static inline
        typename boost::enable_if< boost::is_arithmetic< T >, T>::type
        get_value(char *srcbuf, const int offset);

        static slice get_value(char *srcbuf, const int offset, const int len) {
            return slice(srcbuf + offset, len);
        }
        ////////////////////////////////////////////////////////////////////////
        template<typename T>
        static inline void set_value(char *destbuf, const int offset, const typename boost::enable_if< boost::is_arithmetic< T >, T>::type ch);

        static inline void set_value(char *destbuf, const int offset, const int len, const slice slc);
        ////////////////////////////////////////////////////////////////////////

        template<typename T>
        static void reference(char* buf, const int offset, typename boost::enable_if< boost::is_arithmetic< T >, T>::type ** t) {
            buf += offset;
            *t = (T*) buf;
        }
        ////////////////////////////////////////////////////////////////////////
#if 0

        static string to_hex_string(char *data, const int length, ostream* s = NULL) {
            char buff[4];
            if (s) {
                unsigned int row = 0;
                for (int i = 0; i < length; ++i) {
                    if (i % 16 == 0) {
                        if (i == 0) {
                            *s << '\n' << row << "\t:";
                        } else {
                            *s << ";\n" << row << "\t:";
                        }
                        ++row;
                    }
                    memset(buff, 0, sizeof (buff));
                    if ((unsigned char) data[i] < 16)
                        sprintf(buff, "0%X ", (unsigned char) data[i]);
                    else
                        sprintf(buff, "%X ", (unsigned char) data[i]);
                    *s << buff;
                }
                *s << ';';
                return "";
            } else {
                string rst = "0x[";
                for (int i = 0; i < length; ++i) {
                    memset(buff, 0, sizeof (buff));
                    if ((unsigned char) data[i] < 16)
                        sprintf(buff, "0%X ", (unsigned char) data[i]);
                    else
                        sprintf(buff, "%X ", (unsigned char) data[i]);
                    rst.append(string(buff)); // (char *)
                }
                // rst.push_back(']');
                rst[rst.size() - 1] = ']';
                return rst;
            }
        }
#endif

        static string to_hex_string(const char *data, const int length) {
            string rst = "0x[";
            char buff[4];
            for (int i = 0; i < length; ++i) {
                memset(buff, 0, sizeof (buff));
                if ((unsigned char) data[i] < 16)
                    sprintf(buff, "0%X ", (unsigned char) data[i]);
                else
                    sprintf(buff, "%X ", (unsigned char) data[i]);
                rst.append(string(buff)); // (char *)
            }
            // rst.push_back(']');
            if (length) rst[rst.size() - 1] = ']';
            else rst[rst.size() - 1] = '.';
            return rst;
        }

        static void to_hex_string(const char *data, const int length, ostream& s) {
            if (length < 1) {
                s << '\n';
                return;
            }
            char buff[4];
            unsigned int row = 0;
            for (int i = 0; i < length; ++i) {
                if (i % 16 == 0) {
                    if (i == 0) {
                        s << '\n' << row << "\t:";
                    } else
                        s << ";\n" << row << "\t:";
                    ++row;
                }
                memset(buff, 0, sizeof (buff));
                if ((unsigned char) data[i] < 16)
                    sprintf(buff, "0%X ", (unsigned char) data[i]);
                else
                    sprintf(buff, "%X ", (unsigned char) data[i]);
                s << buff;
            }
            s << ';';
        }
    };

    template<class T>
    inline
    typename boost::enable_if< boost::is_arithmetic< T >, T>::type
    buff_util::get_value(char *srcbuf, const int offset) {
        srcbuf += offset;
        return *(T*) srcbuf;
    }

    template<typename T>
    inline void buff_util::set_value(char *destbuf, const int offset,
            const typename boost::enable_if< boost::is_arithmetic< T >, T>::type data) {
        destbuf += offset;
        *((T*) destbuf) = data;
    }

    /**
     * 这个基础函数不保证长度，必须由slc的size来保证长度。正如string类型也不保证长度
     * typename boost::enable_if< boost::is_arithmetic<slice> >::type
     */
    inline void buff_util::set_value(char *destbuf, const int offset, const int len, const slice slc) {
        destbuf += offset;
        memset(destbuf, 0, len);
        memcpy(destbuf, slc.data(), slc.size() > len ? len : slc.size());
    }
}

#endif	/* BUFFER_UTILITY_HPP */
