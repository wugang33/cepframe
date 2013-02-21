/* 
 * File:   config_metadata.hpp
 * Author: WangquN
 *
 * Created on 2012-01-31 PM 01:59
 */

#ifndef CONFIG_METADATA_HPP
#define	CONFIG_METADATA_HPP

#include <map>
#include <vector>
#include <ostream>

#include "singleton.hpp"
#include "utils_xml.hpp"

namespace cep {
    // class singleton;
    class buffer_metadata;

    // class xml_document;
    // class xml_element; // error: using typedef-name ‘cep::xml_element’ after ‘class’

    class config_metadata : public singleton<config_metadata> {
        friend class singleton<config_metadata>;
    public:
        typedef unsigned int type_t;
        typedef std::map<type_t, buffer_metadata*> map_t;
        typedef std::map<std::string, buffer_metadata*> code_map_t;
        typedef std::vector<std::pair<type_t, std::string> > arr_t;
#ifdef __FLAG4INHERITABLE__
    protected:
#else
    private: // singleton pattern
#endif

        config_metadata() : m_(), code_map_(), info_() { // must have a default constructor
            // cout << this << "->config_metadata default ctor execute!" << endl;
        }

        config_metadata(config_metadata& cm) { // Forbidden copy behave
            // cout << this << "->config_metadata copy from[" << &config_metadata << "] ctor execute!" << endl;
        }

        config_metadata& operator=(const config_metadata& cm) { // Forbidden copy behave
            if (this == &cm) // effective c++ 16
                return *this;
            // cout << this << "->config_metadata assignment operator(" << &cm << ") execute!" << endl;
            return *this;
        }
    public:

#ifdef __FLAG4INHERITABLE__

        virtual
#endif
        ~config_metadata();
    public:
        bool reload_metadata(const xml_element& xml);
        buffer_metadata* metadata(const type_t type) const;
        buffer_metadata* metadata(const std::string& code) const;
        void free();
        /**
         * config_metadata will hold this memory for buffer_metadata*
         * so do not delete buffer_metadata by call this method code
         * 
         * @param type
         * @param metadata
         * @return false need to manage argument's memory
         */
        bool insert_metadata(const type_t type, const std::string& code, buffer_metadata*);
    private:
        friend std::ostream& operator<<(std::ostream&, const config_metadata&);
    private:
        map_t m_;
        code_map_t code_map_;
        arr_t info_;
    };
}

#endif	/* CONFIG_METADATA_HPP */
