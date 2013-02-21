/* 
 * File:   dynamic_struct.hpp
 * Author: WG
 *
 * Created on 2011-12-08 AM 10:22
 */

#ifndef DYNAMIC_STRUCT_HPP
#define	DYNAMIC_STRUCT_HPP

#include <string>
#include <string.h>
#include <stdlib.h>

#include "buffer_utility.hpp"
#include "buffer_metadata.hpp"

using namespace std;

namespace cep {

    /**
     * It just care the data and it's meta data
     * 其实我关心的就是原数据和数据本身
     */
    class dynamic_struct {
    public:
        typedef unsigned int type_t;

        dynamic_struct() : type_(0), metadata_(0), data_(0), length_(0) {
        }

        dynamic_struct(const buffer_metadata* metadata);

        dynamic_struct(const buffer_metadata* metadata, unsigned int type);

        dynamic_struct(char* data, unsigned int len)
        : type_(0), metadata_(0), data_(data), length_(len) {
        }

        dynamic_struct(char* data, unsigned int len, unsigned int type)
        : type_(type), metadata_(0), data_(data), length_(len) {
        }

        dynamic_struct(char* data, unsigned int len, unsigned int type, buffer_metadata* metadata) : type_(type), metadata_(metadata), data_(data), length_(len) {
        }

        ~dynamic_struct();
    public:
        dynamic_struct(const dynamic_struct& orig);

        dynamic_struct& operator=(const dynamic_struct& orig);
    public:
        /**
         * 可以用来获取数字型的值和字符型的值
         * 比如一个body中int型的123，那么调用这个这个函数返回字符串"123"
         * value 和 set_value函数只支持 内建数据类型和slice类型
         * @param name
         * @param slice
         * @return 
         */
        template<typename T>
        inline T value(const std::string& name, const T default_value) const;
        template<typename T>
        inline T value(const size_t index, const T default_value) const;
        /*
         * @Deprecated
         */
        template<typename T>
        inline T value(const size_t offset, const size_t len, const T default_value)const;
        ////////////////////////////////////////////////////////////////////////
        template<typename T>
        inline void set_value(const std::string& name, T value);
        template<typename T>
        inline void set_value(const size_t index, T value);
        /*
         * @Deprecated
         */
        template<typename T>
        inline void set_value(const size_t offset, const size_t len, T value);
        ////////////////////////////////////////////////////////////////////////
        template<typename T>
        void reference(const std::string& name, T** value) const;

        template<typename T>
        void reference(const size_t index, T** value) const;
        /*
         * @Deprecated
         */
        template<typename T>
        void reference(const size_t offset, const size_t len, T** value) const;
        ////////////////////////////////////////////////////////////////////////

        inline const char * data()const {
            return this->data_;
        }

        inline const buffer_metadata * metadata()const {
            return this->metadata_;
        }

        inline unsigned int len() const {
            return this->length_;
        }

        type_t type() const {
            return type_;
        }

        inline void set_data(char * data, unsigned int len) {
            this->data_ = data;
            this->length_ = len;
        }

        void set_metadata(buffer_metadata * metaData) {
            this->metadata_ = metaData;
        }

        type_t set_type(type_t type) {
            type_ = type;
        }

    private: // used 24 Bytes
        char* data_;
        buffer_metadata* metadata_;
        unsigned int length_;
        type_t type_;

        friend ostream& operator<<(ostream& s, const dynamic_struct& r);
#if 0 // Runtime core dumped
        {
            s << &r << "::length=" << r.length_ << ", type=" << r.type_
                    << ", metadata=\n";
            if (r.metadata_) s << *(r.metadata_);
            else s << "NULL";
            s << "\ndata="; // Segmentation fault (core dumped)
            if (r.data_) buff_util::to_hex_string(r.data_, r.length_, s);
            else s << "NULL";
        }
#endif
    };

    ////////////////////////////////////////////////////////////////////////////
    // Member function implements!!! ///////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    inline dynamic_struct::dynamic_struct(const buffer_metadata* metadata)
    : metadata_(const_cast<buffer_metadata*> (metadata)), type_(0)
    , /* rowData_(0), */ length_(metadata->size_of()) {
        // this->data_ = (char*) malloc(length_);
        data_ = new char[length_];
        memset(data_, 0, length_);
    }

    inline dynamic_struct::dynamic_struct(const buffer_metadata* metadata, type_t type)
    : metadata_(const_cast<buffer_metadata*> (metadata)), type_(type)
    , data_(0), length_(metadata->size_of()) {
        // this->data_ = (char*) malloc(length_);
        data_ = new char[length_];
        memset(data_, 0, length_);
    }

    inline dynamic_struct::~dynamic_struct() {
        // free(this->data_);
        delete[] data_;
        // cout << this << "->dynamic_struct dtor executed!" << endl;
    }
    ////////////////////////////////////////////////////////////////////////////

    inline dynamic_struct::dynamic_struct(const dynamic_struct& orig) {
        this->metadata_ = orig.metadata_;
        this->type_ = orig.type_;
        this->length_ = orig.length_;
        // this->data_ = (char*) malloc(orig.length_);
        data_ = new char[orig.length_];
        memcpy(this->data_, orig.data_, orig.length_);
    }

    inline dynamic_struct& dynamic_struct::operator=(const dynamic_struct& orig) {
        if (&orig == this) return *this;
        this->metadata_ = orig.metadata_;
        if (likely(this->data_ != NULL)) {
            // free(this->data_);
            delete[] data_;
        }
        // this->data_ = (char*) malloc(orig.length_);
        data_ = new char[orig.length_];
        memcpy(this->data_, orig.data_, orig.length_);
        this->type_ = orig.type_;
        this->length_ = orig.length_;
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////

    template<typename T>
    inline T dynamic_struct::value(const size_t index, const T default_value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(index);
        if (likely(cfg_entry != NULL)) {
            return buff_util::get_value<T > (this->data_, cfg_entry->offset());
        } else {
            return default_value;
        }
    }

    template<>
    inline slice dynamic_struct::value<slice>(const size_t index, const slice default_value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(index);
        if (likely(cfg_entry != NULL)) {
            return buff_util::get_value(this->data_, cfg_entry->offset(), cfg_entry->length());
        } else {
            return default_value;
        }
    }

    template<typename T>
    inline T dynamic_struct::value(const std::string& name, const T default_value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
        if (likely(cfg_entry != NULL)) {
            return buff_util::get_value<T > (this->data_, cfg_entry->offset());
        } else {
            return default_value;
        }
    }

    template<>
    inline slice dynamic_struct::value<slice>(const std::string& name, const slice default_value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
        if (likely(cfg_entry != NULL)) {
            return buff_util::get_value(this->data_, cfg_entry->offset(), cfg_entry->length());
        } else {
            return default_value;
        }
    }

    template<typename T>
    inline T dynamic_struct::value(const size_t offset, const size_t len, const T default_value) const {
        return buff_util::get_value<T > (this->data_, offset);
    }

    template<>
    inline slice dynamic_struct::value<slice>(const size_t offset, const size_t len, const slice default_value) const {
        return buff_util::get_value(this->data_, offset, len);
    }

    template<typename T>
    inline void dynamic_struct::set_value(const std::string& name, T value) {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
        if (likely(cfg_entry != NULL)) {
            buff_util::set_value<T > (this->data_, cfg_entry->offset(), value);
        }
    }

    template<>
    inline void dynamic_struct::set_value(const std::string& name, slice value) {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
        if (likely(cfg_entry != NULL)) {
            buff_util::set_value(this->data_, cfg_entry->offset(), cfg_entry->length(), value);
        }
    }

    template<typename T>
    inline void dynamic_struct::set_value(const size_t index, T value) {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(index);
        if (likely(cfg_entry != NULL)) {
            buff_util::set_value<T > (this->data_, cfg_entry->offset(), value);
        }
    }

    template<>
    inline void dynamic_struct::set_value<slice>(const size_t index, slice value) {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(index);
        if (likely(cfg_entry != NULL)) {
            buff_util::set_value(this->data_, cfg_entry->offset(), cfg_entry->length(), value);
        }
    }

    template<typename T>
    inline void dynamic_struct::set_value(const size_t offset, const size_t len, T value) {
        buff_util::set_value<T > (this->data_, offset, value);
    }

    template<>
    inline void dynamic_struct::set_value<slice>(const size_t offset, const size_t len, slice value) {
        buff_util::set_value(this->data_, offset, len, value);
    }
    //    inline slice dynamic_struct::reference(const std::string& name, const slice& default_value) const {
    //        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
    //        // if there is no configuration  put the value into the temp valueue map
    //        if (likely(cfg_entry != NULL)) {
    //            return NULL;
    //        }
    //    }

    template<typename T>
    inline void dynamic_struct::reference(const std::string& name, T** value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(name);
        if (likely(cfg_entry != NULL)) {
            buff_util::reference<T > (data_, cfg_entry->offset(), value);
        }
    }

    template<typename T>
    inline void dynamic_struct::reference(const size_t index, T** value) const {
        const field_cfg_entry* cfg_entry = this->metadata_->get_cfg_entry(index);
        if (likely(cfg_entry != NULL)) {
            buff_util::reference<T > (data_, cfg_entry->offset(), value);
        }
    }

    template<typename T>
    inline void dynamic_struct::reference(const size_t offset, const size_t len, T** value) const {
        buff_util::reference<T > (data_, offset, value);
    }
    ////////////////////////////////////////////////////////////////////////////
#if 0 // error: multiple definition

    ostream& operator<<(ostream& s, const dynamic_struct& r) {
        s << &r << "::length=" << r.length_ << ", type=" << r.type_
                << ", metadata=\n";
        if (r.metadata_) s << *(r.metadata_);
        else s << "NULL";
        s << "\ndata=";
        if (r.data_) buff_util::to_hex_string(r.data_, r.length_, s);
        else s << "NULL";
    }
#endif

    inline bool operator==(const dynamic_struct& x, const dynamic_struct& y) {
        return ((x.len() == y.len()) && (memcmp(x.data(), y.data(), x.len()) == 0));
    }

    inline bool operator!=(const dynamic_struct& x, const dynamic_struct& y) {
        return !(x == y);
    }
}

#endif	/* DYNAMIC_STRUCT_HPP */
