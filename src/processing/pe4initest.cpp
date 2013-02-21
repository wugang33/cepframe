/* 
 * File:   pe4initest.cpp
 * Author: WangquN
 *
 * Created on 2012-02-22 AM 09:15
 */

#include "processing/pe4initest.hpp"

#include "macro_log.hpp"
#include "utils_xml.hpp"
#include "event.hpp"
#include "processing/plugin_symbol.hpp"
#include "processing/utils.hpp"

namespace cep {

    pe4initest::pe4initest(const xml_element& config, const xml_element& main_config)
    : pe(config, main_config) {
        MLOG_DEBUG << this << "->pe4initest ctor(" << &config << ", " << &main_config << ") execute!" << endl;
    }

    pe4initest::~pe4initest() {
        destroy();
        MLOG_DEBUG << this << "->pe4initest dtor execute!" << endl;
    }

    /*CEP_LOAD_PE(config, main_config) {
        cout << "execute " << CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION)
                << '(' << &config << "->" << cep::name(config).data()
                << ',' << &main_config << "->" << cep::name(main_config).data() << ')' << endl;
        return new pe4initest(config, main_config);
    }*/

    CEP_UNLOAD_PE(ptr_pe) {
        delete ptr_pe;
        ptr_pe = NULL;
    }
    // private: // Forbidden copy behave

    /*pe4initest::pe4initest(const pe4initest& p) {
        MLOG_DEBUG << this << "->pe4initest copy from[" << &p << "] ctor execute!" << endl;
    }*/

    pe4initest& pe4initest::operator=(const pe4initest& p) {
        if (this == &p) // effective c++ 16
            return *this;
        MLOG_DEBUG << this << "->pe4initest assignment operator(" << &p << ") execute!" << endl;
        return *this;
    }

    /** const or no resource properties initialization */
    bool pe4initest::init() {
        MLOG_DEBUG << this << "->pe4initest::init execute!" << endl;
        return true;
    }

    /** parse, check and load configuration, it prepare for dtor or startup */
    bool pe4initest::reload() {
        destroy();
        MLOG_DEBUG << this << "->pe4initest::reload execute!" << endl;
        return true;
    }

    /** start work */
    bool pe4initest::startup() {
        MLOG_DEBUG << this << "->pe4initest::startup execute!" << endl;
        return true;
    }

    /** end work */
    void pe4initest::shutdown() {
        MLOG_DEBUG << this << "->pe4initest::shutdown execute!" << endl;
    }

    /** invoke by dtor and reload */
    void pe4initest::destroy() {
        MLOG_DEBUG << this << "->pe4initest::destroy execute!" << endl;
    }

    /** Hadoop:Partitioner:: public int getPartition(K key, V value, int numReduceTasks) */
    bool pe4initest::partition(const event& e) const {
        MLOG_DEBUG << this << "->pe4initest::partition(" << &e << ") execute!" << endl;
        return true;
    }

    void pe4initest::push(const event& e) {
        ++push_counter_;
        MLOG_DEBUG << this << "->pe4initest::push(" << &e << ") execute!" << endl;
    }

    void pe4initest::reset_counter() const {
        push_counter_ = 0;
        pop_counter_ = 0;
        time_ms_ = processing_utils::systime_msec();
        MLOG_DEBUG << this << "->pe4initest::reset_counter execute!" << endl;
    }

    unsigned long pe4initest::push_counter() const {
        MLOG_DEBUG << this << "->pe4initest::push_counter execute!" << endl;
        return push_counter_;
    }

    unsigned long pe4initest::pop_counter() const {
        MLOG_DEBUG << this << "->pe4initest::pop_counter execute!" << endl;
        return pop_counter_;
    }

    double pe4initest::push_speed() const {
        MLOG_DEBUG << this << "->pe4initest::push_speed execute!" << endl;
        return (push_counter_ * (double) 1000) / (processing_utils::systime_msec() - time_ms_);
    }

    double pe4initest::pop_speed() const {
        MLOG_DEBUG << this << "->pe4initest::pop_speed execute!" << endl;
        return (pop_counter_ * (double) 1000) / (processing_utils::systime_msec() - time_ms_);
    }

    void pe4initest::logging() const {
        MLOG_DEBUG << this << "->pe4initest::logging execute!" << endl;
    }
}
