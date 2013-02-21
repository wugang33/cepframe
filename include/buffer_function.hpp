/* 
 * File:   buffer_function.hpp
 * Author: WangquN
 *
 * Created on 2012-02-05 PM 01:01
 */

#ifndef BUFFER_FUNCTION_HPP
#define	BUFFER_FUNCTION_HPP

#include <cassert>
#include <ostream>

#include "slice.hpp"
#include "buffer_metadata.hpp"
#include "buffer_utility.hpp"
#include "utils_cstr2nbr.hpp"

using std::ostream;

namespace cep {
    // class slice;

    struct type_slice {
        buffer_metadata::enum_t type;
        slice data; // default is STR[0]

        friend ostream& operator<<(ostream&, const type_slice&);
    };

    class logic_function {
    private:
        template<typename T>
        inline static bool equal_to(const slice& x, const slice& y);
        template<typename T>
        inline static bool not_equal_to(const slice& x, const slice& y);
        template<typename T>
        inline static bool greater(const slice& x, const slice& y);
        template<typename T>
        inline static bool less(const slice& x, const slice& y);
        template<typename T>
        inline static bool greater_equal(const slice& x, const slice& y);
        template<typename T>
        inline static bool less_equal(const slice& x, const slice& y);
        template<typename T>
        inline static bool bitwise_and(const slice& x, const slice& y);
    public:
        typedef unsigned char enum_t;

        static const enum_t EQUAL_TO = 0;
        static const enum_t NOT_EQUAL_TO = 1;
        static const enum_t GREATER = 2;
        static const enum_t LESS = 3;
        static const enum_t GREATER_EQUAL = 4;
        static const enum_t LESS_EQUAL = 5;
        static const enum_t BITWISE_AND = 6;

        inline static bool equal_to(const type_slice&, const type_slice&);

        inline static bool not_equal_to(const type_slice& x, const type_slice& y);

        inline static bool greater(const type_slice& x, const type_slice& y);

        inline static bool less(const type_slice& x, const type_slice& y);

        inline static bool greater_equal(const type_slice& x, const type_slice& y);

        inline static bool less_equal(const type_slice& x, const type_slice& y);

        inline static bool bitwise_and(const type_slice& x, const type_slice& y);
    public:

        static bool call(enum_t logic_func, const type_slice& x, const type_slice& y) {
            static bool (*__ops[BITWISE_AND + 1])(const type_slice&, const type_slice&) = {
                logic_function::equal_to, logic_function::not_equal_to,
                logic_function::greater, logic_function::less,
                logic_function::greater_equal, logic_function::less_equal,
                logic_function::bitwise_and
            };
            assert(logic_func <= BITWISE_AND);
            return __ops[logic_func](x, y);
        }
    };

    struct less : public std::less<type_slice> {

        bool operator()(const type_slice& lhs, const type_slice & rhs) const {
            return logic_function::call(logic_function::LESS, lhs, rhs);
        }
    };

    class utils_function {
    private:

        template<typename T>
        static size_t cstr2nbr(const char* cstr, char* destbuf) {
            buff_util::set_value<T > (destbuf, 0, cep::cstr2nbr(cstr, (T) 0));
            return sizeof (T);
        }

        template<typename T>
        static bool cstrtonbr(const char* cstr, char* destbuf, size_t& size) {
            T v;
            if (cep::cstrtonbr(cstr, v)) {
                buff_util::set_value<T > (destbuf, 0, v);
                size = sizeof (T);
                return true;
            } else return false;
        }
    public:

        static size_t cstr2nbr(buffer_metadata::enum_t desttype, const char* cstr, char* destbuf) {
            static size_t(*__ops[buffer_metadata::DOUBLE])(const char* cstr, char* destbuf) = {
                utils_function::cstr2nbr<char>, utils_function::cstr2nbr<unsigned char>,
                utils_function::cstr2nbr<short>, utils_function::cstr2nbr<unsigned short>,
                utils_function::cstr2nbr<int>, utils_function::cstr2nbr<unsigned int>,
                utils_function::cstr2nbr<float>, utils_function::cstr2nbr<long>,
                utils_function::cstr2nbr<unsigned long>, utils_function::cstr2nbr<double>
            };
            if (desttype <= buffer_metadata::STR || desttype > buffer_metadata::DOUBLE
                    || cstr == NULL || destbuf == NULL)
                return 0;
            return __ops[desttype - 1](cstr, destbuf);
        }

        static bool cstrtonbr(buffer_metadata::enum_t desttype, const char* cstr, char* destbuf, size_t& size) {
            static bool (*__ops[buffer_metadata::DOUBLE])(const char* cstr, char* destbuf, size_t&) = {
                utils_function::cstrtonbr<char>, utils_function::cstrtonbr<unsigned char>,
                utils_function::cstrtonbr<short>, utils_function::cstrtonbr<unsigned short>,
                utils_function::cstrtonbr<int>, utils_function::cstrtonbr<unsigned int>,
                utils_function::cstrtonbr<float>, utils_function::cstrtonbr<long>,
                utils_function::cstrtonbr<unsigned long>, utils_function::cstrtonbr<double>
            };
            if (desttype <= buffer_metadata::STR || desttype > buffer_metadata::DOUBLE
                    || cstr == NULL || destbuf == NULL)
                return false;
            return __ops[desttype - 1](cstr, destbuf, size);
        }

        static size_t padding_length(size_t current_len, buffer_metadata::enum_t type) {
            switch (type) {
                case buffer_metadata::SHORT:
                case buffer_metadata::USHORT:
                {
                    unsigned int align_len = current_len % buffer_metadata::SIZEOF_SHORT;
                    if (align_len != 0) {
                        align_len = buffer_metadata::SIZEOF_SHORT - align_len;
                    }
                    return align_len;
                }
                case buffer_metadata::INT:
                case buffer_metadata::UINT:
                case buffer_metadata::IP:
                {
                    unsigned int align_len = current_len % buffer_metadata::SIZEOF_INT;
                    if (align_len != 0) {
                        align_len = buffer_metadata::SIZEOF_INT - align_len;
                    }
                    return align_len;
                }
                case buffer_metadata::FLOAT:
                {
                    unsigned int align_len = current_len % buffer_metadata::SIZEOF_FLOAT;
                    if (align_len != 0) {
                        align_len = buffer_metadata::SIZEOF_FLOAT - align_len;
                    }
                    return align_len;
                }
                case buffer_metadata::LONG:
                case buffer_metadata::ULONG:
                case buffer_metadata::DATETIME:
                case buffer_metadata::MONTHTIME:
                case buffer_metadata::DOUBLE:
                {
                    unsigned int align_len = current_len % buffer_metadata::SIZEOF_LONG;
                    if (align_len != 0) {
                        align_len = buffer_metadata::SIZEOF_LONG - align_len;
                    }
                    return align_len;
                }
                default:
                    return 0;
            }
        }
    };
    ////////////////////////////////////////////////////////////////////////////

    template<typename T>
    inline bool logic_function::equal_to(const slice& x, const slice& y) {
        return *((T*) x.data()) == *((T*) y.data());
    }

    template<>
    inline bool logic_function::equal_to<slice>(const slice& x, const slice& y) {
        // return x == y;
        slice x1(x.data(), strlen(x.data()) > x.size() ? x.size() : strlen(x.data()));
        slice y1(y.data(), strlen(y.data()) > y.size() ? y.size() : strlen(y.data()));
        return x1 == y1;
    }

    inline bool logic_function::equal_to(const type_slice& x, const type_slice& y) {
        //        static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            // write in the class body complier error: specialization of
            // ‘static bool cep::logic_function::equal_to(const cep::slice&, const cep::slice&)
            //   [with T = cep::slice]’ after instantiation
            logic_function::equal_to<slice>, logic_function::equal_to<char>,
            logic_function::equal_to<unsigned char>, logic_function::equal_to<short>,
            logic_function::equal_to<unsigned short>, logic_function::equal_to<int>,
            logic_function::equal_to<unsigned int>, logic_function::equal_to<float>,
            logic_function::equal_to<long>, logic_function::equal_to<unsigned long>,
            logic_function::equal_to<double>, logic_function::equal_to<unsigned long>, // datetime
            logic_function::equal_to<unsigned int>, // ip
            logic_function::equal_to<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::not_equal_to(const slice& x, const slice& y) {
        return *((T*) x.data()) != *((T*) y.data());
    }

    template<>
    inline bool logic_function::not_equal_to<slice>(const slice& x, const slice& y) {
        // return x != y;
        slice x1(x.data(), strlen(x.data()) > x.size() ? x.size() : strlen(x.data()));
        slice y1(y.data(), strlen(y.data()) > y.size() ? y.size() : strlen(y.data()));
        return x1 != y1;
    }

    inline bool logic_function::not_equal_to(const type_slice& x, const type_slice& y) {
        //        static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::not_equal_to<slice>, logic_function::not_equal_to<char>,
            logic_function::not_equal_to<unsigned char>, logic_function::not_equal_to<short>,
            logic_function::not_equal_to<unsigned short>, logic_function::not_equal_to<int>,
            logic_function::not_equal_to<unsigned int>, logic_function::not_equal_to<float>,
            logic_function::not_equal_to<long>, logic_function::not_equal_to<unsigned long>,
            logic_function::not_equal_to<double>, logic_function::not_equal_to<unsigned long>, // datetime
            logic_function::not_equal_to<unsigned int>, // ip
            logic_function::not_equal_to<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::greater(const slice& x, const slice& y) {
        return *((T*) x.data()) > *((T*) y.data());
    }

    template<>
    inline bool logic_function::greater<slice>(const slice& x, const slice& y) {
        return x > y;
    }

    inline bool logic_function::greater(const type_slice& x, const type_slice& y) {
        //        static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::greater<slice>, logic_function::greater<char>,
            logic_function::greater<unsigned char>, logic_function::greater<short>,
            logic_function::greater<unsigned short>, logic_function::greater<int>,
            logic_function::greater<unsigned int>, logic_function::greater<float>,
            logic_function::greater<long>, logic_function::greater<unsigned long>,
            logic_function::greater<double>, logic_function::greater<unsigned long>, // datetime
            logic_function::greater<unsigned int>, // ip
            logic_function::greater<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::less(const slice& x, const slice& y) {
        return *((T*) x.data()) < *((T*) y.data());
    }

    template<>
    inline bool logic_function::less<slice>(const slice& x, const slice& y) {
        // return x < y;
        bool rst = x < y;
        // cout << "slice less:" << buff_util::to_hex_string(x.data(), x.size())
        //         << '<' << buff_util::to_hex_string(y.data(), y.size()) << '='
        //         << (rst ? "true" : "false") << endl;
        return rst;
    }

    inline bool logic_function::less(const type_slice& x, const type_slice& y) {
        //  static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::less<slice>, logic_function::less<char>,
            logic_function::less<unsigned char>, logic_function::less<short>,
            logic_function::less<unsigned short>, logic_function::less<int>,
            logic_function::less<unsigned int>, logic_function::less<float>,
            logic_function::less<long>, logic_function::less<unsigned long>,
            logic_function::less<double>, logic_function::less<unsigned long>, // datetime
            logic_function::less<unsigned int>, // ip
            logic_function::less<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::greater_equal(const slice& x, const slice& y) {
        return *((T*) x.data()) >= *((T*) y.data());
    }

    template<>
    inline bool logic_function::greater_equal<slice>(const slice& x, const slice& y) {
        return x >= y;
    }

    inline bool logic_function::greater_equal(const type_slice& x, const type_slice& y) {
        //        static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::greater_equal<slice>, logic_function::greater_equal<char>,
            logic_function::greater_equal<unsigned char>, logic_function::greater_equal<short>,
            logic_function::greater_equal<unsigned short>, logic_function::greater_equal<int>,
            logic_function::greater_equal<unsigned int>, logic_function::greater_equal<float>,
            logic_function::greater_equal<long>, logic_function::greater_equal<unsigned long>,
            logic_function::greater_equal<double>, logic_function::greater_equal<unsigned long>, // datetime
            logic_function::greater_equal<unsigned int>, // ip
            logic_function::greater_equal<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::less_equal(const slice& x, const slice& y) {
        return *((T*) x.data()) <= *((T*) y.data());
    }

    template<>
    inline bool logic_function::less_equal<slice>(const slice& x, const slice& y) {
        return x <= y;
    }

    inline bool logic_function::less_equal(const type_slice& x, const type_slice& y) {
        //        static bool (*__ops[buffer_metadata::IP + 1])(const slice&, const slice&) = {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::less_equal<slice>, logic_function::less_equal<char>,
            logic_function::less_equal<unsigned char>, logic_function::less_equal<short>,
            logic_function::less_equal<unsigned short>, logic_function::less_equal<int>,
            logic_function::less_equal<unsigned int>, logic_function::less_equal<float>,
            logic_function::less_equal<long>, logic_function::less_equal<unsigned long>,
            logic_function::less_equal<double>, logic_function::less_equal<unsigned long>, // datetime
            logic_function::less_equal<unsigned int>, // ip
            logic_function::less_equal<unsigned long> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    template<typename T>
    inline bool logic_function::bitwise_and(const slice& x, const slice& y) {
        return ((*((T*) x.data())) & (*((T*) y.data()))) != 0;
    }

    template<>
    inline bool logic_function::bitwise_and<float>(const slice& x, const slice& y) {
        // error: invalid operands of types ‘float’ and ‘float’ to binary ‘operator&’
        // float __x = *((float*) x.data()), __y = *((float*) y.data());
        // return (__x & __y) != 0;
        return false;
    }

    template<>
    inline bool logic_function::bitwise_and<double>(const slice& x, const slice& y) {
        // error: invalid operands of types ‘double’ and ‘double’ to binary ‘operator&’
        // double __x = *((double*) x.data()), __y = *((double*) y.data());
        // return (__x & __y) != 0;
        return false;
    }

    template<>
    inline bool logic_function::bitwise_and<slice>(const slice& x, const slice& y) {
        const char* xdata = x.data();
        const char* ydata = y.data();
        for (int i = x.size() - 1; i >= 0; --i) {
            if ((xdata[i] & ydata[i]) != 0) return true;
        }
        return false;
    }

    inline bool logic_function::bitwise_and(const type_slice& x, const type_slice& y) {
        static bool (*__ops[buffer_metadata::MONTHTIME + 1])(const slice&, const slice&) = {
            logic_function::bitwise_and<slice>, logic_function::bitwise_and<char>,
            logic_function::bitwise_and<unsigned char>, logic_function::bitwise_and<short>,
            logic_function::bitwise_and<unsigned short>, logic_function::bitwise_and<int>,
            logic_function::bitwise_and<unsigned int>, logic_function::bitwise_and<float>,
            logic_function::bitwise_and<long>, logic_function::bitwise_and<unsigned long>,
            logic_function::bitwise_and<double>, logic_function::bitwise_and<unsigned long>, // datetime
            logic_function::bitwise_and<unsigned int>, // ip
            logic_function::bitwise_and<unsigned int> // MONTHTIME
        };
        assert(x.type == y.type);
        assert(x.type <= buffer_metadata::MONTHTIME);
        return __ops[x.type](x.data, y.data);
    }

    ////////////////////////////////////////////////////////////////////////////
    // add it to free compare by WangquN @20120316 /////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    inline bool operator==(const type_slice& x, const type_slice& y) {
        return logic_function::equal_to(x, y);
    }

    inline bool operator!=(const type_slice& x, const type_slice& y) {
        return logic_function::not_equal_to(x, y);
    }

    inline bool operator<(const type_slice& lhs, const type_slice& rhs) {
        return logic_function::less(lhs, rhs);
    }

    inline bool operator>(const type_slice& lhs, const type_slice& rhs) {
        return logic_function::greater(lhs, rhs);
    }

    inline bool operator>=(const type_slice& lhs, const type_slice& rhs) {
        return logic_function::greater_equal(lhs, rhs);
    }

    inline bool operator<=(const type_slice& lhs, const type_slice& rhs) {
        return logic_function::less_equal(lhs, rhs);
    }
}

#endif	/* BUFFER_FUNCTION_HPP */
