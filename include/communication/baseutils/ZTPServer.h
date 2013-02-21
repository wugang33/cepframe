#ifndef __ZTP_SERVER_H__
#define __ZTP_SERVER_H__

#include "communication/baseutils/TCPServer.h"
#include "communication/baseutils/TCPSocket.h"
#include "communication/baseutils/netiobuf.h"

const int ZTP_MAX_BUF_SIZE = 66560; // 65 * 1024

const int ZTP_SEND_SUCCESS = 0; // 发送成功
const int ZTP_SEND_TIME_OUT = 1; // 发送超时
const int ZTP_NOT_SEND = 2; // 由于客户端没有注册相应类型，不发送
const int ZTP_SEND_OVER_SIZE = 3; // 发送超大
const int ZTP_WAIT_INFINITE = -1; // 发送等待时永远等待
const int ZTP_NO_WAIT = 0; // 发送等待时间为不等待
const int ZTP_DEFAULT_TIMEOUT = 100 * 1000; // 发送默认等待时间 100s

const WORD ZTP_MAGIC_NUM = 0x9e62;
const int ZTP_BCM_HEADER_SIZE = 5; //12 9e62  34 packagesize 5 是否压缩 67 cmd 89 cmdstatus 10 11 0
const int ZTP_CMD_SIZE = 6;
const int ZTP_HEADER_SIZE = ZTP_BCM_HEADER_SIZE + ZTP_CMD_SIZE;


const int ZTP_RECORD_HEADER_SIZE = 10; //1 flag 23 type 45 ver_info 6 format 7890 datalen sizeof(ZTP_DATA_INFO_NET)
const WORD ZTP_CMD_BIND = 0x0001;
const WORD ZTP_CMD_BIND_RESP = 0x8001;
const WORD ZTP_CMD_SEND_RECORD = 0x0002;
const WORD ZTP_CMD_SEND_RECORD_RESP = 0x8002;
const WORD ZTP_CMD_CHECK_LINK = 0x0003;
const WORD ZTP_CMD_CHECK_LINK_RESP = 0x8003;
const WORD ZTP_CMD_UNBIND = 0x0004;
const WORD ZTP_CMD_UNBIND_RESP = 0x8004;

const WORD ZTP_CMD_STATUS_SUCCESS = 0; // 发送成功消息(单包数据或分组数据的最后一包)
const WORD ZTP_CMD_STATUS_INVLEN = 1;
const WORD ZTP_CMD_STATUS_INVCMD = 2;
const WORD ZTP_CMD_STATUS_INVSEQ = 3;
const WORD ZTP_CMD_STATUS_INVUSR = 4;
const WORD ZTP_CMD_STATUS_INVPASS = 5;
const WORD ZTP_CMD_STATUS_PKGSEG = 6; // 分组消息的非最后一包
const WORD ZTP_CMD_STATUS_UNKNOWN = 99;

class ZTPSendThread;
class ZTPServerHandler;
#ifdef _AIX
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct ZTP_BCM_HEADER {
    /*
     * ZTP包头的魔数 0x9e62
     */
    unsigned char magic[2];
    /*
     * 后续的包有多长     
     */
    unsigned short package_len;
    /*
     * 是不是压缩
     */
    unsigned char isCompress;
};

struct ZTP_CMD {
    WORD cmd;
    WORD cmd_status;
    WORD reserved;
};

struct ZTP_HEADER {
    ZTP_BCM_HEADER bcm_head;
    ZTP_CMD cmd;
    CHAR ztp_data[1];
};

/*
 * 网络上传输的ZTP_DATA_INFO的信息是如何传输的
 * 一共10个字节
 */
struct ZTP_RECORD_HEADER {
    BYTE flag; //120ac     //00  54
    WORD type; //120ae   //02  56
    WORD ver_info; //120b0   //04  58
    BYTE format; //120b2   //06   5a
    UINT data_len; //120b4   //08 5c
    CHAR record_data[1];
};

/*
 * do bind 的结构体
 * OLD是原来中兴用的
 * NEW是我现在用的
 */
struct BIND_COMMAND_OLD {
    CHAR username[16];
    CHAR password[16];
    /*
     * 客户端请求的命令字数量
     */
    WORD request_types;
    /*后面跟命令字 这里只写出来一个 真是数量是跟request_types数量一样多*/
    // WORD type;
};

struct BIND_COMMAND_NEW {
    CHAR username[14];
    WORD m_nodeid_high;
    CHAR password[14];
    WORD m_nodeid_low;
    /*
     * 客户端请求的命令字数量
     */
    WORD request_types;
    /*后面跟命令字 这里只写出来一个 真是数量是跟request_types数量一样多*/
    //  WORD type;
};

#ifdef _AIX
#pragma pack()
#else
#pragma pack(pop)
#endif

/**
 @brief ZTP数据包控制结构，包含数据类型、长度等信息
 */
struct ZTP_DATA_INFO {
    /**
     @brief 接口数据类型标志：
     - 0 原始信令数据
     - 1 XDR数据
     - 2 基础统计数据
     - 3 业务统计数据
     - 4 告警数据
     - 5 配置数据
     * 200 CEP信令 salve 向master请求信令 201 master向slave下发的信令
     */
    BYTE flag; //120ac     //00  54
    /**
     @brief 数据类型编号，与生成文件名中的TTTT值一致。
     对于XDR数据则取XDR的命令字作为编号。
 对SDTP为消息类型
     */
    WORD type; //120ae   //02  56
    /**
     @brief 该数据类型的版本信息。
     目前将包体中结构体长度作为版本信息。
 对SDTP为大包中的事件数量
     */
    WORD ver_info; //120b0   //04  58
    /**
     @brief 数据格式
     - 0 二进制
     - 1 文本
     - 2 XML
     */
    BYTE format; //120b2   //06   5a
    /**
     @brief 数据长度，不包括本结构体
     */
    UINT data_len; //120b4   //08 5c

    /**
         @brief 包长度，数据中每个包的长度
     */
    UINT pkg_len; //120b8   //0c
}; //16个字节长

/**
 @brief ZTP服务接口
 @interface ZTPService

 每个ZTP连接都对应一个 ZTPService 对象，供 ZTPHandler 接口访问，用于发送数据。
 在服务端，每个连接都有不同的 ZTPService 对象，从而可以满足对不同客户端的不同处理。
 向所有客户端广播数据时，则可以直接通过 ZTPServer 对象发送。
 */
class ZTPService {
public:

    virtual ~ZTPService() {
    }

    /**
     @brief 发送数据
     @param data 待发送的数据
     @param info 待发送数据的基本信息
     @param tTimeOut 发送超时时间
     */
    virtual bool SendData(const void * data,
            ZTP_DATA_INFO * info) = 0;

    /**
     @brief 发送数据包
     发送一个原始的ZTP数据包。见《ZXT2000(V5.2) 系统接口方案》。
     @param cmd 发送的命令
     @param cmd_status 命令状态
     @
     */
    virtual bool SendPacket(WORD cmd, WORD cmd_status, const void * buf, size_t len) = 0;
    /**
     * @brief 断开连接
     */
    virtual void Close() = 0;
};

/**
 @interface ZTPHandler
 @brief ZTP数据处理接口

 ZTPHandler 是ZTP服务读取数据时的回调接口。在应用时需要实现该接口。
 服务端和客户端都使用该接口进行事件处理。

 服务端对每个连接会通过 Clone() 方法创建一个 ZTPHandler 实例；服务端停止时
 会释放所有 Clone() 方法创建的示例，但不会释放在构造函数中注册的示例。

 客户端不会调用 Clone() 方法，也不会释放对象。

 @see ZTPServer, ZTPClient
 */
class ZTPHandler {
public:

    virtual ~ZTPHandler() {
    }

    /**
     @brief 错误处理
     */
    virtual void OnFail(const char * msg) {
    }
    /**
     @brief 克隆出一个新的对象
     */
    virtual ZTPHandler * Clone() = 0;

    /**
     @brief 连接事件
     @param service ZTPService对象，用于发送数据包
     @param 对端IP
     @param 对端端口号
     */
    virtual void OnConnect(ZTPService * service, const char * ip, int port) {
    }

    /**
     @brief 检查注册消息
     
 检查注册是否成功。如果返回true，则注册成功。
 如果返回false，则发送注册失败消息
     */
    virtual bool OnAuth(const char * user, const char * passwd, const WORD* cmds, int count, ZTPServerHandler* pServerHandler) {
        return true;
    }

    /**
     @brief 关闭事件
     */
    virtual void OnClose() {
    }

    /**
     @brief 接收数据
 @param data 数据地址
     @param info 数据描述信息，包括数据长度、数据指针等
 @note 客户端只接收通过 ZTPClient::AddDataType 注册的数据类型的数据。
     */
    virtual void OnReceive(const void * data,
            ZTP_DATA_INFO * info) {
    }

    /** 
     @brief 一段时间内没有收到任何包，即收包超时
     */
    virtual void OnIdle() {
    }

};

/// 一个仅发送数据，不接收数据的客户端 ZTPHandler

class ZTPSendOnlyHandler : public ZTPHandler {
public:

    virtual ~ZTPSendOnlyHandler() {
    }

    /**
     @brief 复制一份现有对象
     */
    virtual ZTPHandler * Clone() {
        return new ZTPSendOnlyHandler;
    }
};

/**
 @brief ZTP通讯服务端

 实现《ZXT2000(V5.2) 系统接口方案》中定义的通讯接口，封装了底层的绑定、信号检查等处理，
 上层只需关心数据的发送、接收。创建 ZTPServer 对象时需注册一个 ZTPHandler 事件处理对象。
 ZTPServer 为每个连接创建一个新的 ZTPHandler 和一个 ZTPService 来处理各个连接的事件。

 客户端在连接时注册所需的数据类型。服务端发送数据时只会向客户端发送已注册的数据。

 ZTPServer::SendData 可以向所有客户端广播数据。如果要向不同的客户端发送不同的数据，
 可以在 ZTPHandler 中向 ZTPService 发送数据来实现。这种情况下不会对数据类型进行检查。

 @see ZTPHandler, ZTPService
 */
class ZTPServer {
public:

    class Builder {
    public:
        NetBufSetting m_NetBufSetting;
        UINT m_localid;
        DWORD m_dwPktSize; // 数据包的最大长度//38h
        WORD m_port; // 端口  //30h
        bool m_bCompress; // 是否压缩//3ch
        BYTE m_iCompressLevel; // 压缩级别 1(最快),非1(最好)3dh
        bool m_bEncrypt; // 是否加密 //3eh
        friend std::ostream& operator<<(std::ostream&, const ZTPServer::Builder&);

        Builder() : m_port(3389), m_localid(0), m_dwPktSize(8 * 1024), m_bCompress(false), m_iCompressLevel(1), m_bEncrypt(0) {
            m_NetBufSetting.BufMode = WITHOUT_FILECACHE;
            m_NetBufSetting.dwMaxFileSize = 50 * 1024 * 1024;
            m_NetBufSetting.iMaxMemBufSize = 10 * 1024 * 1024;
            m_NetBufSetting.iProperSize = 0x20000;
            m_NetBufSetting.wMaxBufCount = 4;
        }

        void reset() {
            m_port = 3389;
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
    /**
     @brief 构造函数
     @param hndl 事件响应句柄
     @param port 侦听端口号
     */
    ZTPServer(ZTPHandler * hndl, int port, UINT localnodeid = 0, DWORD dwBufSize = 1024 * 1024, bool bEncrypt = false,
            DWORD dwPktSize = 8 * 1024);

    ZTPServer(ZTPHandler * hndl, const ZTPServer::Builder&builder);

    virtual ~ZTPServer();

    /**
     @biref 启动服务
     @return true-成功，false-失败
     */
    bool Start();

    /**
     @brief 停止服务
     */
    void Stop();

    unsigned short GetPort() {
        return this->m_tcpserver->GetPort();
    }
    /**
     @brief 向所有连接到的客户端广播数据
     */
    void SendData(const void * data,
            ZTP_DATA_INFO * info);

    void SendData(const void * data,
            ZTP_DATA_INFO * info, UINT nodeid);

    int SetOption(const char *optname, UINT optvalue);

    int SetUsernamePassword(const char *username, const char *password);

    void DumpAll(vector<vector<char> > *arryInfo);

private:
    TCPServer * m_tcpserver; //08h
    ZTPServerHandler * m_ztpservice; //10h
    ZTPHandler * m_ztphandler; //18h
    NetBufSetting m_NetBufSetting; //20h
    int m_port; // 端口  //30h
    DWORD m_dwPktSize; // 数据包的最大长度//38h
    bool m_bCompress; // 是否压缩//3ch
    BYTE m_iCompressLevel; // 压缩级别 1(最快),非1(最好)3dh
    bool m_bEncrypt; // 是否加密 //3eh
private:
    UINT m_localid;
};

/*
class ZTPDefaultParam: public ConfigData
{
public:
    ZTPDefaultParam(const char * pIniFile)
    {
      // 遍历读取所有的配置项
        IniFile ztp_config(pIniFile);
        int iItem = ztp_config.GetINIInt("cfginformationexplain", "cfgitemnumber", 0);
        
        char pSectionKey[100];
        char pKeyKey[100];
        char pSectionValue[100];
        char pKeyValue[100];
        char pCfgValue[100];
        for (int i = 1; i <= iItem; ++i)
        {
            sprintf(pSectionKey, "%s%d%s", "cfgitem", i, "_section");
            sprintf(pKeyKey, "%s%d%s", "cfgitem", i, "_key");
            ztp_config.GetINIString("cfginformationexplain", pSectionKey, "", pSectionValue);
            ztp_config.GetINIString("cfginformationexplain", pKeyKey, "", pKeyValue);
            
            ztp_config.GetINIString(pSectionValue, pKeyValue, "", pCfgValue);
            
            if (strcmp(pCfgValue, "") == 0)
            {
                continue;
            }
            
           // setString(pKeyValue, pCfgValue);
        }
    }
    ~ZTPDefaultParam()
    {
        
    }
private:
};
 */
#endif
