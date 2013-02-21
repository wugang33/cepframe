/* 
 * File:   dlhandler.hpp
 * Author: WangquN
 *
 * Created on 2012-02-25 AM 11:42
 */

#ifndef DLHANDLER_HPP
#define	DLHANDLER_HPP

#include <dlfcn.h>
#include <string>
#include <map>

#include "macro_log.hpp"

using std::string;
using std::endl;
using std::map;

namespace cep {

    class dlhandler {
    public:
        dlhandler();
        ~dlhandler();
    private:

        dlhandler(dlhandler& arg) { // Forbidden copy behave
            // MLOG_INFO << this << "->dlhandler copy from[" << &arg << "] ctor execute!" << endl;
        }

        dlhandler& operator=(const dlhandler& arg) { // Forbidden copy behave
            if (this == &arg) // effective c++ 16
                return *this;
            // MLOG_INFO << this << "->dlhandler assignment operator(" << &arg << ") execute!" << endl;
            return *this;
        }
    public:

        int mode() const {
            return mode_;
        }

        void set_mode(const int mode) {
            mode_ = mode;
        }

        bool is_open() const {
            return is_open_;
        }

        string file() const {
            return file_;
        }
    public:
        bool open(const string& file, const int mode = RTLD_NOW);
        void close();

        template<typename func_ptr>
        bool dlsymbol(const string& function_name, func_ptr& func) const {
            // bool dlsymbol(const string& function_name, func_ptr* func) const {
            if (!is_open()) {
                MLOG_WARN << *this << "::dlsymbol(" << function_name << ", func_ptr&) must open first!" << endl;
                return false;
            }
            // *func = (func_ptr) dlsym(handle_, function_name.c_str());
            func = (func_ptr) dlsym(handle_, function_name.c_str());
            // if (*func == NULL) {
            if (func == NULL) {
                MLOG_WARN << *this << " load [" << function_name << "] error:" << dlerror() << endl;
                return false;
            } else {
                MLOG_DEBUG << *this << " load [" << function_name << "] success!" << endl;
                return true;
            }
        }
    private:
        friend std::ostream& operator<<(std::ostream&, const dlhandler&);

        int mode_;
        bool is_open_;
        void * handle_;
        string file_;
    public:

        class factory {
        public:
            factory();
            ~factory();
        private:
            typedef map<string, dlhandler*> dlhandler_map_t;

            factory(factory& arg) { // Forbidden copy behave
                // MLOG_INFO << this << "->factory copy from[" << &arg << "] ctor execute!" << endl;
            }

            factory& operator=(const factory& arg) { // Forbidden copy behave
                if (this == &arg) // effective c++ 16
                    return *this;
                // MLOG_INFO << this << "->factory assignment operator(" << &arg << ") execute!" << endl;
                return *this;
            }
        public:
            bool add_dlhandler(const string& file, const int mode = RTLD_NOW);
            const dlhandler* get_dlhandler(const string& file) const;

            void clear();
        private:
            friend std::ostream& operator<<(std::ostream&, const factory&);

            dlhandler_map_t m_;
        };
    };
}

#endif	/* DLHANDLER_HPP */
