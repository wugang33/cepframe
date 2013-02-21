#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "communication/baseutils/TCPSocket.h"
#include "boost/thread.hpp"
using namespace std;
class TCPClientNode;

/**
 @brief TCP服务器类
 
 TCPServer 在创建时注册一个 TCPSocketHandler 对象。在客户端有连接请求时，
 为客户端创建一个接收线程，并从 TCPSocketHandler 中克隆出一个新的实例，
 在这个接收线程中处理所有事件。

 */
class TCPServer {
public:

    /**
     @interface Action
     @brief 对所有连接进行批量操作的动作接口类

 实现该接口对所有 TCPSocketHandler 进行批量处理。
     @see ForEach
     */
    class Action {
    public:
        /**
         @brief 需要执行的操作
         @param hndl 要处理的对象
         */
        virtual void exec(TCPSocketHandler * hndl) = 0;
    };

    enum TCPServerOption {
        OPT_ONE_INSTANCE_PER_IP
    };
    virtual ~TCPServer();
    /**
     @brief 建立TCP侦听服务器

 在指定的端口建立侦听服务器。
     @param handler 要处理的对象
     @param port 服务器端口号
     */
    TCPServer(TCPSocketHandler * handler, unsigned short port);

    /*
     * 这是我添加进来的  当启动TCPServer的时候port为0  
     * 那么内核就会指定一个空闲的port  所以如果Start成功以后 
     * 可以得到一个监听的端口号
     */
    unsigned short GetPort() {
        return this->m_port;
    }
    /**
     @brief 建立TCP侦听服务

 在指定的端口和IP建立侦听服务。
     @param handler 要处理的对象
     @param ip 绑定IP地址
     @param port 端口号
     */
    TCPServer(TCPSocketHandler * handler, const char * ip, unsigned short port);

    /**
     @brief 启动服务
     */
    bool Start();

    /**
     @brief 停止服务
     */
    void Stop();

    /**
     * @brief 设置只有一个服务器连接
     */
    void SetOption(TCPServerOption option, bool bFlag);

    void SetOption(const char *optname, int ival);

    /**
     @brief 对每个 TCPSocketHandler 做相同操作

 遍历所有连接的 TCPSocketHandler 对象。为了保证一致性，内部对连接列表加了锁。
     如果把所有对象返回到外部再遍历，那么可能这个过程中有的连接已经释放掉了，不安全。
     因此使用了ForEach这种方式。
 
 示例：
 @code
 class SomeAction : public TCPServer::Action
 {
 public:
     virtual void exec(TCPSocketHandler* hndl)
     {
         // do something with hndl
     }
 }
 TCPServer * server;
 ...
 SomeAction actor;
 server->ForEach(&actor);
 @endcode
	 
     */
    virtual void ForEach(Action * actor);

    void start() {
        if (this->thread_)return;
        this->isrunning = true;
        this->thread_ = new boost::thread(boost::bind(&TCPServer::run, this));
    }
    void run();
private:
    //    Mutex m_lock; //0xc0h
    boost::mutex m_lock;
    // 清除已经停止的连接
    void CheckClients();
    // 关闭已有的连接
    void CloseClient(struct sockaddr_in* addr);
    string m_ip; //e0
    unsigned short m_port; //f0
    int m_socket; //f4
    sockaddr_in m_addr; //f8h
    TCPSocketHandler * m_handler; //108h
    list<TCPClientNode*> m_clients; //110h
    boost::thread *thread_;
    bool isrunning;
    bool m_bSingleIp; //120h
    bool m_bnodelay; //121ch
};

#endif
