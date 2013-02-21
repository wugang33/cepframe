/* 
 * File:   config_metadata.cpp
 * Author: WangquN
 *
 * Created on 2012-01-31 PM 01:59
 */

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "config_metadata.hpp"

// #include <map>
// #include "utils_xml.hpp"
#include "buffer_metadata.hpp"
#include "slice.hpp"
#include "utils_cstr2nbr.hpp"
#include "macro_log.hpp"
#include <set>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

namespace cep {

    config_metadata::~config_metadata() {
        free();
    }

    bool config_metadata::reload_metadata(const xml_element& xml) {
        free();
        buffer_metadata *metadata = NULL;
        field_cfg_entry *entry;
        const cep::xml_element *ele = cep::find_element(xml, "event");
        const cep::xml_element *ele_event_temp = NULL;
        cep::slice svar1, svar2, default_value("");
        field_cfg_entry::data_t offset, length, type;
        field_cfg_entry::flag_t flag;
        field_cfg_entry::idx_t idx;
        string event_name, code;
        //        type_t event_type;
        vector<type_t> event_types;
        while (ele != NULL) {
            svar1 = cep::name(*ele);
            if (strcmp("event", svar1.data()) == 0) {
                event_types.clear();
                svar1 = cep::attribute(*ele, "id", default_value);
                if (svar1.compare(default_value) != 0) {
                    string types_str = svar1.to_string();
                    vector<string> types_vec;
                    boost::split(types_vec, types_str, boost::is_any_of(","));
                    type_t event_type;

                    BOOST_FOREACH(string s, types_vec) {
                        try {
                            boost::trim(s);
                            event_type = boost::lexical_cast<type_t > (s);
                        } catch (boost::bad_lexical_cast e) {
                            MLOG_WARN << this << "->config_metadata::reload_metadata  type id[" << s << "] format is error..." << endl;
                            continue;
                        }
                        event_types.push_back(event_type);
                    }

                    //                    event_type = cep::cstr2nbr(svar1.data(), (type_t) 0);
                    if (event_types.size() == 0) {
                        MLOG_WARN << this << "->config_metadata::reload_metadata  type id is null and skip this event element" << endl;
                        ele = cep::next_sibling(*ele);
                        continue;
                    }
                    svar2 = cep::attribute(*ele, "name", default_value);
                    if (svar2.compare(default_value) != 0) {
                        event_name = svar2.data();
                    } else
                        event_name = svar1.data();
                } else {
                    MLOG_WARN << this << "->config_metadata::reload_metadata cant found event type id and skip this event element" << endl;
                    ele = cep::next_sibling(*ele);
                    continue;
                }

                metadata = new buffer_metadata();
                bool needcontinue = false;

                BOOST_FOREACH(type_t event_type_temp, event_types) {
                    if (!insert_metadata(event_type_temp, event_name, metadata)) {
                        // delete metadata;
                        MLOG_WARN << this << "->config_metadata::reload_metadata insert meta data "
                                << event_type_temp << '[' << event_name << "] unique constraint!" << endl;
                        //  return false;
                    } else {
                        needcontinue = true; /*只要插入了一条 那就可以继续下去*/
                    }
                }

                if (!needcontinue) {
                    delete metadata;
                    stringstream ss;
                    for (int i = 0; i < event_types.size(); i++) {
                        ss << event_types[i];
                        // cout << event_types[i];
                        if (i != event_types.size() - 1) {
                            ss << ',';
                        }
                    }
                    MLOG_WARN << this << "->config_metadata::reload_metadata skip this event element due to the type [" << ss.str() << "] unique constraint " << endl;
                    ele = cep::next_sibling(*ele);
                    continue;
                }
            }
            ele_event_temp = cep::first_child(*ele);
            while (ele_event_temp != NULL) {
                svar1 = cep::name(*ele_event_temp);
                if (strcmp("attr", svar1.data()) == 0) {
                    svar1 = cep::value(*ele_event_temp, default_value);
                    if (svar1.compare(default_value) != 0) {
                        code = svar1.data();
                    } else {
                        code = "not found!";
                    }
                    svar1 = cep::attribute(*ele_event_temp, "pos", default_value);
                    if (svar1.compare(default_value) != 0) {
                        idx = cep::cstr2nbr(svar1.data(), (field_cfg_entry::idx_t) 0);
                    } else {
                        idx = 0;
                        stringstream ss;
                        for (int i = 0; i < event_types.size(); i++) {
                            ss << event_types[i];
                            // cout << event_types[i];
                            if (i != event_types.size() - 1) {
                                ss << ',';
                            }
                        }
                        MLOG_ERROR << this << "->type[" << ss.str() << "] name[" << event_name << "]" << "] field [" << code
                                << "] must have 'pos' attribute to order the fields!" << endl;


                        //                        MLOG_ERROR << *this << "->" << event_type << '[' << event_name
                        //                                << "] field [" << code
                        //                                << "] must have 'pos' attribute to order the fields!" << endl;
                        return false;
                    }
                    svar1 = cep::attribute(*ele_event_temp, "type", default_value);
                    if (svar1.compare(default_value) != 0) {
                        type = cep::cstr2nbr(svar1.data(), (field_cfg_entry::data_t) 0);
                    } else {
                        type = 0;
                    }
                    svar1 = cep::attribute(*ele_event_temp, "flag", default_value);
                    if (svar1.compare(default_value) != 0) {
                        flag = cep::cstr2nbr(svar1.data(), (field_cfg_entry::flag_t) 0);
                    } else {
                        flag = 0;
                    }
                    if (type == 0 || flag == 1) {
                        svar1 = cep::attribute(*ele_event_temp, "len", default_value);
                        if (svar1.compare(default_value) != 0) {
                            length = cep::cstr2nbr(svar1.data(), (field_cfg_entry::data_t) 0);
                        } else {
                            length = 0;
                            stringstream ss;
                            for (int i = 0; i < event_types.size(); i++) {
                                ss << event_types[i];
                                // cout << event_types[i];
                                if (i != event_types.size() - 1) {
                                    ss << ',';
                                }
                            }
                            MLOG_ERROR << this << "->type[" << ss.str() << "] name[" << event_name << "]" << "] The virtual or string field [" << code
                                    << "]  need 'len' attribute!" << endl;
                            //                            MLOG_ERROR << "->" << event_type << '[' << event_name
                            //                                    << "]The virtual or string field[" << code
                            //                                    << "] need 'len' attribute!" << endl;
                            return false;
                        }
                    }
                    if (flag == 1) {
                        svar1 = cep::attribute(*ele_event_temp, "offset", default_value);
                        if (svar1.compare(default_value) != 0) {
                            offset = cep::cstr2nbr(svar1.data(), (field_cfg_entry::data_t) 0);
                        } else {
                            offset = 0;
                            stringstream ss;
                            for (int i = 0; i < event_types.size(); i++) {
                                ss << event_types[i];
                                // cout << event_types[i];
                                if (i != event_types.size() - 1) {
                                    ss << ',';
                                }
                            }
                            MLOG_ERROR << this << "->type[" << ss.str() << "] name[ " << event_name << "]" << "] the virtual field [" << code
                                    << "]  need 'offset' attribute!" << endl;
                            //                            MLOG_ERROR << "->" << event_type << '[' << event_name
                            //                                    << "] the virtual field[" << code
                            //                                    << "] need 'offset' attribute!" << endl;
                            return false;
                        }
                    }
                    entry = new field_cfg_entry(code, idx, type, length, flag, offset);
                    if (!metadata->add_cfg_entry(entry)) {
                        delete entry;
                        stringstream ss;
                        for (int i = 0; i < event_types.size(); i++) {
                            ss << event_types[i];
                            // cout << event_types[i];
                            if (i != event_types.size() - 1) {
                                ss << ',';
                            }
                        }
                        MLOG_ERROR << this << "->config_metadata::add_cfg_entry code [ " << code << "] to type[" << ss.str() << "]unique constraint!" << endl;
                        //                        MLOG_WARN << this << "->config_metadata::add_cfg_entry code["
                        //                                << code << "] to event meta data:" << event_type << '[' << event_name
                        //                                << "] unique constraint!" << endl;
                        return false;
                    }
                }
                ele_event_temp = cep::next_sibling(*ele_event_temp);
            }
            ele = cep::next_sibling(*ele);
            if (!(metadata->rebuild())) return false;
        }
        return true;
    }

    buffer_metadata* config_metadata::metadata(const type_t type) const {
        map_t::const_iterator iter = m_.find(type);
        if (iter != m_.end()) return iter->second;
        else return NULL;
    }

    buffer_metadata* config_metadata::metadata(const string& code) const {
        code_map_t::const_iterator itr = code_map_.find(code);
        if (itr != code_map_.end()) return itr->second;
        else return NULL;
    }

    void config_metadata::free() {
        map_t::const_iterator iter;
        set<buffer_metadata*> diffset; //这个set用来排重buffer_metadata 因为可能多个类型指向了一个buffer_metadata
        for (iter = m_.begin(); iter != m_.end(); ++iter) {
            diffset.insert(iter->second);
            // delete iter->second;
            // iter->second = NULL;
        }

        BOOST_FOREACH(buffer_metadata*bm, diffset) {
            delete bm;
            bm = NULL;
        }
        m_.clear();
        code_map_.clear();
        info_.clear();
    }

    bool config_metadata::insert_metadata(const type_t type, const string& code, buffer_metadata* metadata) {
        if (metadata == NULL) return true;
        //        if (code_map_.find(code) != code_map_.end()) return false;
        std::pair < map_t::iterator, bool> rst = m_.insert(map_t::value_type(type, metadata));
        if (rst.second) {
            code_map_.insert(code_map_t::value_type(code, metadata));
            info_.push_back(make_pair(type, code));
        }
        return rst.second;
    }
    ////////////////////////////////////////////////////////////////////////////

    ostream& operator<<(ostream& s, const config_metadata& r) {
        /*s << &r << " size is " << r.m_.size();
        config_metadata::map_t::const_iterator itr = r.m_.begin();
        config_metadata::map_t::const_iterator end = r.m_.end();
        for (; itr != end; ++itr) {
            s << '\n' << itr->first << "\t:" << *(itr->second);
        }*/
        size_t size = r.info_.size();
        s << &r << " size is " << size;
        config_metadata::type_t type;
        for (int i = 0; i < size; ++i) {
            type = r.info_[i].first;
            s << '\n' << type << '[' << r.info_[i].second << "]\t:"
                    // << *(r.m_[type]);
                    << *(r.m_.find(type)->second);
        }
        return s;
    }
}
