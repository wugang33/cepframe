/* 
 * File:   plugin_function.hpp
 * Author: WangquN
 *
 * Created on 2012-02-26 AM 10:48
 */

#ifndef PLUGIN_FUNCTION_HPP
#define	PLUGIN_FUNCTION_HPP

#include <cassert>
#include <map>
#include <vector>
#include <ostream>

#include "processing/plugin_symbol.hpp"

using std::string;
using std::map;
using std::vector;

namespace cep {
    class type_slice;
    class event;
    // class plugin_func_ptr_t;

    class plugin_function {
    public:
        plugin_function();
        ~plugin_function();
    private:

        plugin_function(plugin_function& arg) { // Forbidden copy behave
            // MLOG_INFO << this << "->plugin_function copy from[" << &arg << "] ctor execute!" << endl;
        }

        plugin_function& operator=(const plugin_function& arg) { // Forbidden copy behave
            if (this == &arg) // effective c++ 16
                return *this;
            // MLOG_INFO << this << "->plugin_function assignment operator(" << &arg << ") execute!" << endl;
            return *this;
        }
    public:
        int add_plugin_func(const string& id, plugin_func_ptr_t plugin_func_ptr);
        int plugin_func_idx(const string& id) const;

        void execute(const size_t idx, const type_slice source[], const size_t source_count,
                event& destination, const size_t fields_idx[], const size_t fields_count) const {
            assert(idx < plugin_funcs_.size());
            plugin_funcs_[idx](source, source_count, destination, fields_idx, fields_count);
        }

        void clear();
    private:
        friend std::ostream& operator<<(std::ostream&, const plugin_function&);

        vector<plugin_func_ptr_t> plugin_funcs_;
        map<string, size_t> id_map_idx_;
    };
}

#endif	/* PLUGIN_FUNCTION_HPP */
