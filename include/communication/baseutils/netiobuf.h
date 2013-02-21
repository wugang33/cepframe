#ifndef _netiobuf_H
#define	_netiobuf_H

#include "communication/baseutils/growbuf.h"
#include "communication/baseutils/mmap.h"
#include "communication/baseutils/TCPSocket.h"
#include "communication/baseutils/config.h"
#include "boost/thread.hpp"
//#include "communication/baseutils/FileList.h"

typedef struct _NetBufSetting {
    UINT iMaxMemBufSize; //0x00  d0  14
    UINT iProperSize; //0x04   d4     18
    DWORD dwMaxFileSize; //0x0c
    WORD wMaxBufCount; //0x08   d8   1c
    unsigned char BufMode; //0x0a  1e
    char unused[1];
} NetBufSetting;

// 缓冲列表信息

typedef struct _NetBufLstInfo {
    UINT bufnum; // buf数
    UINT datalen; // 总大小
} NetBufLstInfo;

typedef struct _NetBufInfo {
    NetBufLstInfo busy; // 忙列表信息0x00
    NetBufLstInfo idle; // 闲列表信息0x08
    NetBufLstInfo filecache; //0x10
    WORD expandtimes; // 扩张次数//0x18
    WORD expandfailed; // 扩张失败次数0x1a
    BYTE crntmode; // 当前模式0x1c
    BYTE unused[3]; //

    friend std::ostream& operator<<(std::ostream&out, const _NetBufInfo & info) {
        out << "\nBUF_STATE:bufmode[" << (unsigned int) info.crntmode << "]\tbusy_buffer_num[" << info.busy.bufnum
                << "]\ttotallens[" << info.busy.datalen << "]\tidle_buffer_num[" << info.idle.bufnum
                << "]\ttotallens[" << info.idle.datalen << "]\t\nfilecache_num  [" << info.filecache.bufnum
                << "]totallens[" << info.filecache.datalen << "]\texpandtimes[" << info.expandtimes
                << "]expandfailed[" << info.expandfailed << "]\n";
        return out;
    }
} NetBufInfo;

typedef enum {
    CLOSE = 0,
    RECONN,
    CONGEST
} ENBUFSTAT;

typedef enum {
    WITHOUT_FILECACHE = 0,
    FORCE_FILECACHE,
    NUM_OF_MBUFMODE
} ENUMBUFMODE;

class IFileCacheStat;
class WithOutFileCache;
class ForceFileCache;
//class SoftFileCache;
class FileCacheManage;
class NetIOBuf;
class NetBufStat;
//class CFileList;

/** 
 * @class 
 * @brief
 * NetBufMode:   缓存模式，value=0|1|2
 *               0:内存缓存，兼容之前模式
 *               1:强制文件缓存，设置使用文件缓存模式，当连接断开或后端堵塞导致当前缓存列表满时将缓存块加入文件管理器
 * wMaxBufCount: 缓存列表中缓存块的个数，用于交换，相当于以前的 m_pInbuf和m_pOutbuf两块缓存交换写的作用的延伸
 *               有两个缓存列表m_lstBusy和m_lstIdle,每个列表的大小为NetBufCount;
 *               客户端写入时从m_lstIdle取一块缓存块，写满后加入m_lstBusy；
 *               发送时从m_lstBusy或文件管理器取一块缓存，写入socket后交回m_lstIdle；
 */

class NetIOBuf {
public:

    NetIOBuf(NetBufSetting & _stNetBufSetting);
    ~NetIOBuf();
    bool Init(NetBufInfo * _pNetBufInfo);
    MMapBuf * GetCrntInBuf(BYTE *&pInBuf, INT &iCrntPos);
    MMapBuf * GetCrntInBufAndTest(BYTE *&pCache, INT &iCrntPos, INT iAddLen);
    MMapBuf * GetOutBuf();
    MMapBuf * GetBusyBuf();
    void FreeBuf();
    void SetAllBufInFile();
    bool Empty();

    bool IdleListEmpty() {
        return m_lstIdle.empty();
    }
    bool EnFileCache();
    void ChangeStat(ENBUFSTAT _stat);
    NetBufStat * GetNetBufStat();
    NetBufInfo * GetNetBufInfo();

private:
    bool SetIn();
    bool TryBufLen(int iNewLen);
    bool Expand(INT iNewLen);
    MMapBuf * GetNewBuf();
    MMapBuf * GetInBuf();
    MMapBuf * GetIdleBuf();
    void SetIdleBuf(MMapBuf * _pBuf);
    void SetBusyBuf(MMapBuf * _pBuf);
    void Dump();

private:
    MMapBuf * m_pInbuf; //0x00
    MMapBuf * m_pOutbuf; //0x08
    IFileCacheStat * m_pFileCacheStat; // 文件缓存控制//10
    FileCacheManage * m_pCacheMng; // 缓存文件管理//18
    deque<MMapBuf *> m_lstBusy; // 缓存忙列表//0x20
    deque<MMapBuf *> m_lstIdle; // 缓存闲列表//0x70
    NetBufStat *m_pBufStat; // iobuf状态机 //0xc0
    NetBufInfo *m_pNetBufInfo; // buf状态信息从NetInfo获取//0xc8
    NetBufSetting m_NetBufSetting; //0xd0
    boost::mutex m_IdlLstLock; //0xf0
};

class FileCacheManage {
public:
    FileCacheManage(NetBufLstInfo * _pFileCacheInfo, DWORD MaxFileSize);
    ~FileCacheManage();

    bool SetFileIn(MMapBuf * pBuf);
    const char * MakeFileName(string &strFullName);
    MMapBuf * GetFileDesc();
    BOOL Empty();
    // 获取文件列表中文件的数目
    DWORD GetFileNum();
    DWORD GetDataNum();

    bool ChkSpace() {
        return true;
    }
    bool IsHwm();
private:
    NetBufLstInfo * m_pFileCacheInfo; //0x00
    deque<MMapBuf *> m_dqeFileDescList;
    DWORD m_dwMaxFileSize; //0x10
    string m_strAbsPath; //0x18
    int m_iThrdID; //0x20
    int m_icount; // 0x24
};

class IFileCacheStat {
public:
    virtual bool EnFileCache() = 0;
    virtual bool SetFileIn(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) = 0;
    virtual MMapBuf * GetFileFrom(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) = 0;
    virtual BOOL Empty(FileCacheManage * _pFileMng) = 0;
    virtual BYTE GetMode() = 0;
};

class WithOutFileCache : public IFileCacheStat {
public:

    static IFileCacheStat * Instance() {
        if (NULL == s_WithOutFileCache) {
            s_WithOutFileCache = new WithOutFileCache;
        }
        return s_WithOutFileCache;
    }

    bool EnFileCache() {
        return false;
    }

    bool SetFileIn(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) {
        return false;
    }

    MMapBuf * GetFileFrom(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng) {
        return NULL;
    }

    BOOL Empty(FileCacheManage * _pFileMng) {
        return true;
    }

    BYTE GetMode() {
        return WITHOUT_FILECACHE;
    }
private:
    static WithOutFileCache * s_WithOutFileCache;
};

class ForceFileCache : public IFileCacheStat {
public:

    static IFileCacheStat * Instance() {
        if (NULL == s_ForceFileCache) {
            s_ForceFileCache = new ForceFileCache;
        }
        return s_ForceFileCache;
    }

    bool EnFileCache() {
        return true;
    }
    bool SetFileIn(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng);
    MMapBuf * GetFileFrom(NetIOBuf * _pNetIOBuf, FileCacheManage * _pFileMng);

    BOOL Empty(FileCacheManage * _pFileMng) {
        return _pFileMng->Empty();
    }

    BYTE GetMode() {
        return FORCE_FILECACHE;
    }
private:
    static ForceFileCache * s_ForceFileCache;

};

class NetBufStat : public INetStat {
public:

    NetBufStat(NetIOBuf * _pNetIOBuf)
    : m_pNetIOBuf(_pNetIOBuf) {
        m_pNetBufInfo = m_pNetIOBuf->GetNetBufInfo();
    }

    ~NetBufStat() {
        m_pNetIOBuf = NULL;
    }

    void Connect(const sockaddr_in * remote, const sockaddr_in * local) {

    }
    //三种状态
    // connection closed->

    void ConnClose(BYTE cause) {
        ChangeStat(CLOSE);
    }
    // connection re_establish

    void ReConn(const sockaddr_in * remote, const sockaddr_in * local) {
        ChangeStat(RECONN);
    }
    // congestion

    void Congestion() {
        ChangeStat(CONGEST);
    }

    void Dump() {
        if (m_pNetIOBuf)
            m_pNetIOBuf->GetNetBufInfo();
    }

private:

    void ChangeStat(ENBUFSTAT _stat) {
        m_pNetIOBuf->ChangeStat(_stat);
    }
private:
    NetIOBuf * m_pNetIOBuf;
    NetBufInfo *m_pNetBufInfo;
};

#endif//_netclient_H

