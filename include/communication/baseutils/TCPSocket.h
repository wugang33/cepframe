#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "communication/baseutils/config.h"

typedef unsigned char BYTE;

/**
 @brief 网络连接状态机
 */
class INetStat {
public:

    virtual ~INetStat() {
    }
    virtual void Connect(const sockaddr_in * remote, const sockaddr_in * local) = 0;
    virtual void ReConn(const sockaddr_in * remote, const sockaddr_in * local) = 0;
    virtual void ConnClose(BYTE cause) = 0;
    virtual void Congestion() = 0;
};

/**
 @brief TCP连接状态类
 */
class TCPStat : public INetStat {
public:

    TCPStat() {
    }

    virtual ~TCPStat() {
    }

    virtual void ConnClose(BYTE cause) {//vtable 20h
    }

    virtual void Connect(const sockaddr_in * remote, const sockaddr_in * local) {
    }

    virtual void ReConn(const sockaddr_in * remote, const sockaddr_in * local) {
    }

    virtual void Congestion() {
    }
private:

};

/**
 @brief 新的Socket类实现，对TCPSocket单独实现
 */
class TCPSocket {
public:
    /** 
     @brief 创建TCPSocket对象
     */
    TCPSocket();

    /** 
     @brief 创建TCPSocket对象,绑定IP
     */
    TCPSocket(const char *ip);

    /** 
     @brief 创建TCPSocket对象,绑定IP和端口
     */
    TCPSocket(const char *ip, int port);

    /**
     @brief 创建TCPSocket对象，并绑定socket句柄和地址

     该构造函数用于封装已有的socket句柄，主要用于服务端接收连接时创建新的TCPSocket对象。
     */
    TCPSocket(int sock, sockaddr_in *addr);

    virtual ~TCPSocket();

    /**
     @brief 连接客户端
     */
    bool Connect(const char * host, int port);

    /**
     @brief 对socket执行select操作
     @param timeout 超时（毫秒）
     @param mode 0:读操作，1:写操作
     */
    int Select(int timeout = 1000, int mode = 0);
    /**
     @brief 取得最后一次对socket进行读操作的返回值
     */
    int GetReadFlag();
    /**
     @brief 取得最后一次对socket进行写操作的返回值
     */
    int GetWriteFlag();
    /**
     @brief 接收数据
     */
    int Read(void * data, size_t len);
    /**
     @brief 读取数据填满缓冲区，直到发生错误为止
     */
    int ReadAll(void * data, size_t len);
    /**
     @brief 写数据
     */
    int Write(const void * data, size_t len);
    /**
     @brief 写所有数据，直到发生错误为止
     */
    int WriteAll(const void * data, size_t len);
    /**
     @brief zerocopy
     */
    int Tranferto(int in_fd, off_t *offset, size_t count);
    /**
     @brief 关闭连接
     */
    void Close();

    /**
     @brief 设置socket缓冲区参数
     */
    void SetSockBufOpt(int SndBuf, int RcvBuf) {
        m_SndBuf = SndBuf;
        m_RcvBuf = RcvBuf;

    }

    int GetFd() {
        return m_socket;
    }

    void SetOption(const char *optname, int ival);

    void SetStat(TCPStat * _pConnStat) {
        m_pConnStat = _pConnStat;
    }

    INetStat * GetStat() {
        return m_pConnStat;
    }

    bool IsValidSocket();

    const sockaddr_in * GetRemoteAddr() {
        return &m_remote;
    }

    const sockaddr_in * GetLocalAddr() {
        return &m_local;
    }
private:
    int m_socket; //8H
    sockaddr_in m_addr; //0ch
    sockaddr_in m_remote; //1ch
    sockaddr_in m_local; //2ch
    volatile int m_rflag; //3ch
    volatile int m_wflag;
    int m_RcvBuf;
    int m_SndBuf;
    bool m_bnodelay; //4ch
    TCPStat * m_pConnStat;
};

/**
 @interface TCPSocketHandler
 @brief TCP socket 事件处理器

 该接口用于 TCPServer 类在接收TCP连接时调用。实现该类可以处理TCP连接中的各种事件。
 TCPServer 为每个连接都创建一个 TCPSocketHandler 对象。

 @see TCPServer
 */
class TCPSocketHandler {
public:

    virtual ~TCPSocketHandler() {
    }
    /**
     @brief 克隆方法，从现有的实例对象创建一个新的对象
     */
    virtual TCPSocketHandler * Clone() = 0;

    /**
     @brief 连接事件

 当一个新的连接建立时，将触发该方法
     @param conn TCP连接对象
     @param host 对端IP地址
     @param port 对端的端口号
     */
    virtual void OnConnect(TCPSocket * conn, const char * host, int port) = 0;
    /**
     @brief 读事件

 当对端有数据发到本机时，将触发该方法。但具体的读方式必须自己实现。
	 
     @param conn 连接对象
     */
    virtual void OnRead(TCPSocket * conn) = 0;

    /**
     @brief 对端关闭，或本地强制关闭了连接
     */
    virtual void OnClose() = 0;

    /**
     @brief 异常事件

 当产生异常时，将触发该方法，主要用于输出日志

 @param msg 错误信息
     */
    virtual void OnFail(const char * msg) = 0;

    virtual void OnIdle(TCPSocket * conn) = 0;
};

#endif
