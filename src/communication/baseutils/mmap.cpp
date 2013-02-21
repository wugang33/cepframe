#include "communication/baseutils/mmap.h"
#include  <sys/mman.h>
UINT MMapFile::m_iAllocationGranularity = 0;

MMapFile::MMapFile() : \
m_iUsed(0), m_hFile(0), m_hFileDesc(-1), m_pView(0), \
m_iSize(0), m_bReadOnly(0), m_bTempHandle(0), m_bMustClose(0) {
    if (!MMapFile::m_iAllocationGranularity) {
        MMapFile::m_iAllocationGranularity = getpagesize();
    }

}

MMapFile::~MMapFile() {// 
    this->clear();
}

void MMapFile::SetLen(UINT iLen) {
    m_iUsed = iLen;
}

UINT MMapFile::GetLen() {
    return m_iUsed;
}

void MMapFile::clear() {
    this->release();
    if (!m_bMustClose) {
        m_hFile = 0;
        m_hFileDesc = -1;
        return;
    }
    if (m_hFile) {
        fclose(m_hFile);
        m_hFile = 0;
        m_hFileDesc = -1;
        return;
    }
    close(m_hFileDesc);
    m_hFile = 0;
    m_hFileDesc = -1;
    return;
}

void MMapFile::SetRO(BOOL bRO) {
    m_bReadOnly = bRO;
}

INT MMapFile::setfile(INT hFile, DWORD dwSize) {
    this->release();
    this->m_hFileDesc = hFile;
    m_bTempHandle = 0;
    if (hFile != -1) {
        m_iSize = dwSize;
        if (dwSize <= 0) {
            m_bReadOnly = 0;
            return 0;
        }
        return 1;
    }
    return hFile;
}

void MMapFile::SetFile(const char * pFileName) {
    m_filename.assign(pFileName);
}

bool MMapFile::ReSize(UINT newsize) {
    this->release();
    if (m_iSize >= newsize) {
        return true;
    }
    this->m_iSize = newsize;

    char tempfile[128] = "filecacheXXXXXX";
    if (!m_hFile) {
        if (m_filename.compare("") == 0) {
            m_hFileDesc = mkstemp(tempfile);
            int ulink_ret = unlink(tempfile);
            if (ulink_ret != 0) {
                printf("unlink error ret[%d]\n", ulink_ret);
            }
            if (m_hFileDesc != -1) {
                this->m_filename.assign(tempfile);
                this->m_hFile = fdopen(m_hFileDesc, "rw");
            } else {
                this->m_hFile = tmpfile();
                if (m_hFile) {
                    m_hFileDesc = fileno(m_hFile);
                    m_bTempHandle = true;
                }
            }
        } else {
            //      O_RDONLY S_IRUSR O_NOFOLLOW
            m_hFileDesc = open(m_filename.c_str(), O_CREAT | O_RDWR, 0x1ed);
            this->m_hFile = fdopen(m_hFileDesc, "rw");
        }
        m_bMustClose = true;
    }
    if (m_hFileDesc != -1) {
        int ret = lseek(m_hFileDesc, m_iSize - 1, SEEK_SET);
        char buffer[1] = {0};
        if (ret != -1) {
            ret = read(m_hFileDesc, buffer, sizeof (buffer));
            if (ret != -1) {
                ret = lseek(m_hFileDesc, m_iSize - 1, SEEK_SET);
                if (ret != -1) {
                    ret = write(m_hFileDesc, buffer, sizeof (buffer));
                    if (ret != -1) {
                        return true;
                    }
                }
            }
        }
    }
    perror("file operator failed:\n");
    fprintf(stderr, "\n error creating mmap the size of %d.\n", this->m_iSize);
    return false;
}

UINT MMapFile::GetSize() const {
    return m_iSize;
}

void *MMapFile::Get(UINT offset, UINT size) const {
    return GetAligned(offset, &size);
}

void *MMapFile::GetMore(UINT offset, UINT size) const {
    return this->Get(offset, size);
}

/*                              rsi             rdx            */
void *MMapFile::GetAligned(UINT offset, UINT *size) const {
    if (!size) {
        return 0;
    }
    if (!m_iSize || m_iSize < (offset + *size)) {
        fprintf(stderr, "\n error mapping file(%d,%d) is out of range.\n", offset, *size);
        return 0;
    }
    /*
     * edx  =offset
     * eax = offset
     * dex>>31位
     * edx = offset%MMapFile24m_iAllocationGranularity
     * eax = offset- offset%MMapFile24m_iAllocationGranularity
     * ecx = size
     * r8d = edx
     * r13d = r8+rcx = offset%MMapFile24m_iAllocationGranularity+size
     * esi = m_iSize
     */
    int eax = offset - offset % MMapFile::m_iAllocationGranularity;
    int edx = offset % MMapFile::m_iAllocationGranularity;

    if (m_pView && (m_iSize > (edx + *size)) || !m_pView) {
        /*
         * if m_bReadOnly<1    edx = (-1&2)+1
         * PROT_WRITE
         */
        m_pView = mmap(0, edx + *size, m_bReadOnly ? PROT_READ : PROT_WRITE, 1, m_hFileDesc, eax);
        m_iMappedSize = edx + *size;
    }
    if (MAP_FAILED == m_pView) {
        fprintf(stderr, "\n error mapping datablock to %d .\n", (edx + size));
    }
    return (void*) ((char*) m_pView + edx); //(void *) (offset - (char*) m_pView - eax);
}

void MMapFile::release() {
    if (m_pView) {
        munmap(m_pView, m_iMappedSize);
        this->m_pView = 0;
    }
}

void MMapFile::release(void *pView, UINT size) {
    if (pView) {
        munmap((void *) ((long) pView - ((long) pView % MMapFile::m_iAllocationGranularity)), size + ((long) pView % MMapFile::m_iAllocationGranularity));
    }
}

void MMapFile::flush(UINT num) {
    if (m_pView) {
        msync(m_pView, num, MS_SYNC);
    }
}

MMapBuf::MMapBuf(INT Type, UINT trunk_size) : m_trunk_size(trunk_size), m_gb_u(Type),
m_alloc(0), //0x78  
m_used(0), //0x7c
m_bInit(0) //0x80 
{//i add it

}

MMapBuf::~MMapBuf() {//iadd it
}

INT MMapBuf::Init(const char *file, UINT size, UINT FileFlags) {//iadd it
    this->m_fm.SetFile(file);
    int fd = open(file, 0x42, 0x1ed);
    char buffer[1] = {0};
    if (fd == -1) {
        return -1;
    }
    int ret = lseek(fd, size - 1, SEEK_SET);
    if (ret != -1) {
        ret = read(fd, buffer, 1);
        if (ret != -1) {
            ret = lseek(fd, size - 1, SEEK_SET);
            if (ret != -1) {
                write(fd, buffer, 1);
            }
        }

    }
    m_alloc = size;
    ret = this->m_fm.setfile(fd, size);
    if (ret == 0) {
        m_bInit = 1;
    }
    return ret;

}

INT MMapBuf::add(const void *data, UINT len) {//iadd it
    if (len <= 0) {
        return 0;
    }
    int len_t = this->GetLen();
    int esi = len + len_t;
    this->ReSize(esi);
    len_t = this->GetLen();
    void *ptr = this->Get(len_t - len, len);
    memcpy(ptr, data, len);
    this->release();
    return this->GetLen() - len;
}

void MMapBuf::SetRO(BOOL bRO) {//iadd it
    this->m_fm.SetRO(bRO);
}

/*
  GrowBuf m_gb; //  0x10        sizeof(GrowBuf)=0x20h
    MMapFile m_fm; //  0x30        sizeof(MMapFile) = 0x40h
    INT m_trunk_size; //0x70
    INT m_gb_u; //0x74
    INT m_alloc; //0x78  
    INT m_used; //0x7c
    bool m_bInit; //0x80
 */
//bool MMapBuf::ReSize(UINT newlen) {//iadd it
//    if (GROWBUF == m_gb_u) {
//        if (newlen <= m_trunk_size) {
//            return this->m_gb.ReSize(newlen);
//        } else {
//            return false;
//        }
//    }
//    m_gb_u = MMAPFILE;
//
//    if (m_alloc > newlen) {
//        return true;
//    }
//    this->m_alloc = newlen + m_trunk_size;
//    bool ret = this->m_fm.ReSize(m_alloc);
//    if (!ret) {
//        return false;
//    }
//    int len_t = this->m_gb.GetLen();
//    if (!len_t) {
//        return true;
//    }
//    void *gb = this->m_gb.Get();
//    void *fm = this->m_fm.Get(0, len_t);
//    memcpy(fm, gb, len_t);
//    this->m_fm.flush(len_t);
//    this->m_fm.release();
//    this->m_gb.ReSize(0);
//    return true;
//
//}

bool MMapBuf::ReSize(UINT newlen) {//iadd it
    if (GROWBUF == m_gb_u) {
        return this->m_gb.ReSize(newlen);
    }
    if (m_alloc > newlen) {
        return true;
    }
    this->m_alloc = newlen + m_trunk_size;
    bool ret = this->m_fm.ReSize(m_alloc);
    if (ret) {
        return true;
    }
    return false;
}

UINT MMapBuf::GetSize() const {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.GetSize();
    } else {
        return this->m_gb.GetSize();
    }
}

UINT MMapBuf::GetLen() const {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_used;
    } else {
        return this->m_gb.GetLen();
    }
}

UINT MMapBuf::GetSpared() const {//iadd it
    return this->GetSize() - this->GetLen();
}

void * MMapBuf::Get() const {//iadd it
    return this->Get(0, this->m_alloc);
}

void * MMapBuf::Get(UINT offset, UINT * sizep) const {//iadd it
    if (sizep) {
        return this->Get(offset, *sizep);
    }
    return 0;
}

void * MMapBuf::Get(UINT offset, UINT size) const {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.Get(offset, size);
    } else {
        return this->m_gb.Get();
    }
}

void * MMapBuf::GetMore(UINT offset, UINT size) const {//iadd it
    if (GROWBUF == m_gb_u) {
        return this->m_gb.Get();
    }
    return this->m_fm.GetMore(offset, size);
}

void MMapBuf::release() {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.release();
    }
}

void MMapBuf::release(void *pView, UINT size) {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.release(pView, size);
    }
}

void MMapBuf::clear() {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.clear();
    }
}

void MMapBuf::flush(UINT num) {//iadd it
    if (MMAPFILE == m_gb_u) {
        return this->m_fm.flush(num);
    }
}
