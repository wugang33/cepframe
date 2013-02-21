#include "communication/baseutils/netiobuf.h"
#include "boost/filesystem.hpp"
#include <string>
using namespace std;
ForceFileCache * ForceFileCache::s_ForceFileCache = NULL;
WithOutFileCache * WithOutFileCache::s_WithOutFileCache = NULL;

FileCacheManage::FileCacheManage(NetBufLstInfo * _pFileCacheInfo, DWORD MaxFileSize) : m_dwMaxFileSize(MaxFileSize == 0 ? 0x1f400000 : MaxFileSize)\
, m_iThrdID(getpid()), m_icount(0), m_pFileCacheInfo(_pFileCacheInfo) {
    char cache_filename[64] = {0};
    snprintf(cache_filename, 0x40, "%d", getpid());
    string curr_path = boost::filesystem::current_path().string();
    curr_path.append(1, '/');
    curr_path.append("proc");
    curr_path.append(1, '/');
    curr_path.append("cache");
    curr_path.append(1, '/');
    curr_path.append(cache_filename);
    curr_path.append(1, '/');
    this->m_strAbsPath.assign(curr_path);
    try {
        boost::filesystem::path p(curr_path);
        if (!boost::filesystem::exists(p)) {
            boost::filesystem::create_directories(p);
        }
    } catch (std::exception &e) {
        cout << "FileCacheManage::FileCacheManage create director fail[" << curr_path << "]" << e.what() << endl;
    }
}

FileCacheManage::~FileCacheManage() {
    deque<MMapBuf *>::iterator iter = this->m_dqeFileDescList.begin();
    while (iter != this->m_dqeFileDescList.end()) {
        delete *iter;
        iter++;
    }
}

bool FileCacheManage::SetFileIn(MMapBuf * pBuf) {
    this->m_dqeFileDescList.push_back(pBuf);
    pBuf->release();
    this->m_pFileCacheInfo->datalen += pBuf->GetLen();
    return true;
}

const char * FileCacheManage::MakeFileName(string & strFullName) {
    char buffer[256] = {0};
    strFullName += this->m_strAbsPath;
    snprintf(buffer, 0x100, "snd_%x_%04x.cache", m_iThrdID, m_icount++);
    strFullName += buffer;
    return strFullName.c_str();
}

MMapBuf * FileCacheManage::GetFileDesc() {
    if (!this->m_dqeFileDescList.empty()) {
        MMapBuf * temp = this->m_dqeFileDescList.front();
        this->m_dqeFileDescList.pop_front();
        if (temp) {
            this->m_pFileCacheInfo->datalen -= temp->GetLen();
        }
        return temp;
    } else {
        return 0;
    }

}

BOOL FileCacheManage::Empty() {
    return this->m_dqeFileDescList.empty();
}
// 获取文件列表中文件的数目

DWORD FileCacheManage::GetFileNum() {
    return this->m_dqeFileDescList.size();
}

DWORD FileCacheManage::GetDataNum() {
    int data_num = 0;
    deque<MMapBuf *>::const_iterator iter = this->m_dqeFileDescList.begin();
    for (; iter != this->m_dqeFileDescList.end(); iter++) {
        data_num += (*iter)->GetLen();
    }
    return data_num;
}

/*
 * 本来是用来限制所有的文件缓存的最大值的  后面改成无限缓存队列以后 就直接返回false
 */
bool FileCacheManage::IsHwm() {
    //    cout << "datalen[" << this->m_pFileCacheInfo->datalen << "] maxfilesize[" << this->m_dwMaxFileSize << "]" << endl;
    //    if (this->m_pFileCacheInfo->datalen >= m_dwMaxFileSize) {
    //        return true;
    //    }
    //    return false;
    return false;
}

bool ForceFileCache::SetFileIn(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) {
    if (!_pFileMng->IsHwm()) {
        MMapBuf * buf = _pNetIOBuf->GetBusyBuf();
        if (!buf) {
            return false;
        }
        _pFileMng->SetFileIn(buf);
        return true;
    }
    return false;
}

MMapBuf * ForceFileCache::GetFileFrom(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) {
    return _pFileMng->GetFileDesc();
}

NetIOBuf::NetIOBuf(NetBufSetting & _stNetBufSetting) : m_pNetBufInfo(0), m_pBufStat(0), m_pInbuf(0), m_pOutbuf(0) {
    m_NetBufSetting = _stNetBufSetting;
}//i add it 

NetIOBuf::~NetIOBuf() {
    if (m_pCacheMng) {
        delete m_pCacheMng;
        m_pCacheMng = 0;
    }
    if (m_pBufStat) {
        delete m_pBufStat;
        m_pBufStat = 0;
    }
    deque<MMapBuf *>::iterator iter = m_lstBusy.begin();
    for (; iter != m_lstBusy.end(); iter++) {
        delete *iter;
    }
    m_lstBusy.clear();
    iter = m_lstIdle.begin();
    for (; iter != m_lstIdle.end(); iter++) {
        delete *iter;
    }
    m_lstIdle.clear();
    if (m_pInbuf) {
        delete m_pInbuf;
        m_pInbuf = 0;
    }
    if (m_pOutbuf) {
        delete m_pOutbuf;
        m_pOutbuf = 0;
    }
}//i add it 

bool NetIOBuf::Init(NetBufInfo * _pNetBufInfo) {
    this->m_pNetBufInfo = _pNetBufInfo;
    m_pCacheMng = new FileCacheManage(&_pNetBufInfo->filecache, m_NetBufSetting.dwMaxFileSize);
    this->ChangeStat(CLOSE);
    m_pBufStat = new NetBufStat(this);
    if (m_NetBufSetting.BufMode == FORCE_FILECACHE) {
        if (m_NetBufSetting.iMaxMemBufSize > 0x0FFFFFF) {
            m_NetBufSetting.iMaxMemBufSize = 0x1000000;
        }
        if (m_NetBufSetting.dwMaxFileSize < this->m_NetBufSetting.iProperSize) {
            m_NetBufSetting.dwMaxFileSize = this->m_NetBufSetting.iProperSize * 2;
        }
    }
    if (m_NetBufSetting.BufMode == WITHOUT_FILECACHE) {
        if (m_NetBufSetting.iMaxMemBufSize < this->m_NetBufSetting.iProperSize) {
            m_NetBufSetting.iMaxMemBufSize = this->m_NetBufSetting.iProperSize;
        }
    }

    while (this->m_lstIdle.size()<this->m_NetBufSetting.wMaxBufCount) {
        this->SetIdleBuf(this->GetNewBuf());
    }
    return true;
}//i add it 

MMapBuf * NetIOBuf::GetCrntInBuf(BYTE *&pInBuf, INT & iCrntPos) {
    MMapBuf * tmp = this->GetInBuf();
    if (tmp) {
        pInBuf = (unsigned char *) tmp->Get();
        iCrntPos = tmp->GetLen();
    } else {
        pInBuf = 0;
    }
    return tmp;
}//i add it 

MMapBuf * NetIOBuf::GetCrntInBufAndTest(BYTE *&pCache, INT &iCrntPos, INT iAddLen) {
    if (!this->GetInBuf()) {
        return 0;
    }
    iCrntPos = m_pInbuf->GetLen();
    int size = m_pInbuf->GetSize();
    if (iCrntPos + iAddLen <= size) {
        iCrntPos = m_pInbuf->GetLen();
        pCache = (BYTE*) m_pInbuf->Get();
        return m_pInbuf;
    }
    int ret = this->Expand(iCrntPos + iAddLen);
    if (!ret) {
        return 0;
    }
    int spared = m_pInbuf->GetSpared();
    if (iAddLen <= spared) {
        iCrntPos = m_pInbuf->GetLen();
        pCache = (BYTE*) m_pInbuf->Get();
        return m_pInbuf;
    }
    return 0;


}//i add it 

MMapBuf * NetIOBuf::GetOutBuf() {
    m_pOutbuf = m_pFileCacheStat->GetFileFrom(this, this->m_pCacheMng);
    if (!m_pOutbuf) {
        m_pOutbuf = this->GetBusyBuf();
        if (!m_pOutbuf) {
            if (this->m_pInbuf) {
                m_pOutbuf = m_pInbuf;
                m_pInbuf = 0;
            }
        }
    }
    return m_pOutbuf;
}

void NetIOBuf::FreeBuf() {
    if (!m_pOutbuf) {
        return;
    }
    m_pOutbuf->release();
    int len = m_pOutbuf->GetSize();
    if (len <= m_NetBufSetting.iMaxMemBufSize) {
        if (this->m_lstIdle.size() < m_NetBufSetting.wMaxBufCount) {
            this->m_pOutbuf->SetLen(0);
            this->SetIdleBuf(this->m_pOutbuf);
            this->m_pOutbuf = 0;
            return;
        }
    }
    if (this->m_pOutbuf->GetFile()) {
        try {
            boost::filesystem::path p(this->m_pOutbuf->GetFile());
            if (boost::filesystem::exists(p)) {
                boost::filesystem::remove(p);
            }
        } catch (std::exception &e) {
            cout << "error NetIOBuf::FreeBuf delete file[" << this->m_pOutbuf->GetFile() << "] exception " << e.what() << endl;
        }
    }
    delete this->m_pOutbuf;
    this->m_pOutbuf = 0;
}//i add it 

void NetIOBuf::SetAllBufInFile() {

    while (m_pFileCacheStat->SetFileIn(this, this->m_pCacheMng));
}//i add it 

bool NetIOBuf::Empty() {
    bool ret = m_pFileCacheStat->Empty(this->m_pCacheMng);
    if (!ret) {
        return ret;
    }
    if (!this->m_lstBusy.empty()) {
        return false;
    }
    if (!this->m_pInbuf) {
        return true;
    }
    if (!this->m_pInbuf->GetLen()) {
        return true;
    }
    return false;

}//i add it 

bool NetIOBuf::EnFileCache() {
    if (!this->m_pFileCacheStat) {

        return false;
    }
    return this->m_pFileCacheStat->EnFileCache();

}//i add it 

void NetIOBuf::ChangeStat(ENBUFSTAT _stat) {
    m_pNetBufInfo->crntmode = this->m_NetBufSetting.BufMode;
    switch (m_NetBufSetting.BufMode) {
        case 0:
            this->m_pFileCacheStat = WithOutFileCache::Instance();
            break;
        case 1:
            this->m_pFileCacheStat = ForceFileCache::Instance();
            break;
        default:
            this->m_pFileCacheStat = WithOutFileCache::Instance();
            break;
    }
}//i add it 

NetBufStat * NetIOBuf::GetNetBufStat() {

    return this->m_pBufStat;
}//i add it 

NetBufInfo * NetIOBuf::GetNetBufInfo() {

    return this->m_pNetBufInfo;
}//i add it 

bool NetIOBuf::SetIn() {
    if (this->m_lstBusy.size() >= m_NetBufSetting.wMaxBufCount) {
        int ret = this->m_pFileCacheStat->SetFileIn(this, this->m_pCacheMng);
        if (ret) {
            return this->SetIn();
        }
        return false;
    }
    this->SetBusyBuf(m_pInbuf);
    this->m_pInbuf = 0;
    return true;
}//i add it 

bool NetIOBuf::TryBufLen(int iNewLen) {
    MMapBuf * tmp = this->GetInBuf();
    if (tmp->GetSize() > iNewLen) {
        return true;
    }
    return false;
}//i add it 

bool NetIOBuf::Expand(INT iNewLen) {
    m_pNetBufInfo->expandtimes += 1;
    int oldlen = this->m_pInbuf->GetLen();
    /*
     * 如果使用文件缓存
     */
    int maxbufsize = 0;
    if (this->m_pInbuf->GetMode() == MMAPFILE)maxbufsize = this->m_NetBufSetting.dwMaxFileSize;
    else maxbufsize = this->m_NetBufSetting.iMaxMemBufSize;
    if (iNewLen > maxbufsize) {
        bool ret = this->SetIn();
        if (!ret) {
            m_pNetBufInfo->expandfailed += 1;
            return false;
        }
        this->m_pInbuf = this->GetInBuf();
        if (!this->m_pInbuf) {
            return false;
        }
        ret = this->m_pInbuf->ReSize(this->m_NetBufSetting.iProperSize);
        if (ret) {
            this->m_pInbuf->SetLen(0);
            return true;
        }
        return false;
    } else {
        bool ret = this->m_pInbuf->ReSize(iNewLen);
        if (ret) {
            this->m_pInbuf->SetLen(oldlen);
            return true;
        }
        return false;
    }
}
//bool NetIOBuf::Expand(INT iNewLen) {
//    m_pNetBufInfo->expandtimes += 1;
//    int oldlen = this->m_pInbuf->GetLen();
//    /*
//     * 如果使用文件缓存
//     */
//    int maxbufsize = 0;
//    if (this->m_pInbuf->GetMode() == MMAPFILE)maxbufsize = this->m_NetBufSetting.dwMaxFileSize;
//    else maxbufsize = this->m_NetBufSetting.iMaxMemBufSize;
//
//
//    if (this->m_pInbuf->GetMode() == MMAPFILE) {
//        if (iNewLen > this->m_NetBufSetting.dwMaxFileSize) {
//            bool ret = this->SetIn();
//            if (!ret) {
//                m_pNetBufInfo->expandfailed += 1;
//                return false;
//            }
//            this->m_pInbuf = this->GetInBuf();
//            if (!this->m_pInbuf) {
//                return false;
//            }
//            ret = this->m_pInbuf->ReSize(this->m_NetBufSetting.iProperSize);
//            if (ret) {
//                this->m_pInbuf->SetLen(0);
//                return true;
//            }
//            return false;
//        } else {
//            bool ret = this->m_pInbuf->ReSize(iNewLen);
//            if (ret) {
//                this->m_pInbuf->SetLen(oldlen);
//                return true;
//            }
//            return false;
//        }
//    } else {
//        /*
//         * 如果使用内存缓存
//         */
//
//        bool ret = this->m_pInbuf->ReSize(iNewLen);
//        if (ret) {
//            this->m_pInbuf->SetLen(oldlen);
//            return true;
//        } else {
//            ret = this->SetIn();
//            if (!ret) {
//                m_pNetBufInfo->expandfailed += 1;
//                return 0;
//            }
//            this->m_pInbuf = this->GetInBuf();
//            if (!this->m_pInbuf) {
//                return false;
//            }
//            ret = this->m_pInbuf->ReSize(this->m_NetBufSetting.iProperSize);
//            if (ret) {
//                this->m_pInbuf->SetLen(0);
//                return true;
//            }
//            return false;
//        }
//    }
//}

MMapBuf * NetIOBuf::GetNewBuf() {
    MMapBuf * tmp_buf = new MMapBuf(m_NetBufSetting.BufMode, m_NetBufSetting.iMaxMemBufSize); /*iMaxMemBufSize*/
    if (m_NetBufSetting.BufMode == FORCE_FILECACHE) {
        string filename;
        tmp_buf->SetFile(this->m_pCacheMng->MakeFileName(filename));
    }
    bool ret = tmp_buf->ReSize(m_NetBufSetting.iProperSize);
    if (!ret) {
        delete tmp_buf;
        return 0;
    }
    tmp_buf->SetLen(0);
    return tmp_buf;

}//i add it 

MMapBuf * NetIOBuf::GetInBuf() {
    if (!this->m_pInbuf) {
        if (!this->m_lstIdle.empty()) {
            this->m_pInbuf = this->GetIdleBuf();
            if (this->m_pInbuf) {
                this->m_pInbuf->SetLen(0);
            }
        } else {
            this->m_pInbuf = this->GetNewBuf();
            if (this->m_pInbuf) {
                this->m_pInbuf->SetLen(0);
            }
        }
    }
    return this->m_pInbuf;
}//i add it 

MMapBuf * NetIOBuf::GetIdleBuf() {
    if (this->m_lstIdle.empty()) {
        return 0;
    }
    MMapBuf *tmp = m_lstIdle.front();
    m_lstIdle.pop_front();
    this->m_pNetBufInfo->idle.bufnum--;
    this->m_pNetBufInfo->idle.datalen -= tmp->GetSize();
    return tmp;
}//i add it 

void NetIOBuf::SetIdleBuf(MMapBuf * _pBuf) {
    this->m_lstIdle.push_back(_pBuf);
    this->m_pNetBufInfo->idle.bufnum++;
    this->m_pNetBufInfo->idle.datalen += _pBuf->GetSize();
}//i add it 

void NetIOBuf::SetBusyBuf(MMapBuf * _pBuf) {
    this->m_lstBusy.push_back(_pBuf);
    this->m_pNetBufInfo->busy.bufnum++;
    this->m_pNetBufInfo->busy.datalen += _pBuf->GetLen();
}//i add it 

MMapBuf * NetIOBuf::GetBusyBuf() {
    MMapBuf * tmp_buf = 0;
    if (!this->m_lstBusy.empty()) {
        tmp_buf = this->m_lstBusy.front();
        int len = tmp_buf->GetLen();
        m_pNetBufInfo->busy.datalen -= len;
        m_pNetBufInfo->busy.bufnum--;
        this->m_lstBusy.pop_front();
    }
    return tmp_buf;
}//i add it 

void NetIOBuf::Dump() {
    if (!m_pNetBufInfo) {
        return;
    }
    this->m_pNetBufInfo->busy.datalen = 0;
    this->m_pNetBufInfo->busy.bufnum = this->m_lstBusy.size();
    deque<MMapBuf *>::iterator iter = this->m_lstBusy.begin();
    for (; iter != m_lstBusy.end(); iter++) {
        this->m_pNetBufInfo->busy.datalen += (*iter)->GetLen();
    }
    this->m_pNetBufInfo->idle.datalen = 0;
    this->m_pNetBufInfo->idle.bufnum = this->m_lstIdle.size();
    if (m_pFileCacheStat) {
        this->m_NetBufSetting.BufMode = m_pFileCacheStat->GetMode();
    }
    if (m_pCacheMng) {
        m_pNetBufInfo->filecache.bufnum = this->m_pCacheMng->GetFileNum();
        m_pNetBufInfo->filecache.datalen = this->m_pCacheMng->GetDataNum();
    }
}
