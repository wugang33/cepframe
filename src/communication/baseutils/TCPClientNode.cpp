/* 
 * File:   TCPClientNode.cpp
 * Author: WG
 * 
 * Created on 2012年2月13日, 上午9:23
 */

#include "communication/baseutils/TCPClientNode.h"

TCPClientNode::TCPClientNode(int socket, sockaddr_in * addr, TCPSocketHandler * hndl) {
    this->m_socket = new TCPSocket(socket, addr);
    this->m_addr.sin_addr.s_addr = addr->sin_addr.s_addr;
    this->m_addr.sin_family = addr->sin_family;
    this->m_addr.sin_port = addr->sin_port;
    this->m_handler = hndl->Clone();
    this->m_bIsDead = false;
    this->thread_ = 0;
    this->isrunning = false;
}

TCPClientNode::~TCPClientNode() {
    this->isrunning = false;
    if (this->thread_) {
        this->thread_->join();
        delete this->thread_;
        this->thread_ = 0;
    }
    if (this->m_socket) {
        m_socket->Close();
        delete this->m_socket;
    }
    delete this->m_handler;
}

void TCPClientNode::GetAddr(sockaddr_in* addr) {
    addr->sin_addr.s_addr = this->m_addr.sin_addr.s_addr;
    addr->sin_port = this->m_addr.sin_port;
    addr->sin_family = this->m_addr.sin_family;
}

bool TCPClientNode::CheckClient() {
    return this->is_dead();
}

void TCPClientNode::run() {
    const char * ip = inet_ntoa(this->m_addr.sin_addr);
    this->m_handler->OnConnect(this->m_socket, ip, this->m_addr.sin_port);
    while (this->isrunning) {
        int ret = this->m_socket->Select(1000, 0);
        if (ret == 0) {//idle
            this->m_handler->OnIdle(this->m_socket);
            continue;
        }
        if (ret > 0) {//read
            this->m_handler->OnRead(this->m_socket);
            if (this->m_socket->GetReadFlag() > 0) {
                continue;
            }
        }
        this->isrunning = false;
        this->m_handler->OnClose();
        this->m_socket->Close();
        this->m_lockDead.lock();
        this->m_bIsDead = true;
        this->m_lockDead.unlock();
    }
}
