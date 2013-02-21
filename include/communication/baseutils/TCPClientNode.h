#ifndef __TCP_CLIENT_NODE_H__
#define __TCP_CLIENT_NODE_H__

#include "communication/baseutils/TCPSocket.h"
#include "boost/thread.hpp"

class TCPClientNode {
public:
    TCPClientNode(int socket, sockaddr_in * addr, TCPSocketHandler * hndl);
    virtual ~TCPClientNode();
    void run();
    void GetAddr(sockaddr_in* addr);
    bool CheckClient();

    TCPSocketHandler * GetHandler() {
        return m_handler;
    }

    bool is_dead() {
        bool isdead;
        m_lockDead.lock();
        isdead = m_bIsDead;
        m_lockDead.unlock();
        return isdead;
    }

    void start() {
        if (this->thread_)return;
        this->isrunning = true;
        this->thread_ = new boost::thread(boost::bind(&TCPClientNode::run, this));
    }
private:
    TCPSocket * m_socket; //0xc0h
    TCPSocketHandler * m_handler; //0xc8h
    sockaddr_in m_addr; //0xd0h
    boost::thread * thread_;
    //    Mutex m_lockDead; //0xe0
    boost::mutex m_lockDead;
    bool isrunning;
    bool m_bIsDead; //0x108h
};

#endif
