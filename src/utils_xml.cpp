/* 
 * File:   utils_xml.hpp
 * Author: liukai
 *
 * Created on 2012-01-29 PM 03:48
 */

#include <string.h>

#include "tinyxml.h"
#include "slice.hpp"

#include <stack>

#include "utils_xml.hpp"

// #define __FLAG4SHOW_DETAIL_LOG__
// #undef __FLAG4SHOW_DETAIL_LOG__
#ifdef __FLAG4SHOW_DETAIL_LOG__
#include <iostream>
#endif

namespace cep {

    xml_document* load_xmlfile(const char* path) {
        xml_document *doc = new xml_document(path);
        doc->SetCondenseWhiteSpace(false); // do not convert " a " to "a"
        if (doc->LoadFile())
            return doc;
        else {
            delete doc;
            return NULL;
        }
    }

    xml_document* parse_xmlstr(const char* xmlstr) {
        xml_document *doc = new xml_document();
        doc->SetCondenseWhiteSpace(false); // do not convert " a " to "a"
        /*if (doc->Parse(xmlstr)) // it cant check the result
            return doc;
        else {
            delete doc;
            return NULL;
        }*/
        doc->Parse(xmlstr);
        return doc;
    }

    const xml_element* root_element(const xml_document& xml_doc) {
        return xml_doc.RootElement();
    }

    const xml_element* first_child(const xml_element& xml_ele) {
        return xml_ele.FirstChildElement();
    }

    const xml_element* next_sibling(const xml_element& xml_ele) {
        return xml_ele.NextSiblingElement();
    }

    slice name(const xml_element& xml_ele) {
        return slice(xml_ele.Value());
    }

    slice value(const xml_element& xml_ele, const slice& default_value) {
        const char *val = xml_ele.GetText();
        if (val) return slice(val);
        else return default_value;
    }

    slice attribute(const xml_element& xml_ele, const char* name, const slice& default_value) {
        if (name == NULL) return default_value;
        const char *val = xml_ele.Attribute(name);
        if (val) return slice(val);
        else return default_value;
    }

    const xml_element* find_element(const xml_element& xml_ele, const char* name) {
        /*if (name == NULL) return NULL;

        if (strcmp(name, xml_ele.Value()) == 0)
            return &xml_ele;

        typedef std::stack<const xml_element *> stack_;
        stack_ stk;
        // stk.push(&xml_ele);
        const xml_element *ele = &xml_ele, *ele2;
        ele2 = ele->FirstChildElement();
        if (ele2) stk.push(ele2); // just find tree that root is ele
        else return NULL; // if ele no child then find failure!
        do {
            ele = stk.top();
            stk.pop();
#ifdef __FLAG4SHOW_DETAIL_LOG__ // pop
            std::cout << "pop:" << ele->Value() << std::endl;
#endif
            if (strcmp(name, ele->Value()) == 0) return ele;
            ele2 = ele->FirstChildElement();
            if (ele2 == NULL) // if ele no child then need find it next sibling
                ele = ele->NextSiblingElement();
            else {
                // if find ele's child then need push ele's next sibling
                ele = ele->NextSiblingElement();
                if (ele) { // if has next sibling
                    stk.push(ele);
#ifdef __FLAG4SHOW_DETAIL_LOG__ // push pop next
                    std::cout << "push pop next:" << ele->Value() << std::endl;
#endif
                }
                ele = ele2;
            }
            while (ele != NULL) { // loop all ele's next sibling
#ifdef __FLAG4SHOW_DETAIL_LOG__ // iterate
                std::cout << "find:" << ele->Value() << std::endl;
#endif
                if (strcmp(name, ele->Value()) == 0) return ele;
                ele2 = ele->FirstChildElement();
                if (ele2) {
#ifdef __FLAG4SHOW_DETAIL_LOG__ // first child
                    std::cout << ele->Value() << ":first child is:" << ele2->Value() << std::endl;
#endif
                    ele = ele->NextSiblingElement();
                    if (ele) { // if has next sibling
                        stk.push(ele);
#ifdef __FLAG4SHOW_DETAIL_LOG__ // push
                        std::cout << "push:" << ele->Value() << std::endl;
#endif
                    }
                    ele = ele2;
                } else
                    ele = ele->NextSiblingElement();
            }
        } while (!stk.empty());

        return NULL;*/
        return found_element(&xml_ele, name);
    }
    ////////////////////////////////////////////////////////////////////////////

    const xml_element* try2get_root_element(const xml_document* xml_doc) {
        if (xml_doc != NULL) return xml_doc->RootElement();
        else return NULL;
    }

    const xml_element* try2get_first_child(const xml_element* xml_ele) {
        if (xml_ele != NULL) return xml_ele->FirstChildElement();
        else return NULL;
    }

    const xml_element* try2get_next_sibling(const xml_element* xml_ele) {
        if (xml_ele != NULL) return xml_ele->NextSiblingElement();
        else return NULL;
    }

    bool try2get_name(const xml_element* xml_ele, slice& target_value) {
        if (xml_ele != NULL) {
            // target_value = slice(xml_ele->Value());
            // return true;
            const char *val = xml_ele->Value();
            if (val != NULL) {
                target_value = slice(val);
                return true;
            } else return false;
        } else return false;
    }

    bool try2get_value(const xml_element* xml_ele, slice& target_value) {
        if (xml_ele != NULL) {
            const char *val = xml_ele->GetText();
            if (val != NULL) {
                target_value = slice(val);
                return true;
            } else return false;
        } else return false;
    }

    bool try2get_attribute(const xml_element* xml_ele, const char* name, slice& target_value) {
        if (name == NULL) return false;
        else if (xml_ele != NULL) {
            const char *val = xml_ele->Attribute(name);
            if (val != NULL) {
                target_value = slice(val);
                return true;
            } else return false;
        } else return false;
    }

    const xml_element* found_element(const xml_element* xml_ele, const char* name) {
        if (xml_ele == NULL) return NULL;
        if (name == NULL) return NULL;

        if (xml_ele->Value() != NULL && strcmp(name, xml_ele->Value()) == 0)
            return xml_ele;

        typedef std::stack<const xml_element *> stack_;
        stack_ stk;
        // stk.push(&xml_ele);
        const xml_element *ele = xml_ele, *ele2;
        ele2 = ele->FirstChildElement();
        if (ele2) stk.push(ele2); // just find tree that root is ele
        else return NULL; // if ele no child then find failure!
        do {
            ele = stk.top();
            stk.pop();
#ifdef __FLAG4SHOW_DETAIL_LOG__ // pop
            std::cout << "pop:" << ele->Value() << std::endl;
#endif
            if (ele->Value() != NULL && strcmp(name, ele->Value()) == 0) return ele;
            ele2 = ele->FirstChildElement();
            if (ele2 == NULL) // if ele no child then need find it next sibling
                ele = ele->NextSiblingElement();
            else {
                // if find ele's child then need push ele's next sibling
                ele = ele->NextSiblingElement();
                if (ele) { // if has next sibling
                    stk.push(ele);
#ifdef __FLAG4SHOW_DETAIL_LOG__ // push pop next
                    std::cout << "push pop next:" << ele->Value() << std::endl;
#endif
                }
                ele = ele2;
            }
            while (ele != NULL) { // loop all ele's next sibling
#ifdef __FLAG4SHOW_DETAIL_LOG__ // iterate
                std::cout << "find:" << ele->Value() << std::endl;
#endif
                if (ele->Value() != NULL && strcmp(name, ele->Value()) == 0) return ele;
                ele2 = ele->FirstChildElement();
                if (ele2) {
#ifdef __FLAG4SHOW_DETAIL_LOG__ // first child
                    std::cout << ele->Value() << ":first child is:" << ele2->Value() << std::endl;
#endif
                    ele = ele->NextSiblingElement();
                    if (ele) { // if has next sibling
                        stk.push(ele);
#ifdef __FLAG4SHOW_DETAIL_LOG__ // push
                        std::cout << "push:" << ele->Value() << std::endl;
#endif
                    }
                    ele = ele2;
                } else
                    ele = ele->NextSiblingElement();
            }
        } while (!stk.empty());

        return NULL;
    }
}
