/* 
 * File:   ConnConfig.cpp
 * Author: Administrator
 * 
 * Created on 2012年3月31日, 下午5:32
 */

#include "communication/ConnConfig.h"
#include <stdio.h>
//#include "utils_cstr2nbr.hpp"

ConnConfig::ConnConfig() {
}

ConnConfig::~ConnConfig() {
}

unsigned int ConnConfig::get_size(string name, unsigned int def_val) {
    map<string, string>::iterator iter = this->m_dict.find(name);
    if (iter != m_dict.end()) {
        if ((iter->second).find('M') != string::npos) {
            return cep::cstr2nbr(iter->second.substr(0, iter->second.size() - 1).c_str(), 0)*1024 * 1024;
        } else if ((iter->second).find('G') != string::npos) {
            return cep::cstr2nbr(iter->second.substr(0, iter->second.size() - 1).c_str(), 0)*1024 * 1024 * 1024;
        } else if ((iter->second).find('K') != string::npos) {
            return cep::cstr2nbr(iter->second.substr(0, iter->second.size() - 1).c_str(), 0)*1024;
        } else {
            return cep::cstr2nbr(iter->second.c_str(), 0);
        }
    }
    return def_val;
}

void ConnConfig::set_value(string name, string val) {
    this->m_dict.insert(map<string, string>::value_type(name, val));
}

void ConnConfig::reset() {
    this->m_dict.clear();
}
