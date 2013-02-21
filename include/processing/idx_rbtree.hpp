/* 
 * File:   idx_rbtree.hpp
 * Author: WangquN
 *
 * Created on 2012-02-14 AM 10:51
 */

#ifndef IDX_RBTREE_HPP
#define	IDX_RBTREE_HPP

#include <map>
#include <ostream>

#include "const_dataset.hpp"

#include "buffer_function.hpp"

using std::map;
using std::ostream;

namespace cep {
    class dynamic_struct;

    class idx_rbtree : public const_dataset::index {
    public:
        typedef multimap<type_slice, const_dataset::table::rows_t::size_type, less> slice_map_t;
        idx_rbtree(const string& id, const vector<string>& keys, const const_dataset::table&);
        virtual ~idx_rbtree();

        virtual bool rebuild();
        // template<typename function_t> //  <stl_algo.h> std::for_each
        // error: templates may not be ‘virtual’
        virtual void select(const type_slice where[], const_dataset::index::iterator&) const;
    protected:
        void free();
    private:
        friend ostream& operator<<(ostream&, const idx_rbtree&);

        slice_map_t map_;
    };
}

#endif	/* IDX_RBTREE_HPP */

