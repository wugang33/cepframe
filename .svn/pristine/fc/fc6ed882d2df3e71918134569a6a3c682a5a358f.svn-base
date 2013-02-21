/* 
 * The const dataset do not have any modify data function.
 * It can high speed to query data by many kinds of index!
 * 
 * File:   processing/const_dataset.hpp
 * Author: WangquN
 *
 * Created on 2012-02-04 PM 04:39
 */
#include "const_dataset.hpp"

#include "buffer_metadata.hpp"
#include "buffer_utility.hpp"
#include "dynamic_struct.hpp"
// #include "buffer_function.hpp"

using namespace std;

namespace cep {

    const_dataset::index::index(const string& id, const vector<string>& keys, const const_dataset::table& tbl)
    : idx_(0), size_(keys.size()), table_(&tbl), id_(id) {
        if (size_ < 1) MLOG_ERROR << this << "->index ctor has no keys!" << endl;
        else {
            idx_keys_ = new size_t[size_];
            vector<string>::const_iterator itr = keys.begin(), end = keys.end();
            const buffer_metadata* metadata = &(table_->metadata());
            const field_cfg_entry* entry;
            for (int i = 0; itr != end; ++itr, ++i) {
                entry = metadata->get_cfg_entry(*itr);
                if (entry == NULL) {
                    MLOG_ERROR << this << "->index ctor use code[" << *itr
                            << "] not to found field meta data in:" << *metadata << endl;
                    idx_keys_[i] = -1;
                } else
                    idx_keys_[i] = entry->idx();
            }
            /*vector<string>::const_iterator found, itr = keys.begin(), end = keys.end();
            buffer_metadata::cfg_entry_list* list = &(table_->metadata().get_entry_list());
            int idx = 0;
            for (int i = list->size() - 1; i >= 0; --i) {
                found = std::find(begin, end, (*list)[i]->code);
                if (found != end) {
                }
            }*/
        }
    }

    /* virtual */
    const_dataset::index::~index() { // = 0;
        delete[] idx_keys_;
    }

    ostream& operator<<(ostream& s, const const_dataset::index& r) {
        s << &r << "->" << r.id_ << '[' << r.idx_ << "]@" << r.table_ << "->"
                << r.table_->id() << '[' << r.table_->idx() << "] index by:";
        size_t idx;
        for (int i = 0; i < r.size_; ++i) {
            idx = *(r.idx_keys_ + i);
            s << r.table_->metadata().get_cfg_entry(idx)->code();
            if (i + 1 < r.size_) s << ',';
        }
        return s;
    }
    ////////////////////////////////////////////////////////////////////////////

    const_dataset::table::table(const string& id, const buffer_metadata& metadata)
    : id_(id), metadata_(&metadata), rows_(), idx_(0) {
    }

    const_dataset::table::~table() {
        clear();
    }

    const dynamic_struct* const_dataset::table::get(const rows_t::size_type row_nbr) const {
        if (row_nbr < rows_.size())
            // return (const dynamic_struct*) rows_[row_nbr];
            return const_cast<dynamic_struct*> (rows_[row_nbr]);
        else
            return NULL;
    }

    bool const_dataset::table::exists(const dynamic_struct& row) const {
        // dynamic_struct *r;
        for (int i = rows_.size() - 1; i >= 0; --i) {
            /*r = rows_[i];
            if (r->len() == row.len()
                    && memcmp(r->data(), row.data(), r->len()) == 0)
                return true;*/
            if (row == *rows_[i]) return true;
        }
        return false;
    }

    bool const_dataset::table::insert(dynamic_struct* row) {
        if (row && row->metadata() == metadata_) {
            rows_.push_back(row);
            return true;
        } else
            return false;
    }

    void const_dataset::table::clear() {
        for (int i = rows_.size() - 1; i >= 0; --i) delete rows_[i];
        rows_.clear();
    }

    std::ostream& operator<<(std::ostream& s, const const_dataset::table& r) {
        const_dataset::table::rows_t::size_type size = r.rows_.size();
        s << &r << "->" << r.id_ << '[' << r.idx_ << "]::size=" << size << ", metadata=\n";
        if (r.metadata_) s << *(r.metadata_);
        else s << "NULL";
        s << "\ndata=";
        for (int i = 0; i < size; ++i) {
            // if (r.rows_[i]) buff_util::to_hex_string(r.rows_[i]->data(), r.rows_[i]->len(), s);
            // else s << "NULL";
            buff_util::to_hex_string(r.rows_[i]->data(), r.rows_[i]->len(), s);
            if (i + 1 < size) s << '\n';
        }
        return s; // << '\n';
    }
    ////////////////////////////////////////////////////////////////////////////

    const_dataset::const_dataset() : tables_(), id_map_table_(), indexes_(), id_map_index_() {
    }

    const_dataset::~const_dataset() {
        clear();
    }

    const_dataset::table* const_dataset::get_table(const string& table_id) {
        id_map_table_t::const_iterator found = id_map_table_.find(table_id);
        if (found != id_map_table_.end()) return get_table(found->second);
        else return NULL;
    }

    const const_dataset::table* const_dataset::get_table(const string& table_id) const {
        // return get_table(table_id);
        return const_cast<const_dataset*> (this)->get_table(table_id);
    }

    const_dataset::table* const_dataset::get_table(
            const const_dataset::tables_t::size_type table_idx) {
        if (table_idx < tables_.size()) return tables_[table_idx];
        else return NULL;
    }

    const const_dataset::table* const_dataset::get_table(
            const const_dataset::tables_t::size_type table_idx) const {
        // return get_table(table_idx);
        return const_cast<const_dataset*> (this)->get_table(table_idx);
    }

    const_dataset::index* const_dataset::get_index(const string& index_id) {
        id_map_index_t::const_iterator found = id_map_index_.find(index_id);
        if (found != id_map_index_.end()) return get_index(found->second);
        else return NULL;
    }

    const const_dataset::index* const_dataset::get_index(const string& index_id) const {
        // return get_index(index_id);
        return const_cast<const_dataset*> (this)->get_index(index_id);
    }

    const_dataset::index* const_dataset::get_index(
            const const_dataset::indexes_t::size_type index_idx) {
        if (index_idx < indexes_.size()) return indexes_[index_idx];
        else return NULL;
    }

    const const_dataset::index* const_dataset::get_index(
            const const_dataset::indexes_t::size_type index_idx) const {
        // return get_index(index_idx);
        return const_cast<const_dataset*> (this)->get_index(index_idx);
    }

    const_dataset::table* const_dataset::insert(const string& table_id,
            const buffer_metadata& metadata) {
        const_dataset::table* tbl = get_table(table_id);
        if (tbl == NULL) {
            tbl = new const_dataset::table(table_id, metadata);
            tbl->set_idx(tables_.size());
            id_map_table_.insert(id_map_table_t::value_type(table_id, tbl->idx()));
            tables_.push_back(tbl);
        }
        return tbl;
    }

    /**
     * This API need to compatible with all type index
     * 
     * @param 
     * @return false need to manage argument's memory
     */
    bool const_dataset::insert(const_dataset::index* idx) {
        if (idx == NULL) return true;
        const_dataset::index* idx2 = get_index(idx->id());
        if (idx2 == NULL) {
            idx->set_idx(indexes_.size());
            id_map_index_.insert(id_map_index_t::value_type(idx->id(), idx->idx()));
            indexes_.push_back(idx);
            return true;
        } else
            return false;
    }

    void const_dataset::clear() {
        for (int i = indexes_.size() - 1; i >= 0; --i) delete indexes_[i];
        for (int i = tables_.size() - 1; i >= 0; --i) delete tables_[i];

        tables_.clear();
        id_map_table_.clear();

        indexes_.clear();
        id_map_index_.clear();
    }

    ostream& operator<<(ostream& s, const const_dataset& r) {
        s << &r << "->tables:\n";
        const_dataset::id_map_table_t::const_iterator itr = r.id_map_table_.begin();
        const_dataset::id_map_table_t::const_iterator end = r.id_map_table_.end();
        for (; itr != end; ++itr)
            s << itr->first << '=' << r.tables_[itr->second] << '\n';
        s << " indexes:";
        const_dataset::id_map_index_t::const_iterator itr2 = r.id_map_index_.begin();
        const_dataset::id_map_index_t::const_iterator end2 = r.id_map_index_.end();
        for (; itr2 != end2; ++itr2) {
            s << '\n' << itr2->first << '=' << r.indexes_[itr2->second]; // << '\n';
            // if (itr2 + 1 != end2) s << '\n'; // error: no match for ‘operator+’ in ‘itr + 1’
        }
        return s;
    }
    ////////////////////////////////////////////////////////////////////////////
    // ref_dataset /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    ref_dataset::metadata::impl_t::~impl_t() {
        for (int i = entry_list_.size() - 1; i >= 0; --i) delete entry_list_[i];
    }

    /** return false need to manage argument's memory */
    bool ref_dataset::metadata::impl_t::add_entry(field_t* field) {
        pair < entry_map_t::iterator, bool> rst = entry_map_.insert(
                entry_map_t::value_type(field->code, field));
        if (rst.second) {
            // rst.first->second.idx = entry_list_.size();
            field->idx = entry_list_.size();
            entry_list_.push_back(field);
            return true;
        } else
            return false;
    }

    std::ostream& operator<<(std::ostream& s, const ref_dataset::metadata::impl_t& r) {
        ref_dataset::metadata::impl_t::entry_list_t::size_type size = r.entry_list_.size();
        s << &r << "::size=" << size << '\n';
        const ref_dataset::metadata::impl_t::field_t* field;
        for (int i = 0; i < size; ++i) {
            field = r.entry_list_[i];
            s << i << '>' << field << "->" << buffer_metadata::get_type_by_id(field->type);
            s << ':' << field->code << '@' << field->idx << ' ';
        }
        return s;
    }

    ref_dataset::metadata::metadata() : metadatas_(), id_map_idx_() {
    }

    ref_dataset::metadata::~metadata() {
        clear();
    }

    int ref_dataset::metadata::get(const string& id) const {
        idx_map_t::const_iterator found = id_map_idx_.find(id);
        if (found != id_map_idx_.end())
            return found->second;
        else
            return -1;
    }

    ref_dataset::metadata::impl_t* ref_dataset::metadata::get(
            const ref_dataset::metadata::metadatas_t::size_type idx) const {
        if (idx < metadatas_.size()) return metadatas_[idx];
        else return NULL;
    }

    /*void ref_dataset::metadata::get_ids(vector<string>& ids) const {
        ids.clear();
        ids.reserve(id_map_idx_.size());
        // ids.resize(id_map_idx_.size());
        idx_map_t::const_iterator itr = id_map_idx_.begin();
        idx_map_t::const_iterator end = id_map_idx_.end();
        // for (; itr != end; ++itr) ids[itr->second] = itr->first;
        for (; itr != end; ++itr) ids.push_back(itr->first);
    }*/

    bool ref_dataset::metadata::insert(const string& id,
            ref_dataset::metadata::impl_t* metadata) {
        if (metadata == NULL) return true;
        int idx = get(id);
        if (idx < 0) { // id not found
            const metadatas_t::size_type size = metadatas_.size();
            /*for (int i = 0; i < size; ++i)
                if (metadatas_[i] == metadata) idx = i;
            if (idx < 0) { // not such meta data object*/
            idx = size;
            metadatas_.push_back(metadata);
            // do not reuse it idx
            // } // if have this meta data object and id reuse it idx
            id_map_idx_.insert(idx_map_t::value_type(id, idx));
            return true;
        } else
            return false;
    }

    void ref_dataset::metadata::clear() {
        for (int i = metadatas_.size() - 1; i >= 0; --i) delete metadatas_[i];
        metadatas_.clear();
        id_map_idx_.clear();
    }

    ostream& operator<<(ostream& s, const ref_dataset::metadata& r) {
        s << &r << "::size=" << r.metadatas_.size() << "{\n";
        ref_dataset::metadata::idx_map_t::const_iterator itr = r.id_map_idx_.begin();
        ref_dataset::metadata::idx_map_t::const_iterator end = r.id_map_idx_.end();
        // ref_dataset::metadata::impl_t::const_iterator itr2, end2;
        ref_dataset::metadata::metadatas_t::size_type idx;
        for (; itr != end; ++itr) {
            idx = itr->second;
            s << itr->first << '[' << idx << "]={";
            /*itr2 = r.metadatas_[idx]->begin();
            end2 = r.metadatas_[idx]->end();
            if (itr2 != end2)
                s << itr2->first << '=' << itr2->second;
            for (++itr2; itr2 != end2; ++itr2)
                s << ',' << itr2->first << '=' << itr2->second;*/
            s << *(r.metadatas_[idx]);
            s << "}\n";
        }
        return s << '}';
    }
    ////////////////////////////////////////////////////////////////////////////

    ref_dataset::record::record(const recordset& records)
    : recordset_(&records), cells_(records.col_size()) {
    }

    type_slice ref_dataset::record::get(const string& col_name, const type_slice& default_value) const {
        /*ref_dataset::recordset::metadata_t::const_iterator found = recordset_->metadata_->find(col_name);
        if (found != recordset_->metadata_->end())
            return get(found->second, default_value);
        else
            return default_value;*/
        const ref_dataset::recordset::metadata_t::field_t* field =
                recordset_->metadata_->entry(col_name);
        if (field != NULL) return get(field->idx, default_value);
        else return default_value;
    }

    type_slice ref_dataset::record::get(const cells_t::size_type col_idx, const type_slice& default_value) const {
        if (col_idx < cells_.size()) {
            const ref_dataset::recordset::metadata_t::field_t* field =
                    recordset_->metadata_->entry(col_idx);
            /*if (field == NULL) {
                MLOG_WARN << this << "->ref_dataset::record::get(" << col_idx
                        << ", " << default_value << ") not found meta data!" << endl;
                return default_value;
            } else*/
            struct type_slice rst = {field->type, cells_[col_idx]};
            return rst;
        } else return default_value;
    }

    bool ref_dataset::record::set(const string& col_name, const type_slice& cell_value) {
        /*ref_dataset::recordset::metadata_t::const_iterator found
                = recordset_->metadata_->find(col_name);
        if (found != recordset_->metadata_->end())
            return set(found->second, cell_value);
        else
            return false;*/
        const ref_dataset::recordset::metadata_t::field_t* field =
                recordset_->metadata_->entry(col_name);
        if (field != NULL && field->type == cell_value.type) {
            cells_[field->idx] = cell_value.data;
            return true;
        } else return false;
    }

    bool ref_dataset::record::set(const cells_t::size_type col_idx, const type_slice& cell_value) {
        /*if (col_idx < cells_.size()) {
            cells_[col_idx] = cell_value;
            return true;
        } else
            return false;*/
        const ref_dataset::recordset::metadata_t::field_t* field =
                recordset_->metadata_->entry(col_idx);
        if (field != NULL && field->type == cell_value.type) {
            cells_[field->idx] = cell_value.data;
            return true;
        } else return false;
    }

    std::ostream& operator<<(std::ostream& s, const ref_dataset::record& r) {
        ref_dataset::record::cells_t::size_type size = r.cells_.size();
        const ref_dataset::metadata::impl_t* metadata = &(r.recordset_->get_metadata());
        s << &r << "->in recordset@";
        if (r.recordset_ == NULL) s << "NULL\n";
        else s << r.recordset_ << '\n';
        /*
        s << &r << "->metadata@";
        if (metadata == NULL) s << "NULL\n";
        else s << metadata << '\n';
        for (int i = 0; i < size; ++i) {
            s << i << '@' << (void*) r.cells_[i].data() << ':';
            buff_util::to_hex_string(r.cells_[i].data(), r.cells_[i].size(), s);
            s << '\n';
        }
         */
        for (int i = 0; i < size; ++i) {
            s << i << '<' << buffer_metadata::get_type_by_id(metadata->entry(i)->type)
                    << '@' << (void*) r.cells_[i].data() << ':';
            buff_util::to_hex_string(r.cells_[i].data(), r.cells_[i].size(), s);
            if (i + 1 < size) s << '\n';
        }
        return s;
    }
    ////////////////////////////////////////////////////////////////////////////

    ref_dataset::recordset::recordset(const string& id,
            const ref_dataset::recordset::metadata_t& metadata)
    : id_(id), metadata_(&metadata), rows_(), idx_(0) {
    }

    ref_dataset::recordset::~recordset() {
        clear();
    }

    ref_dataset::recordset::recordset(const ref_dataset::recordset& rcdset)
    : idx_(rcdset.idx_), metadata_(rcdset.metadata_), id_(rcdset.id_), rows_() {
        // rows_ = rcdset.rows_;
        const size_t size = rcdset.rows_.size();
        rows_.resize(size);
        for (int i = 0; i < size; ++i) {
            // delete rows_[i];
            rows_[i] = new record(*(rcdset.rows_[i]));
            rows_[i]->recordset_ = this; // Be attention please!
        }
    }

    ref_dataset::recordset& ref_dataset::recordset::operator=(const ref_dataset::recordset& rcdset) {
        if (this == &rcdset) return *this;
        clear();
        idx_ = rcdset.idx_;
        metadata_ = rcdset.metadata_;
        id_ = rcdset.id_;
        // rows_ = rcdset.rows_;
        const size_t size = rcdset.rows_.size();
        rows_.resize(size);
        for (int i = 0; i < size; ++i) {
            // delete rows_[i];
            rows_[i] = new record(*(rcdset.rows_[i]));
        }
        return *this;
    }

    const ref_dataset::record* ref_dataset::recordset::get(const rows_t::size_type row_nbr) const {
        if (row_nbr < rows_.size())
            return const_cast<ref_dataset::record*> (rows_[row_nbr]);
        else
            return NULL;
    }

    bool ref_dataset::recordset::exists(const ref_dataset::record& row) const {
        for (int i = rows_.size() - 1; i >= 0; --i)
            if (row == *rows_[i]) return true;
        return false;
    }

    bool ref_dataset::recordset::insert(ref_dataset::record* row) {
        if (row && row->get_set().metadata_ == metadata_) {
            rows_.push_back(row);
            return true;
        } else
            return false;
    }

    void ref_dataset::recordset::clear() {
        for (int i = rows_.size() - 1; i >= 0; --i) delete rows_[i];
        rows_.clear();
    }

    std::ostream& operator<<(std::ostream& s, const ref_dataset::recordset& r) {
        ref_dataset::recordset::rows_t::size_type size = r.rows_.size();
        s << &r << "->" << r.id_ << '[' << r.idx_ << "]::size=" << size << ", metadata={";
        /*ref_dataset::recordset::metadata_t::const_iterator itr = r.metadata_->begin();
        ref_dataset::recordset::metadata_t::const_iterator end = r.metadata_->end();
        if (itr != end) {
            s << itr->first << '=' << itr->second;
            ++itr;
        }
        for (; itr != end; ++itr) {
            s << ',' << itr->first << '=' << itr->second;
        }*/
        s << *(r.metadata_);
        s << "}\n record=";
        for (int i = 0; i < size; ++i) {
            s << '\n' << i << '>' << *(r.rows_[i]);
        }
        return s; // << '\n';
    }
    ////////////////////////////////////////////////////////////////////////////

    ref_dataset::ref_dataset(const ref_dataset::metadata& mdata)
    : metadata_(&mdata), recordsets_(mdata.size()), id_map_idx_() {
        // vector<string> ids;
        // metadata_->get_ids(ids);
        metadata::idx_map_t::const_iterator itr = metadata_->begin_id();
        metadata::idx_map_t::const_iterator end = metadata_->end_id();
        int idx;
        metadata::impl_t* md;
        // for (int i = ids.size() - 1; i >= 0; --i) {
        for (/*int i = 0*/; itr != end; ++itr/*, ++i*/) {
            // idx = metadata_->get(ids[i]);
            idx = metadata_->get(itr->first);
            if (idx < 0) {
                MLOG_ERROR << this << "->ref_dataset cant get metadata by id:"
                        << itr->first << endl;
                continue;
            }
            md = metadata_->get(idx);
            if (md == NULL) {
                MLOG_ERROR << this << "->ref_dataset cant get metadata by index["
                        << itr->first << ']' << idx << endl;
                continue;
            }
            // recordsets_[i] = new recordset(itr->first, *md);
            // recordsets_[i]->set_idx(i);
            // id_map_idx_.insert(idx_map_t::value_type(itr->first, i));
            recordsets_[idx] = new recordset(itr->first, *md);
            recordsets_[idx]->set_idx(idx);
            id_map_idx_.insert(idx_map_t::value_type(itr->first, idx));
        }
    }

    ref_dataset::~ref_dataset() {
        clear();
    }

    ref_dataset::ref_dataset(const ref_dataset& dataset)
    : metadata_(dataset.metadata_), id_map_idx_(dataset.id_map_idx_) {
        // recordsets_ = dataset.recordsets_;
        const size_t size = dataset.recordsets_.size();
        recordsets_.resize(size);
        for (int i = 0; i < size; ++i) {
            // delete recordsets_[i];
            recordsets_[i] = new recordset(*(dataset.recordsets_[i]));
        }
    }

    ref_dataset& ref_dataset::operator=(const ref_dataset& dataset) {
        if (this == &dataset) return *this;
        clear();
        metadata_ = dataset.metadata_;
        id_map_idx_ = dataset.id_map_idx_;
        // recordsets_ = dataset.recordsets_;
        const size_t size = dataset.recordsets_.size();
        recordsets_.resize(size);
        for (int i = 0; i < size; ++i) {
            // delete recordsets_[i];
            recordsets_[i] = new recordset(*(dataset.recordsets_[i]));
        }
        return *this;
    }

    ref_dataset::recordset* ref_dataset::get(const string& id) {
        idx_map_t::const_iterator found = id_map_idx_.find(id);
        if (found != id_map_idx_.end())
            return get(found->second);
        else
            return NULL;
    }

    const ref_dataset::recordset* ref_dataset::get(const string& id) const {
        // return get(id);
        return const_cast<ref_dataset*> (this)->get(id);
    }

    ref_dataset::recordset* ref_dataset::get(const ref_dataset::recordsets_t::size_type idx) {
        if (idx < recordsets_.size()) return recordsets_[idx];
        else return NULL;
    }

    const ref_dataset::recordset* ref_dataset::get(const ref_dataset::recordsets_t::size_type idx) const {
        // return get(idx);
        return const_cast<ref_dataset*> (this)->get(idx);
    }

    void ref_dataset::clear() {
        for (int i = recordsets_.size() - 1; i >= 0; --i) delete recordsets_[i];
        recordsets_.clear();
        id_map_idx_.clear();
    }

    ostream& operator<<(ostream& s, const ref_dataset& r) {
        ref_dataset::recordsets_t::size_type size = r.recordsets_.size();
        s << &r << "::size=" << size << "::metadata=" << *(r.metadata_) << "\n recordset=";
        for (int i = 0; i < size; ++i) {
            s << '\n' << i << ">>" << *(r.recordsets_[i]);
        }
        return s; // << '\n';
    }
}
