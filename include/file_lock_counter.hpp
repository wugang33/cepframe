/* 
 * Here, high availability is more important than performance!
 * High performance version, please see unitest_main case A.
 * 
 * File:   file_lock_counter.hpp
 * Author: WangquN
 *
 * Created on 2012-06-18 PM 07:28
 */

#ifndef FILE_LOCK_COUNTER_HPP
#define	FILE_LOCK_COUNTER_HPP

#include <stdio.h>
#include <unistd.h>

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/thread/mutex.hpp>

using namespace std;
using namespace boost::interprocess;

namespace cep {

    template<typename T>
    class file_lock_counter {
    public:
        file_lock_counter(size_t len4buf = 16);

        ~file_lock_counter();
    private: // Forbidden copy behave

        file_lock_counter(const file_lock_counter& flc) {
            cout << this << "->file_lock_counter copy from[" << &flc << "] ctor execute!" << endl;
        }

        file_lock_counter& operator=(const file_lock_counter& flc) {
            if (this == &flc) // effective c++ 16
                return *this;
            cout << this << "->file_lock_counter assignment operator(" << &flc << ") execute!" << endl;
            return *this;
        }
    public:
        bool reset(const string& filename, T counter);
        bool counting(const string& filename, T& interval);
        void fclose();
        bool fileIsExist(const string & filepath);
    private:
        bool freopen(const char* filename);
    private:
        FILE* file_;
        const size_t len4buf_;
        // char format_[8];
        boost::mutex mutex_;
    };

    ////////////////////////////////////////////////////////////////////////////

    template<typename T>
    file_lock_counter<T>::file_lock_counter(size_t len4buf)
    : len4buf_(len4buf), file_(NULL), mutex_() {
        // memset(format_, 0, sizeof (format_));
        // sprintf(format_, "%%0%dd", len4buf);
    }

    template<typename T>
    file_lock_counter<T>::~file_lock_counter() {
        fclose();
    }
    ////////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool file_lock_counter<T>::freopen(const char* filename) {
        if (file_ != NULL) fclose();
        // Open a file for update both reading and writing. The file must exist.
        file_ = fopen(filename, "r+");
        if (file_ == NULL) {
            // Create an empty file for both reading and writing.
            // If a file with the same name already exists its content is erased
            // and the file is treated as a new empty file.
            file_ = fopen(filename, "w+");
            if (file_ == NULL) {
                cout << ">>> file_lock_counter::freopen(" << filename
                        << ") error opening file!" << endl;
                return false;
            }
        }
        return true;
    }

    template<typename T>
    void file_lock_counter<T>::fclose() {
        if (file_ != NULL) {
            ::fclose(file_);
            file_ = NULL;
        }
    }
    ////////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool file_lock_counter<T>::reset(const string& filename, T counter) {
        boost::mutex::scoped_lock lock(mutex_);
        if (file_ == NULL && !freopen(filename.c_str())) return false;
        try {
            file_lock flock(filename.c_str());
            scoped_lock<file_lock> e_lock(flock);

            char buffer[len4buf_]; // = {0};
            memset(buffer, 0, sizeof (buffer));
            // sprintf(buffer, format_, counter);
            sprintf(buffer, "%d", counter);
            fwrite(buffer, 1, sizeof (buffer), file_);
            return true;
        } catch (interprocess_exception& e) {
            cout << ">>> file_lock_counter::reset(" << filename
                    << ',' << counter << ") error:" << e.what() << endl;
            return false;
        }
    }

    template<typename T>
    bool file_lock_counter<T>::counting(const string& filename, T& interval) {
        boost::mutex::scoped_lock lock(mutex_);
        if (file_ == NULL && !freopen(filename.c_str())) return false;
        try {
            file_lock flock(filename.c_str());
            scoped_lock<file_lock> e_lock(flock);

            char buffer[len4buf_]; // = {0};
            memset(buffer, 0, sizeof (buffer));
            fread(buffer, 1, sizeof (buffer), file_);
            T counter = 0;
            counter = cep::cstr2nbr(buffer, counter);
            // If successful, the function returns a zero value.
            // Otherwise, it returns nonzero value.
            // fseek(file_, 0, SEEK_SET);
            rewind(file_);
            counter += interval;
            memset(buffer, 0, sizeof (buffer));
            // sprintf(buffer, format_, counter);
            sprintf(buffer, "%d", counter);
            fwrite(buffer, 1, sizeof (buffer), file_);

            interval = counter;
            return true;
        } catch (interprocess_exception& e) {
            cout << ">>> file_lock_counter::counting(" << filename
                    << ',' << interval << ") error:" << e.what() << endl;
            return false;
        }
    }

    template<typename T>
    bool file_lock_counter<T>::fileIsExist(const string& filepath) {
        if (access(filepath.c_str(), F_OK) == 0) {
            return true;
        }
        return false;
    }
}

#endif	/* FILE_LOCK_COUNTER_HPP */
