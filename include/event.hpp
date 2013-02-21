/* 
 * File:   event.hpp
 * Author: WG
 *
 * Created on 2012-01-19 AM 09:29
 */

#ifndef EVENT_HPP
#define	EVENT_HPP

#include <string>
// #include <vector>

#include <map>
#include <set>
// #include "CEventRouter.h"

#include "dynamic_struct.hpp"
//using namespace zsmart::cep::communication;
//
//namespace cep {
//    struct EVENT_HEAD_MESSAGE;
//}
#include "const_dataset.hpp"
// #include "buffer_function.hpp"

// using std::vector;
using std::map;
using std::set;

namespace cep {

    // class ref_dataset;
    class type_slice;

    /*
     * event 用来生成我们的自描述的数据 event包含了数据 和描述数据的结构<元数据>  以及其他 数据和数据的长度是不可以更改的 元数据，数据的类型是可以更改的。
     * 所以原则是数据不变性，数据一旦确定就不会更改。
     */
    class event {
    public:
        typedef dynamic_struct::type_t type_t;
        typedef unsigned short int pe_id_t;

        /** support for COUNT(DISTINCT 'single' field content), please see dtor! */
        typedef set<type_slice, cep::less> distinct_set_t;
        // typedef set<type_slice> distinct_set_t;
        /** support for multi-field COUNT(DISTINCT single field content) */
        typedef map<size_t, distinct_set_t> count_map_t;

        /**
         * event的内存是自己malloc或者是别人设置进去的
         * 自己malloc就必须要提供metadata所以提供了metadata的构造函数内存就是自己malloc的
         * 提供了body, len的内存就是设置进去的。
         */
        event(const buffer_metadata * metadata)
        : ds_(metadata), pe_id_(0), seq_no_(0), /*router_list_(),*/ dataset_(NULL)
        , count_distinct_(NULL) {
        }

        event(const buffer_metadata * metadata, type_t type)
        : ds_(metadata, type), pe_id_(0), seq_no_(0), /*router_list_(),*/ dataset_(NULL)
        , count_distinct_(NULL) {
        }

        event(char *body, unsigned int len)
        : ds_(body, len), pe_id_(0), seq_no_(0), /*router_list_(),*/ dataset_(NULL)
        , count_distinct_(NULL) {
        }

        event(char *body, unsigned int len, type_t type)
        : ds_(body, len, type), pe_id_(0), seq_no_(0), /*router_list_(),*/ dataset_(NULL)
        , count_distinct_(NULL) {
        }

        event(const event& evt)
        : pe_id_(evt.pe_id_), seq_no_(evt.seq_no_), ds_(evt.ds_)
        , /*router_list_(),*/ dataset_(NULL), count_distinct_(NULL) {
            if (evt.dataset_ != NULL)
                set_dataset(new ref_dataset(*(evt.dataset_)));
            // Ignore count_distinct_ directly!
            /*if (evt.count_distinct_ != NULL) {
                count_map_t* cmap = new count_map_t();
                set_count_distinct(cmap);
                count_map_t::const_iterator itr = evt.count_distinct_->begin();
                count_map_t::const_iterator end = evt.count_distinct_->end();
                distinct_set_t::const_iterator itr2, end2;
                pair < count_map_t::iterator, bool> ret;
                for (; itr != end; ++itr) {
                    ret = cmap->insert(count_map_t::value_type(itr->first, distinct_set_t()));
                    if (ret.second) {
                        itr2 = itr->second.begin();
                        end2 = itr->second.end();
                        for (; itr2 != end2; ++itr2) {
                            // delete[] itr2->data.data();
                            char* chars = new char[itr2->data.size()];
                            memcpy(chars, itr2->data.data(), itr2->data.size());
                            struct type_slice ts = {ts.type = itr2->type, ts.data = slice(chars, itr2->data.size())};
                            ret.first->second.insert(ts);
                        }
                    } // else // ignore!!!
                }
            }*/
            // count_distinct_(NULL);
            count_distinct_ = NULL;
        }

        ~event() {
            if (dataset_ != NULL) delete dataset_;
            if (count_distinct_ != NULL) set_count_distinct(NULL);
        }
    public:

        template<typename T>
        T value(const std::string& name, T default_value)const {
            return ds_.value(name, default_value);
        }

        template<typename T>
        T value(const size_t index, T default_value)const {
            return ds_.value(index, default_value);
        }

        /*
         * @Deprecated
         */
        template<typename T>
        T value(const size_t offset, const size_t len, T default_value)const {
            return ds_.value(offset, len, default_value);
        }

        template<typename T>
        void set_value(const std::string& name, T value) {
            ds_.set_value(name, value);
        }

        template<typename T>
        void set_value(const size_t index, T value) {
            ds_.set_value(index, value);
        }

        /*
         * @Deprecated
         */
        template<typename T>
        void set_value(const size_t offset, const size_t len, T value) {
            ds_.set_value(offset, len, value);
        }

        template<typename T>
        void reference(const size_t index, T** value) {
            ds_.reference(index, value);
        }

        template<typename T>
        void reference(const std::string& name, T** value) {
            ds_.reference(name, value);
        }

        /*
         * @Deprecated
         */
        template<typename T>
        void reference(const size_t offset, const size_t len, T** value) {
            ds_.reference(offset, len, value);
        }

        void set_type(type_t type) {
            ds_.set_type(type);
        }

        type_t type() const {
            return ds_.type();
        }

        unsigned int len() const {
            return ds_.len();
        }

        void set_metadata(buffer_metadata * metadata) {
            ds_.set_metadata(metadata);
        }

        const buffer_metadata * metadata() const {
            return ds_.metadata();
        }

        const char* data() const {
            return ds_.data();
        }

        unsigned long get_seqno() const {
            return seq_no_;
        }

        void set_seqno(unsigned long seq_no) const {
            seq_no_ = seq_no;
        }

        pe_id_t pe_id() const {
            return pe_id_;
        }

        void set_pe_id(const pe_id_t pe_id) const {
            pe_id_ = pe_id;
        }

        //        vector<string>& router_list() {
        //            return router_list_;
        //        }
        //
        //        const

        /**
         * Do not use as:
         * vector<string> routers = evt.router_list();
         * routers.push_back("pn_001");
         * Please use it as:
         * evt.router_list().push_back("pn_001");
         * or
         * vector<string> *ptr_routers = &(evt.router_list());
         * ptr_routers->push_back("pn_001");
         * 
         * @return a vector<string> reference
        vector<string>& router_list() const {
            return router_list_;
        }
         */

        /**
         * memory management warning!!!
         */
        void setNullData() {
            ds_.set_data(NULL, 0);
        }

        void set_dataset(ref_dataset* dataset) const {
            if (dataset_ == dataset) return;
            if (dataset_ != NULL) delete dataset_;
            dataset_ = dataset;
        }

        ref_dataset* release_dataset() const {
            ref_dataset* rd = dataset_;
            dataset_ = NULL;
            return rd;
        }

        ref_dataset* dataset() const {
            return dataset_;
        }

        void set_count_distinct(count_map_t* count_distinct) {
            if (count_distinct_ == count_distinct) return;
            if (count_distinct_ != NULL) {
                count_map_t::const_iterator itr = count_distinct_->begin();
                count_map_t::const_iterator end = count_distinct_->end();
                distinct_set_t::const_iterator itr2, end2;
                for (; itr != end; ++itr) {
                    itr2 = itr->second.begin();
                    end2 = itr->second.end();
                    for (; itr2 != end2; ++itr2) {
                        delete[] itr2->data.data();
                    }
                }
                delete count_distinct_;
            }
            count_distinct_ = count_distinct;
        }

        count_map_t* release_count_distinct() {
            count_map_t* cd = count_distinct_;
            count_distinct_ = NULL;
            return cd;
        }

        count_map_t* count_distinct() {
            return count_distinct_;
        }
    private:
        mutable pe_id_t pe_id_;
        mutable unsigned long seq_no_;
        mutable ref_dataset* dataset_;

        count_map_t* count_distinct_;

        dynamic_struct ds_;
        // mutable vector<string> router_list_;
    };
}

#endif	/* EVENT_HPP */
