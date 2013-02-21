/* 
 * File:   dlhandler.hpp
 * Author: WangquN
 *
 * Created on 2012-02-25 AM 11:42
 */

#include "dlhandler.hpp"

using namespace std;

namespace cep {

    dlhandler::dlhandler() : mode_(RTLD_NOW), is_open_(false), handle_(NULL), file_("") {
    }

    dlhandler::~dlhandler() {
        close();
    }

    bool dlhandler::open(const string& file, const int mode) {
        if (is_open_ && file_ == file) return true;
        close();

        if (mode_ == RTLD_LAZY || mode_ == RTLD_NOW) {
            handle_ = dlopen(file.c_str(), mode_);
        } else {
            handle_ = dlopen(file.c_str(), RTLD_NOW);
        }

        if (handle_ == NULL) {
            MLOG_WARN << *this << "::open(" << file << ',' << mode << ") error :" << dlerror() << endl;
            return false;
        } else {
            is_open_ = true;
            file_ = file;
            return true;
        }
    }

    void dlhandler::close() {
        if (is_open_) {
            dlclose(handle_);
            is_open_ = false;
            handle_ = NULL;
            file_ = "";
        }
    }

    std::ostream& operator<<(std::ostream& s, const dlhandler& r) {
        return s << &r << "->" << r.file_ << ": is " << (r.is_open_ ? "opened" : "closed")
                << " in mode[" << r.mode_ << ']';
    }

    ////////////////////////////////////////////////////////////////////////////

    dlhandler::factory::factory() : m_() {
    }

    dlhandler::factory::~factory() {
        clear();
    }

    bool dlhandler::factory::add_dlhandler(const string& file, const int mode) {
        dlhandler_map_t::const_iterator found = m_.find(file);
        if (found == m_.end()) {
            dlhandler* dlh = new dlhandler();
            if (dlh->open(file, mode))
                m_.insert(dlhandler_map_t::value_type(file, dlh));
            else {
                delete dlh;
                return false;
            }
        }
        return true;
    }

    const dlhandler* dlhandler::factory::get_dlhandler(const string& file) const {
        dlhandler_map_t::const_iterator found = m_.find(file);
        if (found != m_.end()) return found->second;
        else return NULL;
    }

    void dlhandler::factory::clear() {
        dlhandler_map_t::const_iterator itr = m_.begin();
        dlhandler_map_t::const_iterator end = m_.end();
        for (; itr != end; ++itr) delete itr->second;
        m_.clear();
    }

    std::ostream& operator<<(std::ostream& s, const dlhandler::factory& r) {
        s << &r << "->size=" << r.m_.size();
        dlhandler::factory::dlhandler_map_t::const_iterator itr = r.m_.begin();
        dlhandler::factory::dlhandler_map_t::const_iterator end = r.m_.end();
        for (; itr != end; ++itr)
            s << "\n >" << itr->first << '=' << *(itr->second);
        return s;
    }
}
