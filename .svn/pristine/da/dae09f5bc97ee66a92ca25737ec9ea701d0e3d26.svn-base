#ifndef __GROWBUF_H_
#define __GROWBUF_H_

#include "communication/baseutils/FSPublic.h"

class IGrowBuf {
public:

    virtual ~IGrowBuf() {
    }
    virtual INT add(const void *data, UINT len) = 0;
    virtual bool ReSize(UINT newlen) = 0;
    virtual UINT GetLen() const = 0;
    virtual void *Get() const = 0; //0x28
};

class GrowBuf : public IGrowBuf {
public:

    GrowBuf();

    virtual ~GrowBuf();

    void set_zeroing(INT zero);

    INT add(const void *data, UINT len);

    bool ReSize(UINT newlen);

    void SetLen(UINT newlen);

    UINT GetSize() const;

    void ReSet();

    UINT GetLen() const;

    void *Get() const;

private: // don't copy instances
    GrowBuf(const GrowBuf&);
    void operator=(const GrowBuf&);

private:
    void *m_s; //0x08
    UINT m_alloc; //0x10
    UINT m_used; //0x14
    UINT m_zero; //0x18

protected:
    UINT m_bs; //0x1c
};

class TinyGrowBuf : public GrowBuf {
public:

    TinyGrowBuf() : GrowBuf() {
        m_bs = 1024;
    }
};

#endif

