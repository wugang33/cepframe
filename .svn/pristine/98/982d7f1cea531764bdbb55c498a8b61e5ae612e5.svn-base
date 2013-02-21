/* 
 * File:   dynamic_struct.hpp
 * Author: WG
 *
 * Created on 2011-12-08 AM 10:22
 */

#include "dynamic_struct.hpp"

namespace cep {

    ostream& operator<<(ostream& s, const dynamic_struct& r) {
        s << &r << "::length=" << r.length_ << ", type=" << r.type_
                << ", metadata=\n";
        if (r.metadata_) s << *(r.metadata_);
        else s << "NULL";
        s << "\ndata=";
        if (r.data_) buff_util::to_hex_string(r.data_, r.length_, s);
        else s << "NULL";
    }
}
