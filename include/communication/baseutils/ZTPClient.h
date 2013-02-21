#ifndef __ZTP_CLIENT_H__
#define __ZTP_CLIENT_H__

#include "communication/baseutils/ZTPServer.h"
#include "communication/baseutils/ZTPSendThread.h"
#include "communication/baseutils/lzowrapper.h"
#include "boost/thread.hpp"

/**
 @brief ZTP客户端
 * 
 ZTP客户端类，用于向ZTP服务端连接。使用示例见examples/ztp/client.cpp

 @note ZTP客户端在Windows自动重连时会导致socket句柄泄露。这应该是Windows自身的bug。
 */
class ZTPClient {
public:

    class Builder {
    public:
        NetBufSetting m_NetBufSetting; //0x10514
        UINT m_localid;
        DWORD m_dwPktSize; // 最大包的大小//0x1052c
        bool m_bEncrypt; // 是否加密//0x10530
        bool m_bCompress; // 是否压缩//0x10531
        BYTE m_iCompressLevel; // 压缩级别 1(最快),非1(最好)//0x10532

        friend std::ostream& operator<<(std::ostream&, const ZTPClient::Builder&);

        Builder() : m_iCompressLevel(1), m_bCompress(false), m_bEncrypt(false), m_dwPktSize(8 * 1024), m_localid(0) {
            m_NetBufSetting.BufMode = WITHOUT_FILECACHE;
            m_NetBufSetting.dwMaxFileSize = 50 * 1024 * 1024;
            m_NetBufSetting.iMaxMemBufSize = 10 * 1024 * 1024;
            m_NetBufSetting.iProperSize = 0x20000;
            m_NetBufSetting.wMaxBufCount = 4;
        }

        void reset() {
            m_localid = 0;
            m_dwPktSize = 8 * 1024;
            m_bCompress = false;
            m_iCompressLevel = 1;
            m_bEncrypt = false;
            m_NetBufSetting.BufMode = WITHOUT_FILECACHE;
            m_NetBufSetting.dwMaxFileSize = 50 * 1024 * 1024;
            m_NetBufSetting.iMaxMemBufSize = 10 * 1024 * 1024;
            m_NetBufSetting.iProperSize = 0x20000;
            m_NetBufSetting.wMaxBufCount = 4;
        }
    };
public:
    virtual ~ZTPClient();
    /**
     @brief 构造函数
     @param hndl 事件处理句柄
     */
    ZTPClient(ZTPHandler * hndl, const ZTPClient::Builder&builder);
    /**
     @brief 在 Login 和 Start 之前调用，设置所需的数据类型
     */
    void AddDataType(WORD datatype);

    /**
     @brief 设置用户名和密码
     */
    void Login(const char * user, const char * passwd, unsigned int nodeid);

    /**
     @brief 启动客户端
     */
    bool Start(const char * dip, int dport, const char *oip = NULL, const int oport = 0, int SndBuf = 128 * 1024, int RcvBuf = 128 * 1024);

    /**
     @brief 停止客户端
     */
    void Stop();

    /**
     @brief 向服务端发送数据
     */
    int SendData(const void * data,
            ZTP_DATA_INFO * info,
            int tTimeOut = ZTP_DEFAULT_TIMEOUT);

    int SendData(const void * buf_head,
            UINT head_len,
            const void * data,
            ZTP_DATA_INFO * info,
            int tTimeOut = ZTP_DEFAULT_TIMEOUT);

    /** 
     @brief 直接向服务端发送数据, 不再组成大包
     */
    int SendDataDirectly(const void * buf,
            size_t buf_len,
            int tTimeOut = ZTP_DEFAULT_TIMEOUT);

    bool IsConnected();

    int SetOption(const char *optname, UINT optvalue);

    int SetUsernamePassword(const char *username, const char *password);

    void * GetStat() {
        if (!m_pConnStat)
            return NULL;
        return m_pConnStat->GetStatInfo();
    }

    unsigned int GetRemoteID() {
        return this->m_remoteid;
    }

    unsigned int GetLocalID() {
        return this->m_localid;
    }
    void run();

protected:
    void doSendRecord(TCPSocket * conn, const BYTE * buf, int len, const WORD cmd_status);
    void doRead(TCPSocket * conn);
    void forBind(TCPSocket * conn, const BYTE * buf, int len, const WORD cmd_status); //服务器向客服端的bind信息
    void doBind();
    void doCheckLink();
    void SendPacket(WORD cmd, WORD cmd_status, const void * buf, size_t len);
    void OnIdle();

    void start() {
        if (this->thread_)return;
        this->isrunning = true;
        this->thread_ = new boost::thread(boost::bind(&ZTPClient::run, this));
    }
protected:
    boost::thread * thread_;
    bool isrunning;
    ZTPHandler * m_handler; //0xc0
    TCPSocket * m_socket; //0xc8
    ZTPNetStat * m_pConnStat; //0xd0
    vector<WORD> m_datatypes; //0xd8
    BYTE m_buf[ZTP_MAX_BUF_SIZE]; //0xf0
    string m_user; //0x104f0
    string m_passwd; //0x104f8
    ZTPSendThread * m_sendthread; //0x10500
    string m_ip; //0x10508
    int m_port; //0x10510
    NetBufSetting m_NetBufSetting; //0x10514
    bool m_connected; //0x10524
    DWORD m_dwPktSize; // 最大包的大小//0x1052c
    bool m_bEncrypt; // 是否加密//0x10530
    bool m_bCompress; // 是否压缩//0x10531
    BYTE m_iCompressLevel; // 压缩级别 1(最快),非1(最好)//0x10532
    CLzoWrapper m_lzo; //0x10538
    BYTE *m_lzobuf; //10548
    int m_lzobuflen; //0x10550
    ZTP_DATA_INFO m_info; //0x10554
    vector<char> m_rcvbuf; //0x10568
    bool m_bPostStat; //0x10580
private:
    UINT m_localid;
    UINT m_remoteid;
};

#endif
