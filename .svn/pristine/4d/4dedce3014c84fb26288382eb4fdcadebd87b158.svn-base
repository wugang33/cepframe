#ifndef __MMAP_H_
#define __MMAP_H_

#include "communication/baseutils/FSPublic.h"
#include "communication/baseutils/growbuf.h"

class IMMap {
public:
    virtual bool ReSize(UINT newlen) = 0; //0x30
    virtual UINT GetSize() const = 0;
    virtual void *Get(UINT offset, UINT size) const = 0;
    virtual void *GetAligned(UINT offset, UINT *size) const = 0;
    virtual void *GetMore(UINT offset, UINT size) const = 0;
    virtual void release() = 0;
    virtual void release(void *view, UINT size) = 0;
    virtual void clear() = 0;
    virtual void SetRO(BOOL bRO) = 0;
    virtual void flush(UINT num) = 0;

    virtual ~IMMap() {
    }
};

class MMapFile : public IMMap {
public:

    MMapFile();

    virtual ~MMapFile();

    void SetLen(UINT iLen);

    UINT GetLen();

    void clear();

    void SetRO(BOOL bRO);
#ifdef _WIN32
    INT setfile(HANDLE hFile, DWORD dwSize);
#else

    INT setfile(INT hFile, DWORD dwSize);
#endif

    void SetFile(const char * pFileName);

    bool ReSize(UINT newsize);

    UINT GetSize() const;

    void *Get(UINT offset, UINT size) const;

    void *GetMore(UINT offset, UINT size) const;

    void *GetAligned(UINT offset, UINT *size) const;
    void release();

    void release(void *pView, UINT size);

    void flush(UINT num);

    UINT getGranularity() {
        return m_iAllocationGranularity;
    }

    const char * GetFile() {
        if (m_filename.size() == 0)
            return NULL;
        return m_filename.c_str();
    }

    INT GetFd() {
#if WIN32
        return -1;
#else
        return m_hFileDesc;
#endif
    }
private: // don't copy instances
    MMapFile(const MMapFile&);
    void operator=(const MMapFile&);

private:
#ifdef _WIN32
    HANDLE m_hFile, m_hFileMap;
#else
    FILE *m_hFile; //0x08
    INT m_hFileDesc; //0x10
    mutable INT m_iMappedSize; //0x14
#endif
    mutable void *m_pView; //0x18
    mutable INT m_iSize; //0x20
    BOOL m_bReadOnly; //0x24
    BOOL m_bTempHandle; //0x28
    UINT m_iUsed; //0x2c
    static UINT m_iAllocationGranularity; //0x61e664
    string m_filename; //0x30
    BOOL m_bMustClose; //0x38
};

typedef enum {
    GROWBUF = 0,
    MMAPFILE
} BUFFERTYPE;

class IGrowBuf;

class MMapBuf : public IGrowBuf, public IMMap {
public:

    MMapBuf(INT Type = GROWBUF, UINT trunk_size = 16 << 20);

    virtual ~MMapBuf();

    INT Init(const char *file, UINT size, UINT FileFlags = -1);

    INT add(const void *data, UINT len);

    void SetRO(BOOL bRO);

    bool ReSize(UINT newlen);

    UINT GetSize() const;

    UINT GetLen() const;

    UINT GetSpared() const;

    void *Get() const;

    void *Get(UINT offset, UINT *sizep) const;

    void *Get(UINT offset, UINT size) const;

    void *GetMore(UINT offset, UINT size) const;

    void *GetAligned(UINT offset, UINT *sizep) const {
        UINT size = *sizep;
        return Get(offset, size);
    }

    void release();

    void release(void *pView, UINT size);

    void clear();

    void flush(UINT num);

    void SetLen(UINT iLen) {
        if (m_gb_u == MMAPFILE) {
            m_fm.SetLen(iLen);
            m_used = iLen;
        } else
            m_gb.SetLen(iLen);
    }

    void SetFile(const char * pFileName) {
        m_fm.SetFile(pFileName);
    }

    const char * GetFile() {
        if (m_gb_u == MMAPFILE)
            return m_fm.GetFile();
        return NULL;
    }

    const int GetMode() {
        return this->m_gb_u;
    }

    INT GetFd() {
        if (m_gb_u == MMAPFILE)
            return m_fm.GetFd();
        return -1;
    }
private: // don't copy instances
    MMapBuf(const MMapBuf&);
    void operator=(const MMapBuf&);

private:
    GrowBuf m_gb; //  0x10        sizeof(GrowBuf)=0x20h
    MMapFile m_fm; //  0x30        sizeof(MMapFile) = 0x40h
    UINT m_trunk_size; //0x70
    UINT m_gb_u; //0x74
    UINT m_alloc; //0x78  
    UINT m_used; //0x7c
    bool m_bInit; //0x80
};

#endif//__MMAP_H_

