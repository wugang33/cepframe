/* 
 * File:   cep_base.hpp
 * Author: WangquN
 *
 * Created on 2012-01-07 PM 03:10
 */

// Please see include\comm\FSPublic.h
/*
#ifdef localtime
#undef localtime
#endif

#ifdef yield
#undef yield
#endif
 */

#ifndef CEP_BASE_HPP
#define	CEP_BASE_HPP

namespace cep {

    struct true_tag {
    }; // TRUE; // : multiple definition of `cep::TRUE'

    struct false_tag {
    }; // FALSE; // : multiple definition of `cep::FALSE'

    typedef int event_type_t;
}

#endif	/* CEP_BASE_HPP */
