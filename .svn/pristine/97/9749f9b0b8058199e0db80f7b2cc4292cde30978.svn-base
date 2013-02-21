/* 
 * File:   growbuf.cpp
 * Author: WG
 *
 * Created on 2012年2月25日, 下午2:39
 */
#include "communication/baseutils/growbuf.h"

GrowBuf::GrowBuf() : \
m_s(0), m_alloc(0), m_used(0), m_zero(0), m_bs(0x10000) {

}

GrowBuf::~GrowBuf() {
    if (m_s) {
        free(m_s);
    }

}

void GrowBuf::set_zeroing(INT zero) {
    m_zero = zero;
}

INT GrowBuf::add(const void *data, UINT len) {
    if (len) {
        this->ReSize(m_used + len);
        memcpy(m_used + (char*) m_s - len, (char*) const_cast<void *> (data), len);
        return m_used;
    }
    return 0;
}

bool GrowBuf::ReSize(UINT newlen) {
    int used_r12d = this->m_used;
    this->m_used = newlen;
    if (m_alloc < newlen) {
        this->m_alloc = 2 * newlen + m_bs;
        char * realloced = (char*) realloc(m_s, this->m_alloc);
        if (!realloced) {
            fprintf(stderr, "ack! realloc(%d)failed, trying malloc(%d)!\n", m_alloc, newlen);
            realloced = (char*) malloc(newlen);
            if (!realloced) {
                fprintf(stderr, "ack! realloc/malloc failed!\n");
                return false;
            }
            memcpy(realloced, this->m_s, used_r12d);
            free(this->m_s);
            this->m_s = 0;
            /*
             * rdx =m_alloc_var_24
             * rcx = newlen
             * 
             */
        }
        this->m_s = realloced;
    }
    if (m_zero) {
        if (used_r12d < m_used) {
            memset((char *) m_s + used_r12d, 0, m_used - used_r12d);
        }
    }
    if (!m_used) {
        if (m_alloc > m_bs) {
            free(this->m_s);
            this->m_s = 0;
            this->m_alloc = 0;
        }
    }
    return true;
}

void GrowBuf::SetLen(UINT newlen) {
    this->m_used = newlen;
}

UINT GrowBuf::GetSize() const {
    return this->m_alloc;
}

void GrowBuf::ReSet() {
    this->m_used = 0;
}

UINT GrowBuf::GetLen() const {
    return this->m_used;
    ;
}

void *GrowBuf::Get() const {
    return this->m_s;
}
