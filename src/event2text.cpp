/* 
 * File:   CSaveEvent2File.cpp
 * Author: wg
 * Created on 2011-07-06 PM 05:54
 * Modified by luoxiaoyi 2012-04-05
 */

#include "stdio.h"
#include <string>
#include<string.h>
#include <boost/algorithm/string.hpp>
#include "event2text.hpp"
#include "event.hpp"
#include "config_metadata.hpp"
#include "singleton.hpp"
#include "utils_cstr2nbr.hpp"
#include "utils_nbr2str.hpp"
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace cep;
namespace cep {

    void set_value_int(event &eventmsg, size_t index, const string &value) {
        int temp = cstr2nbr(value.c_str(), (int) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_uint(event &eventmsg, size_t index, const string &value) {
        unsigned int temp = cstr2nbr(value.c_str(), (unsigned int) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_short(event &eventmsg, size_t index, const string &value) {
        short temp = cstr2nbr(value.c_str(), (short) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_ushort(event &eventmsg, size_t index, const string &value) {
        unsigned short temp = cstr2nbr(value.c_str(), (unsigned short) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_long(event &eventmsg, size_t index, const string &value) {
        long temp = cstr2nbr(value.c_str(), (long) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_ulong(event &eventmsg, size_t index, const string &value) {
        unsigned long temp = cstr2nbr(value.c_str(), (unsigned long) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_char(event &eventmsg, size_t index, const string &value) {
        eventmsg.set_value(index, slice(value.c_str(), value.size()));
    }

    void set_value_uchar(event &eventmsg, size_t index, const string &value) {
        unsigned char temp = cstr2nbr(value.c_str(), (unsigned char) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_double(event &eventmsg, size_t index, const string &value) {
        double temp = cstr2nbr(value.c_str(), (double) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_float(event &eventmsg, size_t index, const string &value) {
        float temp = cstr2nbr(value.c_str(), (float) 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_datatime(event &eventmsg, size_t index, const string &value) {
        unsigned long temp = cstr2milsecond(value.c_str());
        eventmsg.set_value(index, temp);
    }

    void set_value_ip(event &eventmsg, size_t index, const string &value) {
        unsigned int temp = str2ip(value, 0);
        eventmsg.set_value(index, temp);
    }

    void set_value_monthtime(event &eventmsg, size_t index, const string &value) {
        unsigned long temp = cep::cstr2nbr(value.c_str(), (unsigned long) 0); //cstr2milsecond(value.c_str(), "%4d%2d%2d");
        eventmsg.set_value(index, temp);
    }

    void set_value_str(event &eventmsg, size_t index, const string &value) {
        eventmsg.set_value(index, slice(value.c_str(), value.size()));
    }

    string get_value_int(const event &eventmsg, size_t index) {
        int temp = eventmsg.value(index, (int) 0);
        return nbr2str(temp);
    }

    string get_value_uint(const event &eventmsg, size_t index) {
        unsigned int temp = eventmsg.value(index, (unsigned int) 0);
        return nbr2str(temp);
    }

    string get_value_short(const event &eventmsg, size_t index) {
        short temp = eventmsg.value(index, (short) 0);
        return nbr2str(temp);
    }

    string get_value_ushort(const event &eventmsg, size_t index) {
        unsigned short temp = eventmsg.value(index, (unsigned short) 0);
        return nbr2str(temp);
    }

    string get_value_long(const event &eventmsg, size_t index) {
        long temp = eventmsg.value(index, (long) 0);
        return nbr2str(temp);
    }

    string get_value_ulong(const event &eventmsg, size_t index) {
        unsigned long temp = eventmsg.value(index, (unsigned long) 0);
        return nbr2str(temp);
    }

    string get_value_char(const event &eventmsg, size_t index) {
        string s;
        char temp = eventmsg.value(index, (char) 0);
        s.push_back(temp);
        return s;
    }

    string get_value_uchar(const event &eventmsg, size_t index) {
        unsigned char temp = eventmsg.value(index, (unsigned char) 0);
        return nbr2str(temp);
    }

    string get_value_double(const event &eventmsg, size_t index) {
        double temp = eventmsg.value(index, (double) 0);
        return nbr2str(temp);
    }

    string get_value_float(const event &eventmsg, size_t index) {
        float temp = eventmsg.value(index, (float) 0);
        return nbr2str(temp);
    }

    string get_value_datatime(const event &eventmsg, size_t index) {
        unsigned long temp = eventmsg.value(index, (unsigned long) 0);
        return milsecond2str(temp);
    }

    string get_value_ip(const event &eventmsg, size_t index) {
        unsigned int temp = eventmsg.value(index, (unsigned int) 0);
        return ip2str(temp);
    }

    string get_value_monthtime(const event &eventmsg, size_t index) {
        unsigned long temp = eventmsg.value(index, (unsigned long) 0);
        return cep::nbr2str(temp);
    }

    string get_value_str(const event &eventmsg, size_t index) {
        slice s = eventmsg.value(index, slice(0, 0));
        string tmpstr = string(s.to_string().c_str());
        boost::replace_all(tmpstr, "\n", "\\n");
        boost::replace_all(tmpstr, "\"", "\"\"");
        return tmpstr;
    }

    //    void (*set_value[13])(event &eventmsg, size_t index, const string &value) = {
    void (*set_value[14])(event &eventmsg, size_t index, const string &value) = {
        &set_value_str, &set_value_char, &set_value_uchar, &set_value_short,
        &set_value_ushort, &set_value_int, &set_value_uint, &set_value_float,
        &set_value_long, &set_value_ulong, &set_value_double, &set_value_datatime,
        &set_value_ip, &set_value_monthtime
    };

    //    string(*get_value[13])(const event &eventmsg, size_t index) = {
    string(*get_value[14])(const event &eventmsg, size_t index) = {
        &get_value_str, &get_value_char, &get_value_uchar, &get_value_short,
        &get_value_ushort, &get_value_int, &get_value_uint, &get_value_float,
        &get_value_long, &get_value_ulong, &get_value_double, &get_value_datatime,
        &get_value_ip, &get_value_monthtime
    };
}

event2text::event2text(char csv_escape, char csv_delimiter)
: file_(NULL), linenum_(0), csv_escape_(csv_escape), csv_delimiter_(csv_delimiter)
, start_buf_cur_(0), end_buf_cur_(0), set_values(set_value), get_values(get_value), header_() {
    memset(buffer_, 0, sizeof (buffer_));
}

event2text::~event2text() {
    if (file_ != 0) {
        flush();
        fclose(file_);
        file_ = 0;
    }
}

int event2text::open(const char * filename, const char * mode) {
    linenum_ = 0;
    filename_.assign(filename);
    if (is_open())
        close();
    file_ = fopen(filename, mode);
    if (file_) {
        fgetpos(file_, &header_pos_);
        return 1;
    }
    return 0;
}

int event2text::rename(const string & new_filename) {
    //string new_filename = filename + ".mm";
    close();
    // success return 0, else return -1
    if (::rename(filename_.c_str(), new_filename.c_str()) == 0) { // success
        filename_ = new_filename;
        return 0;
    } else {
        return -1;
    }
}

void event2text::close() {
    if (file_ != 0) {
        flush();
        fclose(file_);
        file_ = 0;
    }
}

int event2text::swap(const char *filename, const char *mode) {
    if (is_open()) close();
    return open(filename, mode);
}

void event2text::to_string(const event& eventmsg, string& event_str) {
    if (file_ == NULL) return; // NULL;
    const cep::buffer_metadata * metaData = eventmsg.metadata();
    if (metaData == NULL) {
        MLOG_ERROR << "event2text::write metaData(type:" << eventmsg.type() << ") is NULL,please check the configuration!" << endl;
        return; // NULL;
    }
    const buffer_metadata::cfg_entry_list &eventTypeVector = metaData->get_entry_list();
    buffer_metadata::cfg_entry_list::const_iterator iter;
    // string * event_str = new string;
    for (iter = eventTypeVector.begin(); iter != eventTypeVector.end(); iter++) {
        if ((*iter)->flag() & __CEP_VIRTUAL_MASK__) {
            continue;
        }
        if (iter != eventTypeVector.begin()) {
            event_str.push_back(csv_delimiter_);
        }
        // if (csv_escape_) event_str->push_back(csv_escape_);
        if (csv_escape_) event_str.push_back(csv_escape_);
        field_cfg_entry *event_type_enty = *iter;
        string temp_str = "";
        temp_str = get_value[event_type_enty->type()](eventmsg, event_type_enty->idx());
        // *event_str += temp_str;
        event_str += temp_str;
        // if (csv_escape_) event_str->push_back(csv_escape_);
        if (csv_escape_) event_str.push_back(csv_escape_);
        /*if ((iter + 1) != eventTypeVector.end()) {
            // event_str->push_back(csv_delimiter_);
            event_str.push_back(csv_delimiter_);
        }*/
    }
    // event_str->push_back(0x0a);
    event_str.push_back(0x0a);
    // return event_str;
}

bool event2text::write(const string& event_str) {
    //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
    if (file_ == NULL) return false;
    int retval = fwrite(event_str.c_str(), 1, event_str.length(), file_);
    if (retval != event_str.length()) return false;
    return true;
}

bool event2text::write(const event& eventmsg) {
    //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
    if (file_ == NULL) return false;
    const cep::buffer_metadata * metaData = eventmsg.metadata();
    if (metaData == NULL) {
        MLOG_ERROR << "event2text::write metaData(type:" << eventmsg.type() << ") is NULL,please check the configuration!" << endl;
        return false;
    }
    const buffer_metadata::cfg_entry_list &eventTypeVector = metaData->get_entry_list();
    buffer_metadata::cfg_entry_list::const_iterator iter;
    string event_str = "";
    for (iter = eventTypeVector.begin(); iter != eventTypeVector.end(); iter++) {
        if ((*iter)->flag() & __CEP_VIRTUAL_MASK__) {
            continue;
        }
        if (iter != eventTypeVector.begin()) {
            event_str.push_back(csv_delimiter_);
        }
        if (csv_escape_) event_str.push_back(csv_escape_);
        field_cfg_entry *event_type_enty = *iter;
        string temp_str = "";
        temp_str = get_value[event_type_enty->type()](eventmsg, event_type_enty->idx());
        event_str += temp_str;
        if (csv_escape_) event_str.push_back(csv_escape_);
        // fwrite(temp_str.c_str(), 1, temp_str.size(), file);
        // fputc(CSV_ESCAPE, file);
        /*if ((iter + 1) != eventTypeVector.end()) {
            event_str.push_back(csv_delimiter_);
        }*/
    }
    event_str.push_back(0x0a);
    int retval = fwrite(event_str.c_str(), 1, event_str.length(), file_);
    if (retval != event_str.length()) return false;
    return true;
}

bool event2text::read(event &eventmsg) {
    if (!file_) {
        MLOG_ERROR << " event2text::read() Can't be read file before open it" << endl;
        return false;
    }
    if (is_end() == 1) return false;
    const cep::buffer_metadata *metaData = eventmsg.metadata();
    if (!metaData) {
        MLOG_ERROR << " MetaData is NULL" << endl;
        return false;
    }

    const buffer_metadata::cfg_entry_list vec = metaData->get_entry_list();
    vector<string> fields;
    readfields_csv(fields, csv_escape_, csv_delimiter_);
    const size_t field_size = fields.size();
    // if (fields.size() != vec.size()) {
    const int persistent_field_count = metaData->persistent_field_count();
    if (field_size != persistent_field_count) {
        MLOG_ERROR << " event2text::read fields[" << field_size
                << "] not match the configuration of [" << eventmsg.type() << "][" << persistent_field_count
                << "].please check the configuration and readed file[" << filename_ << '>' << linenum_ << "]" << endl;
        return false;
    }
    const int field_count = vec.size();
    int field_cfg_entry_index = 0;
    for (int i = 0; i < field_size; ++i) {
        if (field_cfg_entry_index >= field_count) {
            MLOG_ERROR << " event2text::read fields too many:"
                    << field_cfg_entry_index << ">=" << field_count << endl;
            return false;
        }
        while ((vec[field_cfg_entry_index]->flag() & __CEP_VIRTUAL_MASK__)
                && field_cfg_entry_index < field_count - 1) {
            ++field_cfg_entry_index;
        }
        field_cfg_entry *event_type_enty = vec[field_cfg_entry_index];
        set_value[event_type_enty->type()](eventmsg, event_type_enty->idx(), fields[i]);
        ++field_cfg_entry_index;
    }
    return true;
}

void event2text::readfields_csv(vector<string> &fields, char escape, char delimiter) {
    linenum_++;
    int state = 0;
    string line_temp = "";
    bool exitwhile = false;
    while (!exitwhile) {
        while (start_buf_cur_ < end_buf_cur_) {
            char curr_char = buffer_[start_buf_cur_];
            if (state == 0) {//start
                if (curr_char == delimiter) {
                    fields.push_back(line_temp);
                    line_temp.clear();
                } else if (curr_char == escape) {
                    state = 1;
                } else if (curr_char == 0x0d) {
                    if ((start_buf_cur_) >= end_buf_cur_ - 1) {
                        memset(buffer_, 0, sizeof (buffer_));
                        read2buf();
                        if (start_buf_cur_ >= end_buf_cur_)
                            exitwhile = true;
                    }
                    if ((start_buf_cur_ + 1) < end_buf_cur_) {
                        if (buffer_[start_buf_cur_ + 1] == 0x0A)
                            start_buf_cur_++;
                    }
                    state = 3;
                } else if (curr_char == 0x0a) {
                    state = 3;
                } else {
                    line_temp.push_back(curr_char);
                }
            } else if (state == 1) {
                if (curr_char == escape) {
                    state = 2;
                } else {
                    line_temp.push_back(curr_char);
                }
            } else if (state == 2) {
                if (curr_char == escape) {
                    state = 1;
                    line_temp.push_back(curr_char);
                } else if (curr_char == delimiter) {
                    state = 0;
                    fields.push_back(line_temp);
                    line_temp.clear();
                } else if (curr_char == 0x0d) {
                    if ((start_buf_cur_) >= end_buf_cur_ - 1) {
                        memset(buffer_, 0, sizeof (buffer_));
                        read2buf();
                        if (start_buf_cur_ >= end_buf_cur_)
                            exitwhile = true;
                    }
                    if ((start_buf_cur_ + 1) < end_buf_cur_)
                        if (buffer_[start_buf_cur_ + 1] == 0x0A)
                            start_buf_cur_++;
                    state = 3;
                } else if (curr_char == 0x0a) {
                    state = 3;
                } else {
                    line_temp.push_back(curr_char);
                }
            }
            if (state == 3) {
                start_buf_cur_++;
                fields.push_back(line_temp);
                line_temp.clear();
                return;
            }
            start_buf_cur_++;
        } //end for  �����������
        memset(buffer_, 0, sizeof (buffer_));
        read2buf();
        if (start_buf_cur_ >= end_buf_cur_)
            exitwhile = true;
    }
    if (state == 1) {
        fields.clear();
    }
    if (state == 2 || state == 0) {
        fields.push_back(line_temp);
    }
}

void event2text::read_header(vector<string>& fields) {
//    if (!file_) return;
//    if (header_.size() == 0) {
//        fpos_t now_pos;
//        fgetpos(file_, &now_pos);
//        if (now_pos.__pos == 0) { // at the begining of the file.
//            read2buf();
//            readfields_csv(header_, csv_escape_, csv_delimiter_);
//        } else {
//            // mark the current position
//            int pre_start_buf_cur = start_buf_cur_;
//            int pre_end_buf_cur = end_buf_cur_;
//
//            // set the file pos to the very first position, and ready to read header.
//            fsetpos(file_, &header_pos_);
//            read2buf();
//            readfields_csv(header_, csv_escape_, csv_delimiter_);
//
//            // set the file position to the last read position.
//            now_pos.__pos -= pre_end_buf_cur;
//            fsetpos(file_, &now_pos);
//            read2buf();
//            start_buf_cur_ = pre_start_buf_cur;
//        }
//    }
//
//    fields.clear();
//    fields.assign(header_.begin(), header_.end());
}
/**
void event2text::replace_escape(string &str, int mode) {
    string escape_1 = "";
    string escape_2 = "";
    escape_1.push_back(csv_escape_);
    escape_2.push_back(csv_escape_);
    escape_2.push_back(csv_escape_);
    if (mode == 0) {
        boost::replace_all(str, escape_2, escape_1);
    } else if (mode == 1) {
        boost::replace_all(str, escape_1, escape_2);
    }
}*/
