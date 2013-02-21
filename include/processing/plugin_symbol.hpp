/* 
 * File:   plugin_symbol.hpp
 * Author: WangquN
 *
 * Created on 2012-02-02 PM 01:57
 */

#ifndef PLUGIN_SYMBOL_HPP
#define	PLUGIN_SYMBOL_HPP

#define CEP_MACRO2CSTR(macro_name) __CEP_MACRO2CSTR(macro_name)
#define __CEP_MACRO2CSTR(str) #str

////////////////////////////////////////////////////////////////////////////////
// function pe component ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define CEP_LOAD_PE_FUNCTION __cep_load_pe
#define CEP_LOAD_PE(config, main_config) extern "C" cep::pe* CEP_LOAD_PE_FUNCTION\
(const cep::xml_element& config, const cep::xml_element& main_config)

#define CEP_UNLOAD_PE_FUNCTION __cep_unload_pe
#define CEP_UNLOAD_PE(ptr_pe) extern "C" void CEP_UNLOAD_PE_FUNCTION(cep::pe* ptr_pe)

////////////////////////////////////////////////////////////////////////////////
// plug-in function ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define CEP_PROCESSING_INIT_FUNCTION __cep_plugin_func4init
#define CEP_PLUGIN_FUNC4INIT(source, source_count, destination, fields_idx, fields_count)\
extern "C" void CEP_PROCESSING_INIT_FUNCTION\
(const cep::type_slice source[], const size_t source_count, cep::event& destination, const size_t fields_idx[], const size_t fields_count)

#define CEP_PROCESSING_INIT_EXEC_FUNCTION __cep_plugin_func4init_exec
#define CEP_PLUGIN_FUNC4INIT_EXEC(source, source_count, destination, fields_idx, fields_count)\
extern "C" void CEP_PROCESSING_INIT_EXEC_FUNCTION\
(const cep::type_slice source[], const size_t source_count, cep::event& destination, const size_t fields_idx[], const size_t fields_count)

#define CEP_PROCESSING_EXEC_FUNCTION __cep_plugin_func4exec
#define CEP_PLUGIN_FUNC4EXEC(source, source_count, destination, fields_idx, fields_count)\
extern "C" void CEP_PROCESSING_EXEC_FUNCTION\
(const cep::type_slice source[], const size_t source_count, cep::event& destination, const size_t fields_idx[], const size_t fields_count)

#include "utils_xml.hpp"

namespace cep {
    class pe;

    typedef pe* (*load_pe_func_ptr_t)(const cep::xml_element& config, const cep::xml_element& main_config);
    typedef void (*unload_pe_func_ptr_t)(pe*);

    class type_slice;
    class event;
    typedef void (*plugin_func_ptr_t)(const type_slice source[], const size_t source_count,
            event& destination, const size_t fields_idx[], const size_t fields_count);
}

#endif	/* PLUGIN_SYMBOL_HPP */
