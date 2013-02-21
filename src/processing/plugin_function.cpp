/* 
 * File:   plugin_function.hpp
 * Author: WangquN
 *
 * Created on 2012-02-26 AM 10:48
 */

#include "processing/plugin_function.hpp"

#include "processing/plugin_symbol.hpp"

using namespace std;

namespace cep {

    plugin_function::plugin_function() : plugin_funcs_(), id_map_idx_() {
    }

    plugin_function::~plugin_function() {
        clear();
    }

    int plugin_function::add_plugin_func(const string& id, plugin_func_ptr_t plugin_func_ptr) {
        if (plugin_func_ptr == NULL) return -1;

        map<string, size_t>::const_iterator found = id_map_idx_.find(id);
        if (found == id_map_idx_.end()) {
            size_t idx = plugin_funcs_.size();
            id_map_idx_.insert(map<string, size_t>::value_type(id, idx));
            plugin_funcs_.push_back(plugin_func_ptr);
            return idx;
        } else
            return found->second;
    }

    int plugin_function::plugin_func_idx(const string& id) const {
        map<string, size_t>::const_iterator found = id_map_idx_.find(id);
        if (found == id_map_idx_.end()) return -1;
        else return found->second;
    }

    void plugin_function::clear() {
        plugin_funcs_.clear();
        id_map_idx_.clear();
    }

    ostream& operator<<(ostream& s, const plugin_function& r) {
        s << &r << "->size=" << r.id_map_idx_.size();
        map<string, size_t>::const_iterator itr = r.id_map_idx_.begin();
        map<string, size_t>::const_iterator end = r.id_map_idx_.end();
        for (; itr != end; ++itr)
            s << "\n >" << itr->first << '=' << r.plugin_funcs_[itr->second];
        return s;
    }
}
