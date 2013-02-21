#include <netinet/in.h>
#include "communication/baseutils/ZTPSendThread.h"
#include "communication/baseutils/Encrypt.h"
#include <netinet/in.h>
#include "boost/filesystem.hpp"
UINT ZTPNetStat::s_counter = 0; //100h

ZTPNetStat::~ZTPNetStat() {//i add it
    const char *filename = this->GetFile();
    if (filename) {
        try {
            boost::filesystem::path p(filename);
            if (boost::filesystem::exists(p)) {
                boost::filesystem::remove(p);
            }
        } catch (std::exception &e) {
            cout << "ZTPNetStat ~ZTPNetStat remove file[" << filename << "] error due to " << e.what() << endl;
        }
    }
}

bool ZTPNetStat::Init(const char *pFile, bool bPostStat, int infosize) {
    //    string exepath = cep::os::getexecpath();
    string exepath = boost::filesystem::current_path().string();
    string temp = "/proc/net/";
    exepath += temp;
    try {
        boost::filesystem::path p(exepath);
        if (!boost::filesystem::exists(p)) {
            boost::filesystem::create_directories(p);
        }
    } catch (std::exception &e) {
        cout << "ZTPNetStat init create path[" << exepath << "] error due to " << e.what() << endl;
    }

    char buffer[0x100] = {0};
    snprintf(buffer, 0x100, "netstat_%d_%ld_%d_", getpid(), pthread_self(), ZTPNetStat::s_counter++);
    exepath.append(buffer);
    if (pFile) {
        exepath.append(pFile);
    }
    this->SetFile(exepath.c_str());
    bool ret = this->ReSize(0x90);
    if (ret) {
        void * get_tmp = this->Get();
        memset(get_tmp, 0, 0x90);
        m_ZTPNetInfo = (ZTPNetInfo *) get_tmp;
    } else {
        return false;
    }
    if (bPostStat == false) {
        try {
            boost::filesystem::path p(exepath);
            if (boost::filesystem::exists(p)) {
                boost::filesystem::remove(p);
            }
        } catch (std::exception &e) {
            cout << "ZTPNetStat init remove path[" << exepath << "] error due to " << e.what() << endl;
        }
    }
    return true;
}

/*                               rsi             rdx                  rcx                   r8                         r9        */
ZTPSendThread::ZTPSendThread(TCPSocket * conn, ZTPHandler * hndl, bool is_client, NetBufSetting * _pNetBufSetting,
        //栈        arg0                         arg8             arg 10          arg18                                                   
        bool ZTPSendThreadbSendEncrypt, bool bSendCompress, int iPkgSize, BYTE icompresslev) {
    this->m_socket = conn;
    m_pConnStat = 0;

    NetBufSetting netbufsetting;
    if (_pNetBufSetting) {
        netbufsetting = *_pNetBufSetting;
    }
    if (netbufsetting.wMaxBufCount == 0) {
        netbufsetting.wMaxBufCount = 2;
    }
    this->m_pIoBuf = new NetIOBuf(netbufsetting); //NetIOBuf size is 118h
    if (!m_socket) {
        m_pConnStat = new ZTPNetStat();
        m_pConnStat->Init(0, true, 0);
        m_pIoBuf->Init((NetBufInfo*)this->m_pConnStat->GetStatInfo());
        m_pConnStat->SetBufStat(m_pIoBuf->GetNetBufStat());
    } else {
        this->m_pConnStat = (ZTPNetStat *) m_socket->GetStat();
        if (m_socket->GetStat()) {
            m_pIoBuf->Init((NetBufInfo*)this->m_pConnStat->GetStatInfo());
            m_pConnStat->SetBufStat(m_pIoBuf->GetNetBufStat());
        }
    }
    this->m_pkgsize = iPkgSize;
    m_pkgBuf = new unsigned char[iPkgSize];
    m_is_client = is_client;
    ZTP_HEADER*ztp_header = (ZTP_HEADER*) m_pkgBuf;
    *(unsigned short *) ztp_header->bcm_head.magic = htons_ex(ZTP_MAGIC_NUM);
    ztp_header->bcm_head.package_len = 0;
    ztp_header->bcm_head.isCompress = 0;
    ztp_header->cmd.cmd = htons_ex(ZTP_CMD_SEND_RECORD);
    ztp_header->cmd.cmd_status = 0;
    ztp_header->cmd.reserved = 0;
    m_pkgbuflen = ZTP_HEADER_SIZE;
    m_lzobuf = 0;
    m_bSendCompress = bSendCompress;
    m_bSendEncrypt = ZTPSendThreadbSendEncrypt;

    int ret = m_lzo.Init(icompresslev);
    m_lzobuflen = 0x11499;
    m_lzobuf = new unsigned char[this->m_lzobuflen];
    if (ret != 0) {
        this->m_bSendCompress = 0;
        printf("ZTPSendThread::ZTPSendThread lzo init failure ret[%d]\n", ret);
    }
    this->isrunning = false;
    this->thread_ = 0;
}

ZTPSendThread::~ZTPSendThread() {
    this->isrunning = false;
    if (this->thread_) {
        this->thread_->join();
        delete this->thread_;
        this->thread_ = 0;
    }
    if (this->m_pkgBuf) {
        delete this->m_pkgBuf;
    }
    if (this->m_lzobuf) {
        delete this->m_lzobuf;
    }
    this->m_lock.lock();
    if (m_pIoBuf) {
        delete m_pIoBuf;
    }
    m_pIoBuf = 0;
    this->m_lock.unlock();
}

bool ZTPSendThread::SendData(const void * buf, ZTP_DATA_INFO * info) {
    return this->addToPkg(buf, info);
}

/*                                         rsi               rdx               rcx              r8          */
bool ZTPSendThread::SendData(const void * buf_head, UINT head_len, const void * buf, ZTP_DATA_INFO * info) {
    if ((info->data_len + ZTP_RECORD_HEADER_SIZE + head_len) >this->m_pkgsize) {
        return false;
    }
    this->m_lock.lock();
    if ((info->data_len + ZTP_RECORD_HEADER_SIZE + head_len + m_pkgbuflen) > this->m_pkgsize) {

        bool ret = this->addPkgToSendBuf();
        if (!ret) {
            this->m_lock.unlock();
            return false;
        }
    }

    ZTP_RECORD_HEADER *record_header = (ZTP_RECORD_HEADER*) (m_pkgBuf + m_pkgbuflen);
    record_header->flag = info->flag;
    record_header->type = htons_ex((unsigned short) info->type);
    record_header->ver_info = htons_ex((unsigned short) info->ver_info);
    record_header->format = (unsigned char) info->format;
    record_header->data_len = ntohl_ex((unsigned int) (head_len + info->data_len));
    memcpy(record_header->record_data, buf_head, head_len);
    memcpy(record_header->record_data + head_len, buf, info->data_len);
    this->m_pkgbuflen += (info->data_len + ZTP_RECORD_HEADER_SIZE + head_len);
    this->m_lock.unlock();
    return true;
}

/*
 * ZTP的缓存结构如下
 * 一块大的缓冲区包含了很多package
 * struct cache_unit
 * {
 * unsigned short package_len;
 * char package_data[package_len]
 * };
 * cache_unit
 * cache_unit
 * cache_unit
 */
bool ZTPSendThread::SendPacket(WORD cmd, WORD cmd_status, const void * buf, size_t len) {

    this->m_lock.lock();
    unsigned char* send_buf = 0;
    int send_buf_offset = 0;
    MMapBuf *mmap_buf = this->m_pIoBuf->GetCrntInBufAndTest(send_buf, send_buf_offset, 2 + len + ZTP_HEADER_SIZE);
    if (!send_buf) {
        this->m_lock.unlock();
        return false;
    }
    unsigned short package_len = htons_ex(len + ZTP_HEADER_SIZE);
    *(unsigned short*) (send_buf + send_buf_offset) = package_len;
    ZTP_HEADER *ztp_header = (ZTP_HEADER*) (send_buf + send_buf_offset + 2);
    *(unsigned short*) ztp_header->bcm_head.magic = htons_ex(ZTP_MAGIC_NUM);
    /*
     * bcm_head_.package_len 表示了ZTP_BCM_HEADER后面的有多长 这就包含了ZTP_BCP_COMMAND command command_status的状态
     */
    ztp_header->bcm_head.package_len = htons_ex(len + ZTP_CMD_SIZE);
    ztp_header->bcm_head.isCompress = 0;
    ztp_header->cmd.cmd = htons_ex(cmd);
    ztp_header->cmd.cmd_status = htons_ex(cmd_status);
    ztp_header->cmd.reserved = 0;
    memcpy(ztp_header->ztp_data, buf, len);
    mmap_buf->SetLen(send_buf_offset + 2 + len + ZTP_HEADER_SIZE);
    this->m_lock.unlock();
    return true;
}

void ZTPSendThread::Close() {
    this->m_socket->Close();
}

bool ZTPSendThread::FileCacheEnable() {
    if (m_pConnStat->ConnTimes()) {
        return this->m_pIoBuf->EnFileCache();
    }
    return 0;

}

ZTPNetInfo * ZTPSendThread::GetNetInfo() {
    return (ZTPNetInfo*)this->m_pConnStat->GetStatInfo();
}

void ZTPSendThread::run() {
    while (this->isrunning) {
        this->doSend();
    }
}

void ZTPSendThread::doSend() {
    if (!this->m_pConnStat->IsConn()) {
        if (this->m_pIoBuf->IdleListEmpty()) {
            if (!this->m_pConnStat->ConnTimes()) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(10));
                return;
            } else {
                this->m_lock.lock();
                this->m_pIoBuf->SetAllBufInFile();
                this->m_lock.unlock();
                boost::this_thread::sleep(boost::posix_time::milliseconds(10));
                return;
            }
        } else {
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            return;
        }
    }
    this->m_lock.lock();
    this->m_pIoBuf->FreeBuf();
    if (m_pIoBuf->Empty()) {
        this->addPkgToSendBuf();
        this->m_lock.unlock();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        return;
    }
    if (!this->m_pConnStat->IsConn()) {
        this->m_lock.unlock();
        return;
    }
    MMapBuf * buf = this->m_pIoBuf->GetOutBuf();
    this->m_lock.unlock();
    if (!buf) {
        return;
    }
    unsigned char * out_buf = (unsigned char*) buf->Get();
    int out_buf_len = buf->GetLen(); //r14d
    if (out_buf_len <= 0) {
        return;
    }
    int out_buf_offset = 0;
    int package_len = 0;
    do {
        package_len = ntohs_ex(*(unsigned short*) (out_buf + out_buf_offset)); //得到package的长度
        unsigned char * send_buf = out_buf;
        int send_buf_len = package_len;
        out_buf_offset += 2;
        if (!this->isrunning) {
            return;
        }
        if (out_buf_len < package_len) {
            return;
        }
        if (m_bSendCompress) {
            if (send_buf_len > ZTP_HEADER_SIZE) {
                memcpy(m_lzobuf, send_buf + out_buf_offset, ZTP_HEADER_SIZE);
                int compressed_out_len = m_lzobuflen;
                int ret = this->m_lzo.Compress(send_buf + out_buf_offset + ZTP_HEADER_SIZE, send_buf_len - ZTP_HEADER_SIZE, m_lzobuf + ZTP_HEADER_SIZE, compressed_out_len);
                if (ret != 0) {
                    printf("error ZTPSendThread::doSend compress ret[%d]\n", ret);
                    return;
                }
                ZTP_HEADER * ztp_header = (ZTP_HEADER*) (m_lzobuf);
                ztp_header->bcm_head.package_len = htons_ex((unsigned short) (compressed_out_len + ZTP_CMD_SIZE));
                ztp_header->bcm_head.isCompress = 1;
                send_buf = m_lzobuf;
                send_buf_len = compressed_out_len + ZTP_HEADER_SIZE;
            } else {
                send_buf += out_buf_offset;
            }
        } else {
            send_buf += out_buf_offset;
        }
        if (this->m_bSendEncrypt) {
            Encrypt::Encode(send_buf, ZTP_BCM_HEADER_SIZE);
            Encrypt::Encode(send_buf + ZTP_BCM_HEADER_SIZE, send_buf_len - ZTP_BCM_HEADER_SIZE);
        }
        int ret = m_socket->WriteAll(send_buf, send_buf_len);
        if (ret <= 0) {
            this->m_pConnStat->Discard(package_len, 1);
            this->m_pConnStat->ConnClose(EN_INVALIDSOCKET);
        } else {
            this->m_pConnStat->Send(package_len, 1);
        }
        out_buf_offset += package_len;
    } while (out_buf_len > out_buf_offset);
}

bool ZTPSendThread::addToPkg(const void * buf, ZTP_DATA_INFO * info) {
    this->m_lock.lock();
    if (info->data_len + ZTP_RECORD_HEADER_SIZE + m_pkgbuflen > m_pkgsize) {
        int ret = this->addPkgToSendBuf();
        if (!ret) {
            this->m_lock.unlock();
            return false;
        }
        /*
         * 如果还是小了  那说明这个数据的最长比pkg缓冲区的长度还要长
         */
        if (info->data_len + ZTP_RECORD_HEADER_SIZE + m_pkgbuflen > m_pkgsize) {
            ret = this->addSuperLenPkgToSendBuf(buf, info);
            this->m_lock.unlock();
            return ret;
        }
    }
    ZTP_RECORD_HEADER *info_net = (ZTP_RECORD_HEADER*) (m_pkgBuf + m_pkgbuflen);
    info_net->flag = info->flag;
    info_net->type = htons_ex(info->type);
    info_net->format = info->format;
    info_net->ver_info = htons_ex(info->ver_info);
    info_net->data_len = htonl_ex(info->data_len);
    memcpy(info_net->record_data, buf, info->data_len);
    m_pkgbuflen += (info->data_len + ZTP_RECORD_HEADER_SIZE);
    this->m_lock.unlock();

    return true;
} // 将小包加入到大包

bool ZTPSendThread::addPkgToSendBuf() {
    if (m_pkgbuflen <= ZTP_HEADER_SIZE) {
        m_pkgbuflen = ZTP_HEADER_SIZE;
        return true;
    }
    unsigned char * pCache = 0; //var28
    int iCrntPos = 0; //var20
    /*
     * 获取这么长度的缓冲块
     * m_pkgbuflen+2;
     * m_pkgbuflen 当前已经使用的缓冲块长度 2是为了存储缓冲区长度
     */
    MMapBuf * mapbuf = m_pIoBuf->GetCrntInBufAndTest(pCache, iCrntPos, m_pkgbuflen + 2);
    if (!pCache) {
        return false;
    }
    ZTP_HEADER * zpt_header = (ZTP_HEADER*) (m_pkgBuf);
    zpt_header->bcm_head.package_len = htons_ex((unsigned short) (this->m_pkgbuflen - ZTP_BCM_HEADER_SIZE));
    //    *(unsigned short*) (m_pkgBuf + 2) = htons_ex((unsigned short) this->m_pkgbuflen - 5);
    /*
     * 得到的空闲块的1和2个字节填充的是package的长度
     */
    *(unsigned short*) (pCache + iCrntPos) = htons_ex((unsigned short) this->m_pkgbuflen);
    memcpy(pCache + iCrntPos + 2, m_pkgBuf, this->m_pkgbuflen);
    iCrntPos = m_pkgbuflen + 2 + iCrntPos;
    mapbuf->SetLen(iCrntPos);
    m_pkgbuflen = ZTP_HEADER_SIZE;
    return true;
} // 将大包加入到发送缓冲池

/*
 * 超大的包  以至于package都放不下
 * 处理的方式就是把这个大包分解成很多的小package然后发送出去
 */
bool ZTPSendThread::addSuperLenPkgToSendBuf(const void * buf, ZTP_DATA_INFO * info) {
    if (!addPkgToSendBuf()) {
        return false;
    }
    unsigned char * pCache = 0;
    int iCrntPos = 0;
    /*
     * packages是指把这个数据分解成多少的小package发送出去
     */
    int packages = info->data_len / (m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE);
    int last_package_len = info->data_len % (m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE);
    if (last_package_len) {
        packages++;
    }
    /*
     * 试图获取一块能够储存全部package的cache块
     */
    m_pIoBuf->GetCrntInBufAndTest(pCache, iCrntPos, packages * m_pkgsize);
    if (!pCache) {
        return false;
    }
    unsigned short command_status_pkgseq = htons_ex(ZTP_CMD_STATUS_PKGSEG);
    ZTP_HEADER*ztp_header = (ZTP_HEADER*) (m_pkgBuf);
    ZTP_RECORD_HEADER *record_header = (ZTP_RECORD_HEADER*) (m_pkgBuf + ZTP_HEADER_SIZE);
    int buf_offset = 0;
    while (packages--) {
        if (packages > 1) {
            ztp_header->cmd.cmd_status = command_status_pkgseq;
            record_header->flag = info->flag;
            record_header->type = htons_ex(info->type);
            record_header->format = info->format;
            record_header->ver_info = htons_ex(info->ver_info);
            record_header->data_len = ntohl_ex((unsigned int) (m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE));
            memcpy(record_header->record_data, ((char *) buf + buf_offset), this->m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE);
            buf_offset += (this->m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE);
            m_pkgbuflen = this->m_pkgsize;
            this->addPkgToSendBuf();
        } else {
            /*最后一个包*/
            record_header->flag = info->flag;
            record_header->type = htons_ex(info->type);
            record_header->format = info->format;
            record_header->ver_info = htons_ex(info->ver_info);
            record_header->data_len = ntohl_ex((unsigned int) (last_package_len));
            memcpy(record_header->record_data, ((char *) buf + buf_offset), last_package_len);
            //            buf_offset += (this->m_pkgsize - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE);
            m_pkgbuflen = m_pkgbuflen + ZTP_RECORD_HEADER_SIZE + last_package_len;
            this->addPkgToSendBuf();
        }
    }
    return true;
}
