/* 
 * File:   buffer_metadata.hpp
 * Author: wugang
 *
 * Created on 2011-06-16 AM 11:09
 */

#ifndef BUFFER_METADATA_HPP
#define	BUFFER_METADATA_HPP

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined( __GNUC__ ) && ( defined( __i386__ ) || defined( __x86_64__ ) )
// // #pragma message(\"X86 CPU Architecture is supported!\")
// #warning OK, Relax! Its just an information:X86 CPU Architecture is supported.
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
// #error Unsupported CPU Architecture
#warning Unsupported __builtin_expect in current CPU Architecture
#define likely(x)	x
#define unlikely(x)	x
#endif

#include <vector>
#include <string.h>

#ifdef __VER4BUFF_METADATA_RB_MAP__
#include <map>
#elif __VER4BUFF_METADATA_UNORDER_MAP__
#include <unordered_map>
#else // __VER4BUFF_METADATA_HASH_MAP__
#include <ext/hash_map>

namespace __gnu_cxx {

    template<>struct hash<std::string> {

        size_t operator()(const std::string & str) const {
            unsigned long __h = 0;
            const char* data = str.c_str();
            for (; *data; ++data) {
                __h = __h + (__h << 2) + *data;
            }
            // for (; *data; ++data) __h = 5 * __h + *data;
            return size_t(__h);
        }
    };
}
#endif

namespace cep {

    struct field_cfg_entry {
    public:
        typedef int data_t;
        typedef int flag_t;
        typedef int idx_t;

        field_cfg_entry(const std::string& code = "", const idx_t idx = 0, const data_t type = 0,
                const data_t length = 0, const flag_t flag = 0, const data_t offset = 0)
        : offset_(offset), length_(length), type_(type), flag_(flag), idx_(idx), code_(code) {
        }
    public:

        const data_t offset() const {
            return offset_;
        }

        const data_t length() const {
            return length_;
        }

        const data_t type() const {
            return type_;
        }

        const flag_t flag() const {
            return flag_;
        }

        const idx_t idx() const {
            return idx_;
        }

        std::string code() const {
            return code_;
        }

        std::string to_string() const;
    private:
        friend class buffer_metadata;
        /* friend */ // error
        friend std::ostream& operator<<(std::ostream&, const field_cfg_entry&);

        data_t offset_;
        data_t length_;
        data_t type_;
#define __CEP_VIRTUAL_MASK__ 0x00000001
        flag_t flag_;

        idx_t idx_; // for field configuration entry sort!

        std::string code_;
    };

    /**
     * 
     */
    class buffer_metadata {
    public:
        typedef unsigned char enum_t;

        static const enum_t STR = 0;
        static const enum_t CHAR = 1;
        static const enum_t UCHAR = 2;
        static const enum_t SHORT = 3;
        static const enum_t USHORT = 4;
        static const enum_t INT = 5;
        static const enum_t UINT = 6;
        static const enum_t FLOAT = 7;
        static const enum_t LONG = 8;
        static const enum_t ULONG = 9;
        static const enum_t DOUBLE = 10;
        static const enum_t DATETIME = 11;
        static const enum_t IP = 12;
        static const enum_t MONTHTIME = 13;

        static const enum_t SIZEOF_CHAR = sizeof (char);
        static const enum_t SIZEOF_UCHAR = sizeof (unsigned char);
        static const enum_t SIZEOF_SHORT = sizeof (short);
        static const enum_t SIZEOF_USHORT = sizeof (unsigned short);
        static const enum_t SIZEOF_INT = sizeof (int);
        static const enum_t SIZEOF_UINT = sizeof (unsigned int);
        static const enum_t SIZEOF_FLOAT = sizeof (float);
        static const enum_t SIZEOF_LONG = sizeof (long);
        static const enum_t SIZEOF_ULONG = sizeof (unsigned long);
        static const enum_t SIZEOF_DOUBLE = sizeof (double);
        static const enum_t SIZEOF_DATETIME = SIZEOF_ULONG;
        static const enum_t SIZEOF_IP = SIZEOF_UINT;
        static const enum_t SIZEOF_MONTHTIME = SIZEOF_ULONG;

        typedef std::vector<field_cfg_entry*> cfg_entry_list;
#ifdef __VER4BUFF_METADATA_RB_MAP__
        typedef std::map<std::string, field_cfg_entry*> cfg_entry_map;
#elif __VER4BUFF_METADATA_UNORDER_MAP__
        typedef std::unordered_map<std::string, field_cfg_entry*> cfg_entry_map;
#else // __VER4BUFF_METADATA_HASH_MAP__
        typedef __gnu_cxx::hash_map<std::string, field_cfg_entry*> cfg_entry_map;
#endif

    public:
        buffer_metadata();
#ifdef __FLAG4INHERITABLE__
        virtual
#endif
        ~buffer_metadata();
    private: // Forbidden copy behave

        buffer_metadata(const buffer_metadata& metadata4buff) {
            // std::cout << this << "->buffer_metadata copy from[" << &metadata4buff << "] ctor execute!" << std::endl;
        }

        buffer_metadata& operator=(const buffer_metadata& metadata4buff) {
            if (this == &metadata4buff) // effective c++ 16
                return *this;
            // std::cout << this << "->buffer_metadata assignment operator(" << &metadata4buff << ") execute!" << std::endl;
            return *this;
        }
    public:
        const field_cfg_entry* get_cfg_entry(const std::string&) const;
        const field_cfg_entry* get_cfg_entry(const size_t) const;

        /** return false need to manage argument's memory */
        bool add_cfg_entry(field_cfg_entry *);
        bool rebuild();

        const cfg_entry_list& get_entry_list() const {
            return this->entry_list;
        }

        inline int size() const {
            // return this->length;
            return entry_list.size();
        }

        inline int size_of() const {
            return this->length;
        }

        inline unsigned int persistent_field_count() const {
            return persistent_field_count_;
        }

        std::string to_string() const;

        // public:
        //     unsigned long length;
    public:

        static unsigned int get_size_by_type_id(enum_t type_id, unsigned int default_size) {
            switch (type_id) {
                case CHAR:
                    return SIZEOF_CHAR;
                case UCHAR:
                    return SIZEOF_UCHAR;
                case SHORT:
                    return SIZEOF_SHORT;
                case USHORT:
                    return SIZEOF_USHORT;
                case INT:
                    return SIZEOF_INT;
                case UINT:
                    return SIZEOF_UINT;
                case FLOAT:
                    return SIZEOF_FLOAT;
                case LONG:
                    return SIZEOF_LONG;
                case ULONG:
                    return SIZEOF_ULONG;
                case DOUBLE:
                    return SIZEOF_DOUBLE;
                case DATETIME:
                    return SIZEOF_DATETIME;
                case IP:
                    return SIZEOF_IP;
                case MONTHTIME:
                    return SIZEOF_MONTHTIME;
                default:
                    return default_size;
            }
        }

#if 0

        static void get_type_by_id(std::ostream& s, enum_t type_id) {
            switch (type_id) {
                case STR:
                    s << (int) type_id << "str";
                    break;
                case CHAR:
                    s << (int) type_id << "char[" << (int) SIZEOF_CHAR << ']';
                    break;
                case UCHAR:
                    s << (int) type_id << "uchar[" << (int) SIZEOF_UCHAR << ']';
                    break;
                case SHORT:
                    s << (int) type_id << "short[" << (int) SIZEOF_SHORT << ']';
                    break;
                case USHORT:
                    s << (int) type_id << "ushort[" << (int) SIZEOF_USHORT << ']';
                    break;
                case INT:
                    s << (int) type_id << "int[" << (int) SIZEOF_INT << ']';
                    break;
                case UINT:
                    s << (int) type_id << "uint[" << (int) SIZEOF_UINT << ']';
                    break;
                case FLOAT:
                    s << (int) type_id << "float[" << (int) SIZEOF_FLOAT << ']';
                    break;
                case LONG:
                    s << (int) type_id << "long[" << (int) SIZEOF_LONG << ']';
                    break;
                case ULONG:
                    s << (int) type_id << "ulong[" << (int) SIZEOF_ULONG << ']';
                    break;
                case DOUBLE:
                    s << (int) type_id << "double[" << (int) SIZEOF_DOUBLE << ']';
                    break;
                case DATETIME:
                    s << (int) type_id << "datetime[" << (int) SIZEOF_DATETIME << ']';
                    break;
                case IP:
                    s << (int) type_id << "ip[" << (int) SIZEOF_IP << ']';
                    break;
                default:
                    s << (int) type_id << "UnsupportedType";
            }
        }
#endif

        static std::string get_type_by_id(enum_t type_id) {
            // #include <limits.h>
            // uchar:0-256                          3
            // ushort:0-32767*2+1                   5
            // uint:0-2147483647*2+1                10
            // ulong:0-9223372036854775807L*2+1     20
            switch (type_id) {
                case STR:
                {
                    char buffer[3 + 4 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:str", type_id);
                    return std::string(buffer);
                }
                case CHAR:
                {
                    char buffer[3 * 2 + 7 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:char[%d]", type_id, SIZEOF_CHAR);
                    return std::string(buffer);
                }
                case UCHAR:
                {
                    char buffer[3 * 2 + 8 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:uchar[%d]", type_id, SIZEOF_UCHAR);
                    return std::string(buffer);
                }
                case SHORT:
                {
                    char buffer[3 * 2 + 8 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:short[%d]", type_id, SIZEOF_SHORT);
                    return std::string(buffer);
                }
                case USHORT:
                {
                    char buffer[3 * 2 + 9 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:ushort[%d]", type_id, SIZEOF_USHORT);
                    return std::string(buffer);
                }
                case INT:
                {
                    char buffer[3 * 2 + 6 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:int[%d]", type_id, SIZEOF_INT);
                    return std::string(buffer);
                }
                case UINT:
                {
                    char buffer[3 * 2 + 7 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:uint[%d]", type_id, SIZEOF_UINT);
                    return std::string(buffer);
                }
                case FLOAT:
                {
                    char buffer[3 * 2 + 8 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:float[%d]", type_id, SIZEOF_FLOAT);
                    return std::string(buffer);
                }
                case LONG:
                {
                    char buffer[3 * 2 + 7 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:long[%d]", type_id, SIZEOF_LONG);
                    return std::string(buffer);
                }
                case ULONG:
                {
                    char buffer[3 * 2 + 8 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:ulong[%d]", type_id, SIZEOF_ULONG);
                    return std::string(buffer);
                }
                case DOUBLE:
                {
                    char buffer[3 * 2 + 9 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:double[%d]", type_id, SIZEOF_DOUBLE);
                    return std::string(buffer);
                }
                case DATETIME:
                {
                    char buffer[3 * 2 + 11 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:datetime[%d]", type_id, SIZEOF_DATETIME);
                    return std::string(buffer);
                }
                case IP:
                {
                    char buffer[3 * 2 + 5 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:ip[%d]", type_id, SIZEOF_IP);
                    return std::string(buffer);
                }
                case MONTHTIME:
                {
                    char buffer[3 * 2 + 11 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:mothtime[%d]", type_id, SIZEOF_MONTHTIME);
                    return std::string(buffer);
                }
                default:
                {
                    char buffer[3 + 16 + 1];
                    memset(buffer, 0, sizeof (buffer));
                    sprintf(buffer, "%d:UnsupportedType", type_id);
                    return std::string(buffer);
                }
            }
        }
    private:
        unsigned int length, persistent_field_count_;
        cfg_entry_list entry_list;
        cfg_entry_map entry_map;

        static bool sort_pred(const field_cfg_entry* left, const field_cfg_entry* right) {
            if (left->idx_ == right->idx_) return left->flag_ < right->flag_;
            else return left->idx_ < right->idx_;
        }

        friend std::ostream& operator<<(std::ostream&, const buffer_metadata&);
    };

    inline
    const field_cfg_entry* buffer_metadata::get_cfg_entry(const std::string& code) const {
        cfg_entry_map::const_iterator iter = this->entry_map.find(code);
        if (likely(iter != this->entry_map.end())) {
            return iter->second;
        } else
            return NULL;
    }

    inline
    const field_cfg_entry* buffer_metadata::get_cfg_entry(const size_t idx) const {
        if (unlikely(idx >= entry_list.size())) return NULL;
        else return entry_list[idx];
    }
} // namespace cep {

#endif	/* BUFFER_METADATA_HPP */
