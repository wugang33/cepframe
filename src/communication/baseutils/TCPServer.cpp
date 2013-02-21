/* 
 * File:   TCPServer.cpp
 * Author: WG
 * 
 * Created on 2012年2月13日, 上午9:24
 */

#include "communication/baseutils/TCPServer.h"
#include "communication/baseutils/TCPClientNode.h"

TCPServer::~TCPServer() {
    if (this->m_handler) {
        this->m_handler->OnClose();
    }
    list<TCPClientNode*>::iterator iter = this->m_clients.begin();
    for (; iter != this->m_clients.end();) {
        delete *iter;
        this->m_clients.erase(iter++);
    }

}

TCPServer::TCPServer(TCPSocketHandler * handler, unsigned short port) {
    this->m_ip = "0.0.0.0";
    this->m_handler = handler;
    this->m_port = port;
    this->m_addr.sin_port = htons_ex(port);
    this->m_addr.sin_family = AF_INET;
    this->m_addr.sin_addr.s_addr = 0;
    this->m_bSingleIp = 0;
    this->m_bnodelay = 0;
    this->m_socket = -1;
    this->thread_ = 0;
    this->isrunning = false;
}

/**
 @brief 建立TCP侦听服务

在指定的端口和IP建立侦听服务。
 @param handler 要处理的对象
 @param ip 绑定IP地址
 @param port 端口号
 */
TCPServer::TCPServer(TCPSocketHandler * handler, const char * ip, unsigned short port) : m_ip(ip), m_handler(handler) {
    inet_aton(ip, &this->m_addr.sin_addr);
    this->m_port = port;
    this->m_addr.sin_port = htons_ex(port);
    this->m_addr.sin_family = AF_INET;
    this->m_handler = handler;
    this->m_bSingleIp = 0;
    this->m_bnodelay = 0;
    this->m_socket = -1;
    this->thread_ = 0;
    this->isrunning = false;
}

/**
 @brief 启动服务
 */
bool TCPServer::Start() {
    int ret = socket(AF_INET, SOCK_STREAM, 0);
    this->m_socket = ret;
    if (ret < 0) {
        perror("socket fail!");
        return false;
    }
    int set_val = 1;
    ret = setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &set_val, sizeof (int));
    if (ret == -1) {
        perror("setsockopt fail!");
        close(this->m_socket);
        return false;
    }
    if (this->m_bnodelay != 0) {
        int set_val = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_ERROR, &set_val, sizeof (int));
    }
    ret = bind(this->m_socket, (sockaddr *) & this->m_addr, sizeof (sockaddr));
    if (ret == -1) {
        perror("bind fail!");
        close(this->m_socket);
        return false;
    }
    ret = listen(this->m_socket, 128);
    if (ret == -1) {
        perror("listen fail!");
        close(this->m_socket);
        return false;
    }
    socklen_t sock_size = sizeof (struct sockaddr);
    getsockname(this->m_socket, (struct sockaddr*) & this->m_addr, &sock_size);
    this->m_port = htons_ex(m_addr.sin_port);
    //    printf("port[%d] ip[%x]\n", this->m_port, htons_ex(m_addr.sin_addr.s_addr));
    this->start();
    return true;
}

void TCPServer::run() {
    while (this->isrunning) {
        CheckClients();
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(m_socket, &read_fds);
        int ret = select(m_socket + 1, &read_fds, NULL, NULL, NULL);
        if (ret <= 0) {
            if (ret != 0) {
                perror("select fail!");
                return;
            } else {
                continue;
            }
        }
        struct sockaddr_in temp_addr;
        socklen_t sock_len = sizeof (sockaddr);
        ret = accept(m_socket, (struct sockaddr*) &temp_addr, &sock_len);
        if (ret == -1) {
            perror("accept fail!");
            close(this->m_socket);
            return;
        }
        if (this->m_bSingleIp) {
            CloseClient(&temp_addr);
        }
        TCPClientNode* clientNode = new TCPClientNode(ret, &temp_addr, this->m_handler);
        clientNode->start();
        this->m_lock.lock();
        this->m_clients.push_back(clientNode);
        this->m_lock.unlock();
    }
}

void TCPServer::ForEach(Action * actor) {
    this->m_lock.lock();
    list<TCPClientNode*>::iterator iter = this->m_clients.begin();
    for (; iter != this->m_clients.end(); iter++) {
        if (!((*iter)->is_dead())) {
            actor->exec((*iter)->GetHandler());
        }
    }
    this->m_lock.unlock();
}

/**
 @brief 停止服务
 */
void TCPServer::Stop() {
    this->isrunning = false;
    if (this->thread_) {
        this->thread_->join();
        delete this->thread_;
        this->thread_ = 0;
    }
    this->m_lock.lock();
    list<TCPClientNode*>::iterator iter = this->m_clients.begin();
    for (; iter != this->m_clients.end();) {
        //        printf("delete on client\n");
        delete *iter;
        this->m_clients.erase(iter++);
    }
    this->m_lock.unlock();
}

/**
 * @brief 设置只有一个服务器连接
 */
void TCPServer::SetOption(TCPServerOption option, bool bFlag) {
    if (bFlag) {
        this->m_bSingleIp = true;
    }
}

void TCPServer::SetOption(const char *optname, int ival) {
    if (strcmp(optname, "nodelay") == 0 && ival) {
        this->m_bnodelay = true;
    }
}

void TCPServer::CheckClients() {//有个小bug
    this->m_lock.lock();
    list<TCPClientNode*>::iterator iter = this->m_clients.begin();
    for (; iter != this->m_clients.end();) {
        bool is_dead = (*iter)->CheckClient();
        if (is_dead) {
            delete *iter;
            this->m_clients.erase(iter++);
        } else {
            iter++;
        }
    }
    this->m_lock.unlock();
}
// 关闭已有的连接

void TCPServer::CloseClient(struct sockaddr_in* addr) {
    this->m_lock.lock();
    list<TCPClientNode*>::iterator iter = this->m_clients.begin();
    for (; iter != this->m_clients.end();) {
        struct sockaddr_in temp_addr;
        temp_addr.sin_addr.s_addr = 0;
        temp_addr.sin_port = 0;
        temp_addr.sin_family = 0;
        (*iter)->GetAddr(&temp_addr);
        if (temp_addr.sin_addr.s_addr == addr->sin_addr.s_addr && temp_addr.sin_family == addr->sin_family && temp_addr.sin_port == addr->sin_port) {
            delete *iter;
            this->m_clients.erase(iter++);
        } else {
            iter++;
        }
    }
    this->m_lock.unlock();
}
