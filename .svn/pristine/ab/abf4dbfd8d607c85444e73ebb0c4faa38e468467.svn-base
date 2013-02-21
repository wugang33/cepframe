/* 
 * The const dataset do not have any modify data function.
 * It can high speed to query data by many kinds of index!
 * 
 * File:   const_dataset.hpp
 * Author: WangquN
 *
 * Created on 2012-02-04 PM 04:39
 */

#ifndef CONST_DATASET_HPP
#define	CONST_DATASET_HPP

#include <vector>
#include <map>

#include <ostream>

#include "macro_log.hpp"
#include "slice.hpp"
#include "buffer_function.hpp"

// using std::vector;
// using std::map;
// using std::endl;
using namespace std;

namespace cep {
    class dynamic_struct;
    // class buffer_metadata;

    // class slice;
    // class type_slice;
    // class less;

    /** Base on dynamic_struct */
    class const_dataset {
    public:
        class table;

        class index {
        public:

            struct iterator
            : public std::binary_function<const dynamic_struct&, const size_t, void> {
                virtual bool operator()(const dynamic_struct&, const size_t&) = 0;
            };

            index(const string& id, const vector<string>& keys, const table&);
            virtual ~index(); // = 0;
        private: // Forbidden copy behave

            index(const index& idx) : size_(0) {
                MLOG_TRACE << this << "->index copy from[" << &idx << "] ctor execute!" << endl;
            }

            index& operator=(const index& idx) {
                if (this == &idx) // effective c++ 16
                    return *this;
                MLOG_TRACE << this << "->index assignment operator(" << &idx << ") execute!" << endl;
                return *this;
            }
        public:
            virtual bool rebuild() = 0;
            // template<typename function_t> //  <stl_algo.h> std::for_each
            // error: templates may not be ‘virtual’
            virtual void select(const type_slice where[], iterator&) const = 0;
        public:

            string id() const {
                return id_;
            }

            int idx() const {
                return idx_;
            }

            void set_idx(const int idx) {
                idx_ = idx;
            }

            /*size_t size() const {
                return table_->size();
            }*/

            const table& get_table() const {
                return *table_;
            }

            const size_t key_size() const {
                return size_;
            }

            const size_t* idx_keys() const {
                return idx_keys_;
            }
        protected:
            /** For get table from dataset directly! */
            int idx_;
            const size_t size_;
            /** index key's subscript for inner table */
            size_t* idx_keys_;
            const table* table_;
            string id_;
            /** query parameter field subscript */
            // vector<size_t> map_keys_; // process it in outer code
        private:
            friend ostream& operator<<(ostream&, const index&);
        };

        class table {
        public:
            typedef vector<dynamic_struct*> rows_t;
            // table() {}
            table(const string& id, const buffer_metadata& metadata);
            ~table();
        private: // Forbidden copy behave

            table(const table& tbl) {
                MLOG_TRACE << this << "->table copy from[" << &tbl << "] ctor execute!" << endl;
            }

            table& operator=(const table& tbl) {
                if (this == &tbl) // effective c++ 16
                    return *this;
                MLOG_TRACE << this << "->table assignment operator(" << &tbl << ") execute!" << endl;

                return *this;
            }
        public:

            string id() const {
                return id_;
            }

            const buffer_metadata& metadata() const {
                return *metadata_;
            }

            int idx() const {
                return idx_;
            }

            void set_idx(const int idx) {
                idx_ = idx;
            }

            rows_t::size_type size() const {
                return rows_.size();
            }

            // const rows_t& rows() const { return rows_; }

            void reserve(const rows_t::size_type size) {
                rows_.reserve(size);
            }
            const dynamic_struct* get(const rows_t::size_type row_nbr) const;
            bool exists(const dynamic_struct&) const;
            /** return false need to manage argument's memory */
            bool insert(dynamic_struct*);
            void clear();
        private:
            friend ostream& operator<<(ostream&, const table&);

            /** For get table from dataset directly! */
            int idx_;
            const buffer_metadata* metadata_;
            string id_;

            rows_t rows_;
        };
    public:
        typedef vector<table*> tables_t;
        typedef map<string, vector<table*>::size_type> id_map_table_t;
        typedef vector<index*> indexes_t;
        typedef map<string, vector<index*>::size_type> id_map_index_t;

        const_dataset();
        ~const_dataset();
    private: // Forbidden copy behave

        const_dataset(const const_dataset& dataset) {
            MLOG_TRACE << this << "->const_dataset copy from[" << &dataset << "] ctor execute!" << endl;
        }

        const_dataset& operator=(const const_dataset& dataset) {
            if (this == &dataset) // effective c++ 16
                return *this;
            MLOG_TRACE << this << "->const_dataset assignment operator(" << &dataset << ") execute!" << endl;

            return *this;
        }
    public:

        tables_t::size_type table_size() const {
            return tables_.size();
        }

        indexes_t::size_type index_size() const {
            return indexes_.size();
        }
    public:
        table* get_table(const string& table_id);
        const table* get_table(const string& table_id) const;
        table* get_table(const tables_t::size_type table_idx);
        const table* get_table(const tables_t::size_type table_idx) const;

        index* get_index(const string& index_id);
        const index* get_index(const string& index_id) const;
        index* get_index(const indexes_t::size_type index_idx);
        const index* get_index(const indexes_t::size_type index_idx) const;

        table* insert(const string& table_id, const buffer_metadata& metadata);
        /**
         * This API need to compatible with all type index
         * 
         * @param 
         * @return false need to manage argument's memory
         */
        bool insert(index*);
        void clear();
    private:
        friend ostream& operator<<(ostream&, const const_dataset&);

        tables_t tables_;
        id_map_table_t id_map_table_;

        indexes_t indexes_;
        id_map_index_t id_map_index_;
    };

    // const_dataset::index::~index() {} // pure virtual dtor must be implement

    /** Base on slice */
    class ref_dataset {
    public:

        class metadata {
        public:

            class impl_t {
            public:
                typedef buffer_metadata::enum_t field_type_t;
                // typedef unsigned char field_type_t;

                struct field_t {
                    field_type_t type;
                    size_t idx;
                    string code;
                };
                typedef std::vector<field_t*> entry_list_t;
                typedef std::map<std::string, field_t*> entry_map_t;

                impl_t() : entry_list_(), entry_map_() {
                }
#ifdef __FLAG4INHERITABLE__

                virtual
#endif
                ~impl_t();
            private: // Forbidden copy behave

                impl_t(const impl_t& t) {
                    // std::cout << this << "->metadata::impl_t copy from[" << &t << "] ctor execute!" << std::endl;
                }

                impl_t& operator=(const impl_t& metadata4buff) {
                    if (this == &metadata4buff) // effective c++ 16
                        return *this;
                    // std::cout << this << "->metadata::impl_t assignment operator(" << &t << ") execute!" << std::endl;
                    return *this;
                }
            public:
                const field_t* entry(const std::string&) const;
                const field_t* entry(const size_t) const;

                /** return false need to manage argument's memory */
                bool add_entry(field_t*);

                const entry_list_t& entry_list() const {
                    return this->entry_list_;
                }

                inline int size() const {
                    // return this->length;
                    return entry_list_.size();
                }
            private:
                entry_list_t entry_list_;
                entry_map_t entry_map_;

                friend std::ostream& operator<<(std::ostream&, const impl_t&);
            }; // typedef map<string, size_t> impl_t;

            typedef vector<impl_t*> metadatas_t;
            typedef map<string, metadatas_t::size_type> idx_map_t;

            metadata();
            ~metadata();
        private: // Forbidden copy behave

            metadata(const metadata& mdata) {
                MLOG_TRACE << this << "->metadata copy from[" << &mdata << "] ctor execute!" << endl;
            }

            metadata& operator=(const metadata& mdata) {
                if (this == &mdata) // effective c++ 16
                    return *this;
                MLOG_TRACE << this << "->metadata assignment operator(" << &mdata << ") execute!" << endl;

                return *this;
            }
        public:
            int get(const string& id) const;
            impl_t* get(const metadatas_t::size_type idx) const;

            /*metadatas_t::size_type size() const {
                return metadatas_.size();
            }*/
            idx_map_t::size_type size() const {
                // do not reuse metadatas_'s vector cell, so it equals metadatas_.size();
                return id_map_idx_.size();
            }
            // void get_ids(vector<string>& ids) const;

            idx_map_t::const_iterator begin_id() const {
                return id_map_idx_.begin();
            }

            idx_map_t::const_iterator end_id() const {
                return id_map_idx_.end();
            }

            void reserve(const metadatas_t::size_type size) {
                metadatas_.reserve(size);
            }
            /**
             * The different id can use the same meta data pointer!
             * 
             * @param id
             * @param metadata
             * @return false need to manage argument's memory
             */
            bool insert(const string& id, impl_t* metadata);
            void clear();
        private:
            friend ostream& operator<<(ostream&, const metadata&);

            metadatas_t metadatas_;
            idx_map_t id_map_idx_;
        };

        class recordset;

        class record {
        public:
            typedef vector<slice> cells_t;
            // record() {}
            record(const recordset&);
            // ~record();
            // private: // Forbidden copy behave
            // record(const record& rcd);
            /*{
                MLOG_TRACE << this << "->record copy from[" << &rcd << "] ctor execute!" << endl;
            }*/
            // record& operator=(const record& rcd);
            /*{
                if (this == &rcd) // effective c++ 16
                    return *this;
                MLOG_TRACE << this << "->record assignment operator(" << &rcd << ") execute!" << endl;

                return *this;
            }*/
        public:
            type_slice get(const string& col_name, const type_slice& default_value) const;
            type_slice get(const cells_t::size_type col_idx, const type_slice& default_value) const;
            bool set(const string& col_name, const type_slice& cell_value);
            bool set(const cells_t::size_type col_idx, const type_slice& cell_value);

            const recordset& get_set() const {
                return *recordset_;
            }
        private:
            friend class recordset;
            friend bool operator==(const record&, const record&);

            friend ostream& operator<<(ostream&, const record&);

            const recordset* recordset_;
            cells_t cells_;
        };

        class recordset {
        public:
            typedef vector<record*> rows_t;
            typedef metadata::impl_t metadata_t;
            // recordset() {}
            recordset(const string& id, const metadata_t& metadata);
            ~recordset();
            // private: // Forbidden copy behave
            recordset(const recordset& records);
            /*{
                MLOG_TRACE << this << "->recordset copy from[" << &records << "] ctor execute!" << endl;
            }*/
            recordset& operator=(const recordset& records);
            /*{
                if (this == &records) // effective c++ 16
                    return *this;
                MLOG_TRACE << this << "->recordset assignment operator(" << &records << ") execute!" << endl;

                return *this;
            }*/
        public:

            string id() const {
                return id_;
            }

            int idx() const {
                return idx_;
            }

            void set_idx(const int idx) {
                idx_ = idx;
            }

            record::cells_t::size_type col_size() const {
                return metadata_->size();
            }

            const metadata_t& get_metadata() const {
                return *metadata_;
            }

            rows_t::size_type size() const {
                return rows_.size();
            }

            void reserve(const rows_t::size_type size) {
                rows_.reserve(size);
            }
            const record* get(const rows_t::size_type row_nbr) const;
            bool exists(const record&) const;
            /** return false need to manage argument's memory */
            bool insert(record*);
            void clear();
        private:
            friend class record;

            friend ostream& operator<<(ostream&, const recordset&);

            /** For get table from dataset directly! */
            int idx_;
            const metadata_t* metadata_;
            string id_;

            rows_t rows_;
        };
    public:
        typedef vector<recordset*> recordsets_t;
        typedef map<string, recordsets_t::size_type> idx_map_t;

        ref_dataset(const metadata& mdata);
        ~ref_dataset();
        // private: // Forbidden copy behave
        ref_dataset(const ref_dataset& dataset);
        /*{
            MLOG_TRACE << this << "->ref_dataset copy from[" << &dataset << "] ctor execute!" << endl;
        }*/
        ref_dataset& operator=(const ref_dataset& dataset);
        /*{
            if (this == &dataset) // effective c++ 16
                return *this;
            MLOG_TRACE << this << "->ref_dataset assignment operator(" << &dataset << ") execute!" << endl;

            return *this;
        }*/
    public:

        const metadata& get_metadata() const {
            return *metadata_;
        }

        recordsets_t::size_type size() const {
            return recordsets_.size();
        }
    public:
        recordset* get(const string& id);
        const recordset* get(const string& id) const;
        recordset* get(const recordsets_t::size_type idx);
        const recordset* get(const recordsets_t::size_type idx) const;
        void clear();
    private:
        friend ostream& operator<<(ostream&, const ref_dataset&);

        const metadata* metadata_;
        recordsets_t recordsets_;
        idx_map_t id_map_idx_;
    };

    inline const ref_dataset::metadata::impl_t::field_t*
    ref_dataset::metadata::impl_t::entry(const std::string& code) const {
        entry_map_t::const_iterator iter = entry_map_.find(code);
        // if (likely(iter != this->entry_map_.end())) {
        if (iter != this->entry_map_.end()) {
            return iter->second;
        } else
            return NULL;
    }

    inline const ref_dataset::metadata::impl_t::field_t*
    ref_dataset::metadata::impl_t::entry(const size_t idx) const {
        // if (unlikely(idx >= entry_list.size())) return NULL;
        if (idx < entry_list_.size()) return entry_list_[idx];
        else return NULL;
    }

    inline bool operator==(const ref_dataset::record& x, const ref_dataset::record& y) {
        // return ((&(x.recordset_->get_metadata()) == &(y.recordset_->get_metadata())) &&
        return ((x.recordset_->col_size() == y.recordset_->col_size()) &&
                equal(x.cells_.begin(), x.cells_.end(), y.cells_.begin()));
    }

    inline bool operator!=(const ref_dataset::record& x, const ref_dataset::record& y) {
        return !(x == y);
    }
}

#endif	/* CONST_DATASET_HPP */
