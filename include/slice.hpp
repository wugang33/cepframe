/* 
 * File:   slice.hpp
 * Author: WG
 *
 * Created on 2011-12-09 AM 10:45
 */

// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

// leveldb/include/leveldb/slice.h

#ifndef SLICE_HPP
#define	SLICE_HPP

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>
#include <stdio.h>

// #include <iostream>

namespace cep {

    class slice {
    public:
        //    Slice(const Slice& orig) {
        //        this->data_ = orig.data_;
        //        this->size_ = orig.size_;
        //    }

        /** Create an empty slice. */
        slice() : data_(""), size_(0) {
        }

        /** Create a slice that refers to d[0,n-1]. */
        slice(const char* d, size_t n) : data_(d), size_(n) {
        }

        /** Create a slice that refers to the contents of "s" */
        explicit slice(const std::string& s) : data_(s.data()), size_(s.size()) {
        }

        /** Create a slice that refers to s[0,strlen(s)-1] */
        explicit slice(const char* s) : data_(s), size_(strlen(s)) {
        }

        /** Return a pointer to the beginning of the referenced data */
        const char* data() const {
            return data_;
        }

        /** Return the length (in bytes) of the referenced data */
        size_t size() const {
            return size_;
        }

        /** Return true iff the length of the referenced data is zero */
        bool empty() const {
            return size_ == 0;
        }

        /**
         * Return the ith byte in the referenced data.
         * REQUIRES: n < size()
         * 
         * @param n
         * @return 
         */
        char operator[](size_t n) const {
            assert(n < size());
            return data_[n];
        }

        /** Change this slice to refer to an empty array */
        void clear() {
            data_ = "";
            size_ = 0;
        }

        /** Drop the first "n" bytes from this slice. */
        void remove_prefix(size_t n) {
            assert(n <= size());
            data_ += n;
            size_ -= n;
        }

        /** Return a string that contains the copy of the referenced data. */
        std::string to_string() const {
            return std::string(data_, size_);
        }

        // Three-way comparison.  Returns value:
        //   <  0 iff "*this" <  "b",
        //   == 0 iff "*this" == "b",
        //   >  0 iff "*this" >  "b"
        int compare(const slice& b) const;

        /*
         * WG add it to adapter the stl map @20111226
         * 
         * return 
        int operator<(const slice& b) const {
            // const int min_len = (size_ < b.size_) ? size_ : b.size_;
            // int r = memcmp(data_, b.data_, min_len);
            // if (r == 0) {
            //     if (size_ < b.size_) r = -1;
            //     else if (size_ > b.size_) r = +1;
            // }
            // return r;
            return compare(b); // < 0;
        }
         */

        /** Return true iff "x" is a prefix of "*this" */
        bool starts_with(const slice& x) const {
            return ((size_ >= x.size_) &&
                    (memcmp(data_, x.data_, x.size_) == 0));
        }

    private:
        friend bool operator==(const slice& lhs, const slice& rhs);
        friend bool operator<(const slice& lhs, const slice& rhs);
        friend bool operator>(const slice& lhs, const slice& rhs);

        const char* data_;
        size_t size_;

        // Intentionally copyable
    };

    inline bool operator==(const slice& x, const slice& y) {
        return ((x.size_ == y.size_) &&
                (memcmp(x.data_, y.data_, x.size_) == 0));
    }

    inline bool operator!=(const slice& x, const slice& y) {
        return !(x == y);
    }

    /** add it to free compare by WangquN @20120130 */
    inline bool operator<(const slice& lhs, const slice& rhs) {
        // return lhs.compare(rhs) < 0;
        if (lhs.size_ < rhs.size_) return true;
        else if (lhs.size_ > rhs.size_) return false;
        else return memcmp(lhs.data_, rhs.data_, lhs.size_) < 0;
    }

    /** add it to free compare by WangquN @20120130 */
    inline bool operator>(const slice& lhs, const slice& rhs) {
        // return lhs.compare(rhs) > 0;
        if (lhs.size_ > rhs.size_) return true;
        else if (lhs.size_ < rhs.size_) return false;
        else return memcmp(lhs.data_, rhs.data_, lhs.size_) > 0;
    }

    /** add it to free compare by WangquN @20120207 */
    inline bool operator>=(const slice& lhs, const slice& rhs) {
        return !(lhs < rhs);
    }

    /** add it to free compare by WangquN @20120207 */
    inline bool operator<=(const slice& lhs, const slice& rhs) {
        return !(lhs > rhs);
    }

    inline int slice::compare(const slice& b) const {
#ifdef __VER4SLICE_COMPARE_ALPHABETICAL_ORDER__
        const int min_len = (size_ < b.size_) ? size_ : b.size_;
        int r = memcmp(data_, b.data_, min_len);
        if (r == 0) {
            if (size_ < b.size_) r = -1;
            else if (size_ > b.size_) r = +1;
        }
        return r;
#else
        // std::cout << "slice:" << size_ << '<' << b.size_ << std::endl;
        if (size_ == b.size_) {
            // std::cout << "slice memcmp:" << memcmp(data_, b.data_, size_) << std::endl;
            return memcmp(data_, b.data_, size_);
        } else return size_ - b.size_;
        /*else if (size_ < b.size_)
            return -1;
        else
            return +1;*/
#endif
    }
}

#endif	/* SLICE_HPP */
