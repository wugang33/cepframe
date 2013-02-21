/* 
 * File:   buffer_function.hpp
 * Author: WangquN
 *
 * Created on 2012-02-05 PM 01:01
 */

#include "buffer_function.hpp"

namespace cep {

    std::ostream& operator<<(std::ostream& s, const type_slice& r) {
        // hex=16 dec=10 oct=8 // no effective
        // return s << std::dec << r.type << ':' << buff_util::to_hex_string(r.data.data(), r.data.size());
        return s << /* &r << "->" << */ buffer_metadata::get_type_by_id(r.type) << ':'
                << buff_util::to_hex_string(r.data.data(), r.data.size());
    }
}
