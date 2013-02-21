/* 
 * File:   utils_xml.hpp
 * Author: liukai
 *
 * Created on 2012-01-29 PM 03:48
 */

#ifndef UTILS_XML_HPP
#define	UTILS_XML_HPP

#include "tinyxml.h"
#include "slice.hpp"

// class TiXmlDocument;
// class TiXmlElement;

namespace cep {
    // class slice;

    typedef TiXmlDocument xml_document;
    typedef TiXmlElement xml_element;

    extern xml_document* load_xmlfile(const char* path);
    extern xml_document* parse_xmlstr(const char* xmlstr);

    extern const xml_element* root_element(const xml_document& xml_doc);

    extern const xml_element* first_child(const xml_element& xml_ele);
    extern const xml_element* next_sibling(const xml_element& xml_ele);
    extern slice name(const xml_element& xml_ele);
    extern slice value(const xml_element& xml_ele, const slice& default_value);
    extern slice attribute(const xml_element& xml_ele, const char* name, const slice& default_value);

    /** Depth First Search Algorithm */
    extern const xml_element* find_element(const xml_element& xml_ele, const char* name);

    ////////////////////////////////////////////////////////////////////////////

    extern const xml_element* try2get_root_element(const xml_document* xml_doc);

    extern const xml_element* try2get_first_child(const xml_element* xml_ele);
    extern const xml_element* try2get_next_sibling(const xml_element* xml_ele);

    extern bool try2get_name(const xml_element* xml_ele, slice& target_value);
    extern bool try2get_value(const xml_element* xml_ele, slice& target_value);
    extern bool try2get_attribute(const xml_element* xml_ele, const char* name, slice& target_value);

    extern const xml_element* found_element(const xml_element* xml_ele, const char* name);
}

#endif	/* UTILS_XML_HPP */
