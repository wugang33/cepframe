/* 
 * File:   buffer_metadata.cpp
 * Author: wugang
 *
 * Created on 2011-06-16 PM 05:15
 */

#include "buffer_metadata.hpp"

#include <algorithm>
// #include <string.h>
#include "macro_log.hpp"
#include "buffer_function.hpp"

using namespace std;
// using namespace cep; // friend operator<< access permission denied 

namespace cep {

    string field_cfg_entry::to_string() const {
        // #include <limits.h>
        // uchar:0-256                          3
        // ushort:0-32767*2+1                   5
        // uint:0-2147483647*2+1                10
        // ulong:0-9223372036854775807L*2+1     20
        string result_str;
        result_str = "name[" + this->code_ + "]";
        char buffer[12 + 10 + 1] = {0};
        sprintf(buffer, "\tlength[%d]", this->length_);
        result_str += buffer;
        memset(buffer, 0, sizeof (buffer));
        sprintf(buffer, "\ttype[%d]", this->type_);
        result_str += buffer;
        memset(buffer, 0, sizeof (buffer));
        sprintf(buffer, "\toffset[%d]", this->offset_);
        result_str += buffer;
        memset(buffer, 0, sizeof (buffer));
        sprintf(buffer, "\tflag[0x%x]", this->flag_);
        result_str += buffer;
        memset(buffer, 0, sizeof (buffer));
        sprintf(buffer, "\tidx[%d]", this->idx_);
        result_str += buffer;
        return result_str;
    }

    std::ostream& operator<<(std::ostream& s, const field_cfg_entry& r) {
        s << &r << "::name[" << r.code_ << "]\tlength[" << r.length_
                << "]\ttype:" << buffer_metadata::get_type_by_id(r.type_)
                // ; buffer_metadata::get_type_by_id(s, r.dataType); s
                << "\toffset[" << r.offset_
                << "]\tflag[0x" << std::hex << r.flag_ << std::dec << "]\tidx[" << r.idx_ << ']';
        return s;
    }

    ////////////////////////////////////////////////////////////////////////////

    buffer_metadata::buffer_metadata()
    : length(0), persistent_field_count_(0), entry_list(), entry_map() {
    }

    buffer_metadata::~buffer_metadata() {
        cfg_entry_list::size_type size = entry_list.size();
        for (int i = 0; i < size; ++i) {
            delete entry_list[i];
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    /**
     * 
     * @param cfg_entry
     * @return false need to manage argument's memory
     */
    bool buffer_metadata::add_cfg_entry(field_cfg_entry *cfg_entry) {
        // cfg_entry->offset = this->length;
        // this->length = this->length + cfg_entry->length;
        // if (!cfg_entry->virt) {
        // this->entry_list.push_back(cfg_entry);
        // }
        // this->entry_map.insert(cfg_entry_map::value_type(cfg_entry->code, cfg_entry));
        pair < cfg_entry_map::iterator, bool> rst = entry_map.insert(
                cfg_entry_map::value_type(cfg_entry->code_, cfg_entry));
        if (rst.second) {
            entry_list.push_back(cfg_entry);
            return true;
        } else
            return false;
    }

    bool buffer_metadata::rebuild() {
        cfg_entry_list::size_type size = entry_list.size();
        std::sort(entry_list.begin(), entry_list.end(), sort_pred);
        length = 0;
        for (int i = 0; i < size; ++i) {
            int padding_len = cep::utils_function::padding_length(length, entry_list[i]->type_);
            if (!(entry_list[i]->flag_ & __CEP_VIRTUAL_MASK__)) entry_list[i]->offset_ = length + padding_len;
            entry_list[i]->length_ = get_size_by_type_id(entry_list[i]->type_, entry_list[i]->length_);
            if (!(entry_list[i]->flag_ & __CEP_VIRTUAL_MASK__)) length += (entry_list[i]->length_ + padding_len);
        }
        persistent_field_count_ = 0;
        for (int i = 0; i < size; ++i) {
            if (!(entry_list[i]->flag_ & __CEP_VIRTUAL_MASK__)) ++persistent_field_count_;
            if ((entry_list[i]->flag_ & __CEP_VIRTUAL_MASK__)
                    && entry_list[i]->offset_ + entry_list[i]->length_ > length) {
                MLOG_ERROR << "Virtual field out of range:" << *entry_list[i] << '\n'
                        << *this << endl;
                return false;
            }
            //            if (STR > entry_list[i]->type_ || entry_list[i]->type_ > IP) {
            if (STR > entry_list[i]->type_ || entry_list[i]->type_ > MONTHTIME) {
                MLOG_ERROR << "Field data type unsupported:" << *entry_list[i] << '\n'
                        << *this << endl;
                return false;
            }
            entry_list[i]->idx_ = i; // it can use this seq as a index!!!
        }
        return true;
    }

    string buffer_metadata::to_string() const {
        string result_str;
        cfg_entry_list::size_type size = entry_list.size();
        for (int i = 0; i < size; ++i) {
            result_str.append(entry_list[i]->to_string());

            if (i + 1 < size) result_str.push_back('\n');
        }
        return result_str;
    }

    std::ostream& operator<<(std::ostream& s, const cep::buffer_metadata& r) {
        s << &r << "::size=" << r.length << std::endl;
        cep::buffer_metadata::cfg_entry_list::size_type size = r.entry_list.size();
        for (int i = 0; i < size; ++i) {
            s << *(r.entry_list[i]);
            if (i + 1 < size) s << '\n';
        }
        // cep::buffer_metadata::cfg_entry_map::const_iterator itr = r.entry_map.begin();
        // cep::buffer_metadata::cfg_entry_map::const_iterator end = r.entry_map.end();
        // if (itr != end) {
        //     s << *(itr->second);
        //     ++itr;
        //     while (itr != end) {
        //         s << '\n' << *(itr->second);
        //         ++itr;
        //     }
        // }
        return s;
    }
}
