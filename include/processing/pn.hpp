/* 
 * File:   pn.hpp
 * Author: WangquN
 *
 * Created on 2012-02-04 12:11
 */

#ifndef PROCESSING_PN_HPP
#define	PROCESSING_PN_HPP

#include <ostream>
#include <map>
#include <set>
#include <boost/thread/mutex.hpp>
#include <boost/threadpool.hpp>

#include "macro_log.hpp"
#include "singleton.hpp"
#include "utils_xml.hpp"
#include "dlhandler.hpp"
// #include "processing/pe.hpp"
#include "processing/router.hpp"
#include "processing/plugin_function.hpp"

// using namespace std;
using std::string;
using std::endl;
using std::map;
using std::set;

namespace cep {
    class event;
    // class xml_element; // error: using typedef-name ‘cep::xml_element’ after ‘class’
    class pe;
    // class pe::pe_id_t;
    // class router;

    class pn : public cep::singleton<pn> {
        friend class cep::singleton<pn>;
    public:
        // typedef string pn_id_t; // support new network tools
        typedef unsigned short pn_id_t;
        // typedef map<pe::pe_id_t, pe*> pe_id2pe_map_t;
        // typedef unsigned short int pe_id_t;
        typedef map<router::pe_id_t, pe*> pe_id2pe_map_t;
        ~pn();
    private: // singleton pattern // protected
        pn(); // must have a default constructor // id_("default_pn_id")

        pn(pn& arg) { // Forbidden copy behave
            MLOG_INFO << this << "->pn copy from[" << &arg << "] ctor execute!" << endl;
        }

        pn& operator=(const pn& arg) { // Forbidden copy behave
            if (this == &arg) // effective c++ 16
                return *this;
            MLOG_INFO << this << "->pn assignment operator(" << &arg << ") execute!" << endl;
            return *this;
        }
    public:
        /** const or must not failure properties initialization */
        void init(const pn_id_t& id, const string& url = "");
        /** parse, check and load configuration, it prepare for dtor or startup */
        bool reload(const xml_element& config);
        /** start work */
        bool startup();
        /** end work */
        void shutdown();
    private:
        /** invoke by dtor and reload */
        void destroy();
    public:

        pn_id_t id() const {
            return id_;
        }

        string url() const {
            return url_;
        }
    public:
        void push(const event&);
        /** routing to others pe after the current pe processed */
        void distribute4processed(const event&, const set<router::pe_id_t>* optinal_pe_id_set = NULL) const;
        /** routing to others pe at the same time of current pe receiving */
        void distribute4received(const event&, const set<router::pe_id_t>* optinal_pe_id_set = NULL) const;

        /**
         * not delete event memory version:
         * routing to others pe after the current pe processed
         */
        bool processed_distribute(const event&, const set<router::pe_id_t>* optinal_pe_id_set = NULL) const;
        /**
         * not delete event memory version:
         * routing to others pe at the same time of current pe receiving
         */
        bool received_distribute(const event&, const set<router::pe_id_t>* optinal_pe_id_set = NULL) const;
    public:
        /** uniform schedule asynchronous task in pool */
        bool schedule(const boost::threadpool::pool::task_type& task);

        dlhandler::factory& pe_factory() {
            return pe_factory_;
        }

        dlhandler::factory& plugin_func_factory() {
            return plugin_func_factory_;
        };

        /** uniform function plug-in container */
        plugin_function& plugin_funcs() {
            return plugin_funcs_;
        }
    public:
        void reset_counter() const;

        unsigned long push_counter() const;
        unsigned long pop_counter() const;
        double push_speed() const;
        double pop_speed() const;

        void logging() const;
    private:
        friend std::ostream& operator<<(std::ostream&, const pn&);
        mutable unsigned long push_counter_, pop_counter_;
        mutable unsigned long time_ms_;

        boost::mutex mutex_;
        volatile bool running_;
        pn_id_t id_;
        // const xml_element *config_;
        /** configuration server URL or local configuration file path */
        string url_;

        boost::threadpool::pool workers_;
        router router_;
        pe_id2pe_map_t local_pes_, remote_pes_;
        dlhandler::factory pe_factory_, plugin_func_factory_;
        plugin_function plugin_funcs_;
    };
}

#endif	/* PROCESSING_PN_HPP */
