#ifndef __ZTP_SERVER_HANDLER_H__
#define __ZTP_SERVER_HANDLER_H__

#include "communication/baseutils/ZTPServer.h"
#include "communication/baseutils/lzowrapper.h"
#include "communication/baseutils/ZTPSendThread.h"

/*
 @brief ZTP服务处理，每个对象对应一个连接
 */
class ZTPServerHandler : public TCPSocketHandler {
private:
    DWORD m_dwPktSize; //0ch
public:
    ZTPServerHandler(ZTPHandler * hndl, UINT localid, bool bEncrypt, bool bCompress,
            DWORD dwPktSize = 8 * 1024, BYTE icompresslev = COMPRESS_LEVEL_FAST);
    virtual ~ZTPServerHandler();
    /*
     @brief 克隆方法，创建一个新的ZTPService对象。
     */
    virtual TCPSocketHandler * Clone();

    virtual void OnConnect(TCPSocket * conn, const char * host, int port);
    virtual void OnRead(TCPSocket * conn);
    virtual void OnClose();
    virtual void OnFail(const char * msg);
    virtual int SendData(const void * data,
            ZTP_DATA_INFO * info, int tTimeOut = ZTP_DEFAULT_TIMEOUT);

    virtual int SendData(const void * data,
            ZTP_DATA_INFO * info, UINT nodeid, int tTimeOut = ZTP_DEFAULT_TIMEOUT);

    virtual void SendPacket(TCPSocket * conn, WORD cmd, WORD cmd_status, const void * buf, size_t len);
    virtual void OnIdle(TCPSocket * conn);
    virtual bool dump(vector<vector<char> > *arryInfo);

    void SetNetBufSetting(NetBufSetting * _pNetBufSetting) {
        m_pNetBufSetting = _pNetBufSetting;
    }
private:
    void doBind(TCPSocket * conn, const unsigned char * buf, int len);
    void doCheckLink(TCPSocket * conn, const unsigned char * buf, int len);
    void doUnbind(TCPSocket * conn, const unsigned char * buf, int len);
    void doSendRecord(TCPSocket * conn, const unsigned char * buf, int len, const WORD cmd_status);
    unsigned char m_buf[65536 + 32]; //10h //65568
    unsigned int m_hash[2048]; //10030h //一共是73760 12020h
    //unsigned char m_grouphash[65536];
    time_t m_timestamp; //12030h
    ZTPHandler * m_hndl; //12038h
    ZTPSendThread * m_sendthread; //12040h
    ZTPNetStat * m_pConnStat; //12048h
    boost::mutex m_sendthreadlock; //12050h
    bool m_bEncrypt; //12078h
    NetBufSetting * m_pNetBufSetting; //12080h
    bool m_bCompress; // 是否压缩//12088h
    BYTE m_iCompressLevel; // 压缩级别 1(最快),非1(最好)//12089h
    CLzoWrapper m_lzo; //12090h
    BYTE *m_lzobuf; //120a0h
    int m_lzobuflen; //120a8h
private:
    ZTP_DATA_INFO m_info; //120ach
    vector<char> m_rcvbuf; //120c0h
private:
    UINT m_localid;
    UINT m_remoteid;
};

#endif
