/* 
 * File:   pe.hpp
 * Author: WangquN
 *
 * Created on 2012-02-17 AM 09:24
 */

#ifndef PROCESSING_PE_HPP
#define	PROCESSING_PE_HPP

#include "macro_log.hpp"
#include "utils_xml.hpp"
#include "processing/pn.hpp"

namespace cep {
    class event;
    // class xml_element; // error: using typedef-name ‘cep::xml_element’ after ‘class’
    // class pn::pn_id_t;

    class pe {
    public:
        typedef unsigned short int pe_id_t;
        static const pe_id_t INIT_PE_ID = 0; // constant declaration // = 0 is good in g++

        pe(const xml_element& config, const xml_element& main_config)
        : config_(&config), main_config_(&main_config) {
        };

        virtual ~pe() {
            // if (config_ != NULL) delete config_;
        };
        // virtual ~pe() = 0;
        // You'd better declare the deconstructor a vitaul member function,
        // other than pure virtual member function,
        // unless you not have pure virtual member function in your base class
        // and you don't make the class instantiated.
    private: // Forbidden copy behave

        pe(const pe& p) /*: id_(0)*/ {
            MLOG_TRACE << this << "->pe copy from[" << &p << "] ctor execute!" << endl;
        }

        pe& operator=(const pe& p) {
            if (this == &p) // effective c++ 16
                return *this;
            MLOG_TRACE << this << "->pe assignment operator(" << &p << ") execute!" << endl;
            return *this;
        }
    public:

        pe_id_t id() const {
            return id_;
        }

        pn::pn_id_t pn_id() const {
            return pn_id_;
        }
    public:
        /** const or no resource properties initialization */
        virtual bool init() = 0;
        /** parse, check and load configuration, it prepare for dtor or startup */
        virtual bool reload() = 0;
        /** start work */
        virtual bool startup() = 0;
        /** end work */
        virtual void shutdown() = 0;
    protected:
        /** invoke by dtor and reload */
        virtual void destroy() = 0;
    public:
        /** Hadoop:Partitioner:: public int getPartition(K key, V value, int numReduceTasks) */
        virtual bool partition(const event&) const = 0;
        virtual void push(const event&) = 0;
    public:
        virtual void reset_counter() const = 0;

        virtual unsigned long push_counter() const = 0;
        virtual unsigned long pop_counter() const = 0;
        virtual double push_speed() const = 0;
        virtual double pop_speed() const = 0;

        virtual void logging() const = 0;
    protected:
        // friend std::ostream& operator<<(std::ostream&, const pe&);
        /*const*/pe_id_t id_;
        pn::pn_id_t pn_id_;
        const xml_element *config_, *main_config_;
    };

    // pe::~pe() {} // pure virtual dtor must be implement
    // error: multiple definition of `cep::pe::~pe()'
}

#endif	/* PROCESSING_PE_HPP */
