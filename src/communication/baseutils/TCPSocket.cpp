/* 
 * File:   TCPSocket.cpp
 * Author: WG
 * 
 * Created on 2012年2月13日, 上午9:08
 */

#include "communication/baseutils/TCPSocket.h"
#ifndef _AIX
#include <sys/sendfile.h> /* sendfile */
#endif

TCPSocket::TCPSocket() {
    m_socket = -1;
    m_rflag = 0;
    m_wflag = 0;
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = 0;
    m_addr.sin_addr.s_addr = 0;
    m_remote.sin_family = AF_INET;
    m_remote.sin_port = 0;
    m_remote.sin_addr.s_addr = 0;
    m_local.sin_family = AF_INET;
    m_local.sin_port = 0;
    m_local.sin_addr.s_addr = 0;
    m_RcvBuf = 0x20000;
    m_SndBuf = 0x20000;
    m_bnodelay = 0;
    m_pConnStat = 0;
}

TCPSocket::~TCPSocket() {
    close(this->m_socket);
}

/** 
    @brief 创建TCPSocket对象
 */

/** 
 @brief 创建TCPSocket对象,绑定IP
 */
TCPSocket::TCPSocket(const char *ip) {
    inet_aton(ip, &m_addr.sin_addr);
    m_addr.sin_port = 0;
    m_addr.sin_family = AF_INET;
    m_socket = -1;
    m_rflag = 0;
    m_wflag = 0;
    m_remote.sin_family = AF_INET;
    m_remote.sin_port = 0;
    m_remote.sin_addr.s_addr = 0;
    m_local.sin_family = AF_INET;
    m_local.sin_port = 0;
    m_local.sin_addr.s_addr = 0;
    m_RcvBuf = 0x20000;
    m_SndBuf = 0x20000;
    m_bnodelay = 0;
    m_pConnStat = 0;

}

/** 
 @brief 创建TCPSocket对象,绑定IP和端口
 */
TCPSocket::TCPSocket(const char *ip, int port) {
    m_socket = -1;
    m_rflag = 0;
    m_wflag = 0;
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons_ex(port);
    inet_aton(ip, &m_addr.sin_addr);
    m_addr.sin_port = 0;
    m_addr.sin_family = AF_INET;
    m_remote.sin_family = AF_INET;
    m_remote.sin_port = 0;
    m_remote.sin_addr.s_addr = 0;
    m_local.sin_family = AF_INET;
    m_local.sin_port = 0;
    m_local.sin_addr.s_addr = 0;
    m_RcvBuf = 0x20000;
    m_SndBuf = 0x20000;
    m_bnodelay = 0;
    m_pConnStat = 0;
}

/**
 @brief 创建TCPSocket对象，并绑定socket句柄和地址

 该构造函数用于封装已有的socket句柄，主要用于服务端接收连接时创建新的TCPSocket对象。
 */
TCPSocket::TCPSocket(int sock, sockaddr_in *addr) {
    m_socket = sock;
    //    m_rflag = 0;
    //    m_wflag = 0;
    //    m_addr.sin_family = AF_INET;
    //    m_addr.sin_port = 0;
    //    m_addr.sin_addr = 0;
    //    m_remote.sin_family = addr->sin_family;
    //    m_remote.sin_port = addr->sin_port;
    //    m_remote.sin_addr = addr->sin_addr;
    //    m_local.sin_family = AF_INET;
    //    m_local.sin_port = 0;
    //    m_local.sin_addr = 0;
    //    m_RcvBuf = 0x20000;
    //    m_SndBuf = 0x20000;
    //    m_bnodelay = 0;
    //    m_pConnStat = 0;
}

/**
 @brief 连接客户端
 * rdi rsi  rdx  (rdi,rsi,rdx)
 */
bool TCPSocket::Connect(const char * host, int port) {
    this->Close();
    if (this->m_socket < 0) {
        this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
        int flag = fcntl(this->m_socket, F_GETFD, 0);
        fcntl(this->m_socket, F_SETFD, flag | O_WRONLY);
        if (m_socket < 0) {
            return false;
        }
        int set_val = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char *) &set_val, sizeof (int));
        if (this->m_bnodelay) {
            char set_val = 1;
            setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char *) &set_val, sizeof (unsigned char));
        }
        if (this->m_addr.sin_addr.s_addr != 0) {
            if (this->m_addr.sin_addr.s_addr != 0x7f000001)//127.0.0.1
            {
                if (this->m_addr.sin_addr.s_addr != 1) {
                    char set_val = 1;
                    int ret = setsockopt(m_socket, SOL_SOCKET, SO_ERROR, (const char *) &set_val, sizeof (unsigned char));
                    if (ret == -1) {
                        close(this->m_socket);
                        return false;
                    }
                }
            }
        }//end if(this->m_addr.sin_addr!=0)
        setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char *) & this->m_RcvBuf, sizeof (unsigned int));
        setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char *) & this->m_SndBuf, sizeof (unsigned int));
        if (this->m_addr.sin_addr.s_addr != 0) {
            if (this->m_addr.sin_addr.s_addr != 0x7f000001)//127.0.0.1
            {
                if (this->m_addr.sin_addr.s_addr != 1) {
                    int ret = bind(this->m_socket, (struct sockaddr*) & this->m_addr, sizeof (sockaddr));
                    if (ret != 0) {
                        close(this->m_socket);
                        return false;
                    }
                }
            }
        }//  end if (this->m_addr.sin_addr != 0) 
        this->m_remote.sin_addr.s_addr = inet_addr(host);
        this->m_remote.sin_port = htons_ex(port);
        int ret = connect(this->m_socket, (const struct sockaddr*) & this->m_remote, sizeof (struct sockaddr));
        if (ret != 0) {
            return false;
        }
        socklen_t sock_len = sizeof (struct sockaddr);
        getsockname(this->m_socket, (struct sockaddr*) & this->m_local, &sock_len);
        //    printf("port[%d]\n", htons_ex(this->m_local.sin_port));
        return true;
    }
    return false;
}

/**
 @brief 对socket执行select操作
 @param timeout 超时（毫秒）
 @param mode 0:读操作，1:写操作
 */
int TCPSocket::Select(int timeout, int mode) {
    if (this->m_socket < 0) {
        return -1;
    }
    fd_set read_fds, write_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    if (mode == 0) { //read
        FD_SET(m_socket, &read_fds);
        return select(m_socket + 1, &read_fds, NULL, NULL, &tv);
    }
    FD_SET(m_socket, &write_fds);
    return select(m_socket + 1, NULL, &write_fds, NULL, &tv);
}

/**
 @brief 取得最后一次对socket进行读操作的返回值
 */
int TCPSocket::GetReadFlag() {
    return this->m_rflag;
}

/**
 @brief 取得最后一次对socket进行写操作的返回值
 */
int TCPSocket::GetWriteFlag() {
    return this->m_wflag;
}

/**
 @brief 接收数据
 */
int TCPSocket::Read(void * data, size_t len) {
    if (this->m_socket < 0)return -1;
    this->m_rflag = recv(this->m_socket, data, len, 0);
    return this->m_rflag;
}

/**
 @brief 读取数据填满缓冲区，直到发生错误为止
 */
int TCPSocket::ReadAll(void * data, size_t len) {
    int readed_nr = 0;
    while (readed_nr < len) {
        int r_nr = Read((void*) ((char*) data + readed_nr), len - readed_nr);
        if (r_nr > 0) {
            readed_nr += r_nr;
        } else if (r_nr == 0) {
            break; /*EOF*/
        } else {
            /*if errno is EINTR you can read again*/
            if (this->m_socket < 0) {
                return -1;
            } else if (errno != EINTR) {
                perror("socket error on read:");
                return -1;
            }
        }
    }
    return readed_nr;

}

/**
 @brief 写数据
 */
int TCPSocket::Write(const void * data, size_t len) {
    if (this->m_socket < 0)return -1;
    this->m_wflag = send(this->m_socket/*edi*/, data, len/**/, 0/*ecx*/);
    return this->m_wflag;
}

/**
 @brief 写所有数据，直到发生错误为止
 */
int TCPSocket::WriteAll(const void * data, size_t len) {
    if (this->m_socket < 0)return -1;
    int writed_nr = 0;
    while (writed_nr < len) {
        int wr_nr = Write((void *) ((char*) data + writed_nr), len - writed_nr);
        if (wr_nr > 0) {
            writed_nr += wr_nr;
        } else {
            if (wr_nr < 0 && errno == EINTR) {
                continue;
            } else {
                if (this->m_socket < 0) {
                    return -1;
                } else {
                    perror("socket error on write:");
                    return -1;
                }
            }
        }
    }
    return writed_nr;
}

/**
 @brief zerocopy
 */
int TCPSocket::Tranferto(int in_fd, off_t *offset, size_t count) {
#ifndef _AIX
    if (this->m_socket < 0)return -1;
    this->m_wflag = sendfile(m_socket, in_fd, offset, count);
    return this->m_wflag;
#endif

}

/**
 @brief 关闭连接
 */
void TCPSocket::Close() {
    if (this->m_socket > 0) {
        shutdown(this->m_socket, SHUT_RD);
        close(this->m_socket);
        this->m_socket = -1;
    }
}

void TCPSocket::SetOption(const char *optname, int ival) {
    if (strcmp(optname, "nodelay") == 0) {
        if (ival > 0) {
            this->m_bnodelay = true;
        }
    }
}

bool TCPSocket::IsValidSocket() {
    return this->m_socket;
}
