/* 
 * File:   ConnConfig.h
 * Author: Administrator
 *
 * Created on 2012年3月31日, 下午5:32
 */

#ifndef CONNCONFIG_H
#define	CONNCONFIG_H
#include <map>
#include <string>
#include <assert.h>
#include "utils_cstr2nbr.hpp"

using namespace std;
#define _CONNMAXPNID 65535

class ConnConfig {
public:

    template<typename T>
    T get_value(string name, T def_val) {
        map<string, string>::iterator iter = this->m_dict.find(name);
        if (iter != m_dict.end()) {
            return cep::cstr2nbr(iter->second.c_str(), def_val);
        }
        return def_val;
    }

    string get_value(string name, const char* def_val) {
        map<string, string>::iterator iter = this->m_dict.find(name);
        if (iter != m_dict.end()) {
            return iter->second;
        }
        return string(def_val);
    }

    unsigned int get_size(string name, unsigned int def_val);

    void set_value(string name, string val);

    void reset();

    ConnConfig();
    ~ConnConfig();
private:

    /*
     * 键值对
     */
    ConnConfig(const ConnConfig& orig);

    map<string, string> m_dict;
};

template<>
inline string ConnConfig::get_value<string>(string name, string def_val) {
    map<string, string>::iterator iter = this->m_dict.find(name);
    if (iter != m_dict.end()) {
        return iter->second;
    }
    return def_val;
}

template<>
inline bool ConnConfig::get_value<bool>(string name, bool def_val) {
    map<string, string>::iterator iter = this->m_dict.find(name);
    if (iter != m_dict.end()) {
        if (iter->second == "true") {
            return true;
        } else if (iter->second == "false") {
            return false;
        }
    }
    return def_val;
}
#endif	/* CONNCONFIG_H */

