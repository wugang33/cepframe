/* 
 * File:   idx_rbtree.cpp
 * Author: WangquN
 *
 * Created on 2012-02-14 AM 10:51
 */

#include "processing/idx_rbtree.hpp"

#include "buffer_metadata.hpp"
#include "buffer_utility.hpp"
#include "dynamic_struct.hpp"
#include "macro_log.hpp"

#include "processing/utils.hpp"

using namespace std;

namespace cep {

    idx_rbtree::idx_rbtree(const string& id, const vector<string>& keys, const const_dataset::table& tbl)
    : const_dataset::index(id, keys, tbl), map_() {
    }

    /* virtual */
    idx_rbtree::~idx_rbtree() {
        free();
    }
    ////////////////////////////////////////////////////////////////////////////

    /* virtual */
    bool idx_rbtree::rebuild() {
        free();
        for (int i = 0; i < size_; ++i) { // check the idx_keys_
            if (table_->metadata().get_cfg_entry(*(idx_keys_ + i)) == NULL) {
                MLOG_ERROR << this << "->idx_rbtree::rebuild use idx[" << *(idx_keys_ + i)
                        << "] not to found field meta data in:" << table_->metadata() << endl;
                return false;
            }
        }
        type_slice key;
        if (size_ > 1) key.type = buffer_metadata::STR;
        else key.type = table_->metadata().get_cfg_entry(*idx_keys_)->type();
        type_slice keys[size_];
        slice keys_slice[size_];
        const dynamic_struct* row;
        size_t idx;
        slice default_data;
        int len;
        const const_dataset::table::rows_t::size_type size = table_->size();
        for (int i = 0; i < size; ++i) {
            // key.data = processing_utils::encoding(idx_keys_, size_, ':', *(table_->get(i)));
            // empty string is not as a index value! add by waq@2011-08-31
            // if (key.data.size() == 0) {
            //     delete[] key.data.data();
            //     continue;
            // }
            row = table_->get(i);
            for (int j = 0; j < size_; ++j) {
                idx = *(idx_keys_ + j);
                keys[j].type = table_->metadata().get_cfg_entry(idx)->type();
                keys[j].data = row->value(idx, default_data);
            }
            len = processing_utils::encoding2slice(keys, size_, 1, keys_slice);
            if (len < 1) {
                // maybe error or
                // empty string is not as a index value! add by waq@2011-08-31
                continue;
            }
            char* key_buff = new char[len];
            processing_utils::encoding(keys_slice, size_, key_buff, len, ':');
            key.data = slice(key_buff, len);
            map_.insert(slice_map_t::value_type(key, i)); // must insert successful
            // MLOG_DEBUG << this << "->idx_rbtree::rebuild insert[" << i << "]=" << key << endl;
        }
        return true;
    }

    /* virtual */
    void idx_rbtree::select(const type_slice where[], const_dataset::index::iterator& func) const {
        type_slice key;
        /*if (row.metadata() != &(table_->metadata())) {
            MLOG_WARN << this << "->idx_rbtree::select use row meta data[" << row.metadata()
                    << "] do not match index meta data:" << table_->metadata() << endl;
            return;
        }*/
        if (size_ > 1) key.type = buffer_metadata::STR;
        else // do not check again!!!
            key.type = table_->metadata().get_cfg_entry(*idx_keys_)->type();
        // key.data = processing_utils::encoding(idx_keys_, size_, ':', row);
        slice keys_slice[size_];
        int len = processing_utils::encoding2slice(where, size_, 1, keys_slice);
        if (len < 1) return; // empty string is not as a index value! add by waq@2011-08-31
        char* key_buff = new char[len];
        processing_utils::encoding(keys_slice, size_, key_buff, len, ':');
        key.data = slice(key_buff, len);

        // cout << "idx_rbtree::select args:" << key << endl;
        pair<slice_map_t::const_iterator, slice_map_t::const_iterator> ret = map_.equal_range(key);
        // MLOG_DEBUG << this << "->idx_rbtree::select key=" << key
        //         << (ret.first == ret.second ? " not " : " ") << "found!" << endl;
        delete[] key.data.data();

        const_dataset::table::rows_t::size_type rownum = 0;
        for (slice_map_t::const_iterator itr = ret.first; itr != ret.second; ++itr, ++rownum) {
            // MLOG_DEBUG << rownum << '>' << *(table_->get(itr->second)) << endl;
            if (!func(*(table_->get(itr->second)), rownum)) // table cant contains null
                break;
        }
#if MLOG_LEVEL < 20 && 0 // debug
        cout << this << "->idx_rbtree::select index show all:" << endl;
        slice_map_t::const_iterator itr = map_.begin(), end = map_.end();
        for (; itr != end; ++itr) {
            cout << itr->first << '=' << itr->second << endl;
        }
#endif
    }

    void idx_rbtree::free() {
        slice_map_t::const_iterator itr = map_.begin(), end = map_.end();
        for (; itr != end; ++itr)
            delete[] itr->first.data.data();
        map_.clear();
    }

    ostream& operator<<(ostream& s, const idx_rbtree& r) {
        s << "idx_rbtree@" << *((const_dataset::index*) (&r)) << " size is:" << r.map_.size();
        idx_rbtree::slice_map_t::const_iterator itr = r.map_.begin();
        idx_rbtree::slice_map_t::const_iterator end = r.map_.end();
        for (; itr != end; ++itr)
            s << '\n' << itr->first << '=' << itr->second;
        return s;
    }
}
