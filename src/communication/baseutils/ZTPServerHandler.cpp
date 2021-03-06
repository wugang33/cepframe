#include "communication/baseutils/ZTPServerHandler.h"
#include "communication/baseutils/ZTPSendThread.h"
#include "communication/baseutils/Encrypt.h"

ZTPServerHandler::ZTPServerHandler(ZTPHandler * hndl, UINT localid, bool bEncrypt, bool bCompress,
        DWORD dwPktSize, BYTE icompresslev) : m_bEncrypt(bEncrypt), m_bCompress(bCompress), m_dwPktSize(dwPktSize), m_localid(localid) {
    this->m_iCompressLevel = icompresslev;
    m_hndl = hndl->Clone();
    memset(m_buf, 0, sizeof (this->m_buf));
    memset(m_hash, 0, sizeof (this->m_hash));
    memset(&m_info, 0, sizeof (ZTP_DATA_INFO));
    m_sendthread = 0;
    m_timestamp = time(0);
    this->m_bCompress = bCompress;
    int ret = m_lzo.Init(m_iCompressLevel);
    if (ret != 0) {
        printf("error ZTPServerHandler::ZTPServerHandler lzo init failure ret[%d]\n", ret);
    }
    m_lzobuf = new unsigned char[69755];
    m_lzobuflen = 69755;
    if (m_iCompressLevel != 1) {
        this->m_iCompressLevel = 9;
    }
    m_pConnStat = 0;
    this->m_rcvbuf.resize(0x10020, 0);
    m_pNetBufSetting = 0;
    m_remoteid = 0;
}

ZTPServerHandler::~ZTPServerHandler() {
    if (m_hndl) {
        delete m_hndl;
    }
    if (m_lzobuf) {
        delete m_lzobuf;
    }

}

/*
 @brief 克隆方法，创建一个新的ZTPService对象。
 */
TCPSocketHandler * ZTPServerHandler::Clone() {
    ZTPServerHandler* handl = new ZTPServerHandler(this->m_hndl, this->m_localid,\
            this->m_bEncrypt, this->m_bCompress,\
            this->m_dwPktSize, this->m_iCompressLevel);
    handl->SetNetBufSetting(this->m_pNetBufSetting);
    return handl;
}

void ZTPServerHandler::OnConnect(TCPSocket * conn, const char * host, int port) {
    this->m_pConnStat = new ZTPNetStat();
    //printf("ZTPServerHandler[%p]\n", this->m_pConnStat);
    conn->SetStat(m_pConnStat);
    char buffer[64] = {0};
    sprintf(buffer, "%d", conn->GetLocalAddr()->sin_port);
    this->m_pConnStat->Init(buffer, true, 0);
    this->m_sendthread = new ZTPSendThread(conn, this->m_hndl,\
                false, this->m_pNetBufSetting, \
                this->m_bEncrypt, this->m_bCompress, this->m_dwPktSize, \
                this->m_iCompressLevel);
    this->m_sendthread->start();
    m_pConnStat->Connect(conn->GetRemoteAddr(), conn->GetLocalAddr());
    m_hndl->OnConnect(this->m_sendthread, host, port);
}

void ZTPServerHandler::OnRead(TCPSocket * conn) {
    bool is_encrypt = false;
    int ret = conn->ReadAll(this->m_buf, ZTP_BCM_HEADER_SIZE);
    if (ret <= 0) {
        m_pConnStat->ConnClose(EN_INVALIDSOCKET);
        return;
    }
    ZTP_BCM_HEADER * bcm_head = (ZTP_BCM_HEADER*)this->m_buf;
    if (ntohs_ex((*(unsigned short*) bcm_head->magic)) != ZTP_MAGIC_NUM) {
        is_encrypt = true;
        Encrypt::Decode(this->m_buf, ZTP_BCM_HEADER_SIZE);
        if (ntohs_ex((*(unsigned short*) bcm_head->magic)) != ZTP_MAGIC_NUM) {
            conn->Close();
            m_pConnStat->ConnClose(EN_INVALIDPROTO);
            return;
        }
    }
    int package_len = ntohs_ex(bcm_head->package_len);
    int total_len = package_len + ZTP_BCM_HEADER_SIZE;
    ret = conn->ReadAll(this->m_buf + ZTP_BCM_HEADER_SIZE, package_len); //得到长度 开始读取了5个长度 第1，2\
                                                                           个字节魔数。第3,4个字节是\
                                                                           用来标识后面的字节有多长\
                                                                           第5个字节是用标识是否压缩？
    if (ret <= 0) {
        m_pConnStat->ConnClose(EN_INVALIDSOCKET);
        return;
    }
    this->m_pConnStat->Recv(total_len, 1);
    if (is_encrypt) {
        Encrypt::Decode(this->m_buf + ZTP_BCM_HEADER_SIZE, package_len);
    }
    if (total_len >= ZTP_HEADER_SIZE) {
        if (bcm_head->isCompress) {
            int out_len = this->m_lzobuflen;
            int decompress_ret = this->m_lzo.DeCompress(this->m_buf + ZTP_HEADER_SIZE, total_len - ZTP_HEADER_SIZE, this->m_lzobuf, out_len);
            if (decompress_ret != 0) {
                printf("decompress fault decompress ret is %d\n", decompress_ret);
                conn->Close();
                m_pConnStat->ConnClose(EN_INVALIDPROTO);
                return;
            }
            memcpy(this->m_buf + ZTP_HEADER_SIZE, this->m_lzobuf, out_len);
            bcm_head->package_len = ntohs_ex((unsigned short) (out_len + ZTP_HEADER_SIZE - ZTP_BCM_HEADER_SIZE));
            total_len = out_len + ZTP_HEADER_SIZE;
        }
    } else {
        printf("package length[%d] less the ZTP_HEADER_SIZE!\n", total_len);
        conn->Close();
        m_pConnStat->ConnClose(EN_INVALIDPROTO);
        return;
    }
    ZTP_HEADER *ztp_header = (ZTP_HEADER*)this->m_buf;
    unsigned short cmd = ntohs_ex(ztp_header->cmd.cmd); //cmd
    unsigned short cmd_status = ntohs_ex(ztp_header->cmd.cmd_status); //cmdstatus
    //    unsigned short var38 = htons_ex(*(unsigned short*) (m_buf + 0x09));//不知道是什么
    if (cmd == ZTP_CMD_SEND_RECORD) {
        this->doSendRecord(conn, this->m_buf, total_len, cmd_status);
        return;
    } else if (cmd == ZTP_CMD_BIND) {
        this->doBind(conn, this->m_buf, total_len);
    } else if (cmd == ZTP_CMD_CHECK_LINK) {
        this->doCheckLink(conn, this->m_buf, total_len);
    } else if (cmd == ZTP_CMD_UNBIND) {
        this->doUnbind(conn, this->m_buf, total_len);
    } else {
        conn->Close();
        m_pConnStat->ConnClose(EN_INVALIDPROTO);
    }
}

void ZTPServerHandler::OnClose() {
    this->m_hndl->OnClose();
    this->m_sendthreadlock.lock();
    if (this->m_sendthread) {
        this->m_sendthread->stop();
        delete this->m_sendthread;
        this->m_sendthread = 0;
    }
    this->m_sendthreadlock.unlock();
    if (this->m_pConnStat) {
        delete this->m_pConnStat;
        this->m_pConnStat = 0;
    }

}

void ZTPServerHandler::OnFail(const char * msg) {
    this->m_hndl->OnFail(msg);
}

int ZTPServerHandler::SendData(const void * data,
        ZTP_DATA_INFO * info, int tTimeOut) {
    int temp_timeout = tTimeOut;
    if (!m_sendthread) {
        return 0;
    }
    /*
     * find if the client is register the data type;
     */
    unsigned short ecx = info->type;
    unsigned int eax = 1;
    unsigned short edx = ecx;
    ecx = ecx & 0x1f;
    edx = edx >> 5;
    eax = eax << (ecx & 0xFF);
    if ((this->m_hash[edx] & eax) == 0) {
        return 2;
    }
    unsigned long eax_addr = info->data_len;
    if (eax_addr + 0x0a > 0x40000000) {
        this->m_pConnStat->Discard(info->data_len, 1);
        return 3;
    }
    //    cout << "test ZTPServerHandler senddata 4" << endl;
    this->m_sendthreadlock.lock();
    if (!m_sendthread) {
        this->m_timestamp = time(0);
        this->m_sendthreadlock.unlock();
        return 0;
    }
    do {
        int ret = this->m_sendthread->SendData(data, info);
        if (ret != 0) {
            this->m_timestamp = time(0);
            this->m_sendthreadlock.unlock();
            return 0;
        }
        this->m_sendthreadlock.unlock();
        if (temp_timeout <= 0) {
            //            cout << "test ZTPServerHandler senddata 6" << endl;
            this->m_pConnStat->Discard(info->data_len, 1);
            this->m_pConnStat->Congestion();
            return 1;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        this->m_sendthreadlock.lock();
        if (!m_sendthread) {
            this->m_timestamp = time(0);
            this->m_sendthreadlock.unlock();
            return 0;
        }
        temp_timeout -= 10;
    } while (temp_timeout > 0);
}

int ZTPServerHandler::SendData(const void * data,
        ZTP_DATA_INFO * info, UINT nodeid, int tTimeOut) {
    int temp_timeout = tTimeOut;
    if (!m_sendthread) {
        return 0;
    }
    if (nodeid != this->m_remoteid) {
        return 2;
    }
    unsigned long data_len = info->data_len;
    if (data_len + ZTP_RECORD_HEADER_SIZE > 0x40000000) {
        this->m_pConnStat->Discard(info->data_len, 1);
        return 3;
    }
    this->m_sendthreadlock.lock();
    if (!m_sendthread) {
        this->m_timestamp = time(0);
        this->m_sendthreadlock.unlock();
        return 0;
    }
    do {
        int ret = this->m_sendthread->SendData(data, info);
        if (ret != 0) {
            this->m_timestamp = time(0);
            this->m_sendthreadlock.unlock();
            return 0;
        }
        this->m_sendthreadlock.unlock();
        if (temp_timeout <= 0) {
            this->m_pConnStat->Discard(info->data_len, 1);
            this->m_pConnStat->Congestion();
            return 1;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        this->m_sendthreadlock.lock();
        if (!m_sendthread) {
            this->m_timestamp = time(0);
            this->m_sendthreadlock.unlock();
            return 0;
        }
        temp_timeout -= 10;
    } while (temp_timeout > 0);
}

void ZTPServerHandler::SendPacket(TCPSocket * conn, WORD cmd, WORD cmd_status, const void * buf, size_t len) {
    if (!m_sendthread) {
        return;
    }
    this->m_sendthreadlock.lock();
    if (!m_sendthread) {
        this->m_sendthreadlock.unlock();
        this->m_timestamp = time(0);
        return;
    }
    while (this->m_sendthread) {
        int ret = this->m_sendthread->SendPacket(cmd, cmd_status, buf, len);
        if (ret) {
            this->m_sendthreadlock.unlock();
            this->m_timestamp = time(0);
            return;
        }
        this->m_sendthreadlock.unlock();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        this->m_sendthreadlock.lock();
    }
    this->m_sendthreadlock.unlock();
    this->m_timestamp = time(0);


}

void ZTPServerHandler::OnIdle(TCPSocket * conn) {
    if (this->m_info.data_len != 0) {
        this->m_info.pkg_len = this->m_info.data_len;
        m_hndl->OnReceive((void *) this->m_rcvbuf.data(), &this->m_info);
        this->m_info.data_len = 0;
    }
    this->m_hndl->OnIdle();
    time_t time_temp = time(0);
    if (time_temp - this->m_timestamp > 0x78) {
        this->m_hndl->OnFail("Check link timeout.");
        this->m_pConnStat->ConnClose(EN_TIMEOUT);
        conn->Close();
    }
}

bool ZTPServerHandler::dump(vector<vector<char> > *arryInfo) {
    ZTPNetInfo * net_info = (ZTPNetInfo *) this->m_pConnStat->GetStatInfo();
    if (!net_info) {
        return false;
    }
    vector<char> vec;
    vec.resize(0x90, 0);
    memmove(vec.data(), net_info, 0x90);
    if (arryInfo)
        arryInfo->push_back(vec);
}

void ZTPServerHandler::doBind(TCPSocket * conn, const unsigned char * buf, int len) {
    unsigned char username[32] = {0};
    BIND_COMMAND_NEW * bind_command = (BIND_COMMAND_NEW *) (this->m_buf + ZTP_HEADER_SIZE);

    strncpy((char*) username, bind_command->username, sizeof (bind_command->username));
    unsigned short m_nodeid_high = bind_command->m_nodeid_high;
    unsigned char password[32] = {0};
    strncpy((char*) password, bind_command->password, sizeof (bind_command->password));
    unsigned short m_nodeid_low = bind_command->m_nodeid_low;
    m_remoteid = ntohs_ex(m_nodeid_high);
    m_remoteid <<= 8 * sizeof (unsigned short);
    m_remoteid |= ntohs_ex(m_nodeid_low);
    /*
     * len = 63=
     */
    unsigned int type_count = ntohs_ex(bind_command->request_types);
    if (len != (type_count * 2 + ZTP_HEADER_SIZE + sizeof (BIND_COMMAND_NEW))) {
        conn->Close();
        this->m_pConnStat->ConnClose(EN_INVALIDCHKSUM);
    }
    int ret = m_hndl->OnAuth((char *) username, (char *) password, (WORD*) & m_remoteid, type_count, this);
    if (ret == 0) {
        this->SendPacket(conn, ZTP_CMD_BIND_RESP, ZTP_CMD_STATUS_INVUSR, 0, 0);
        return;
    }
    for (int i = 0; i < type_count; i++) {
        unsigned short type = ntohs_ex(*(unsigned short*) (buf + ZTP_HEADER_SIZE + sizeof (BIND_COMMAND_NEW) + i * 2));
        if (type != 0) {
            unsigned int hash_idx = (type >> 5)&0x7ff;
            unsigned int ecx = type & 0x1f;
            unsigned int edx = 1 << ecx;
            this->m_hash[hash_idx] |= edx;
        } else {
            memset((unsigned char *) this->m_hash, 0xff, 0x2000);
        }
    }
    /*
     * 本来是没有的 为了实现功能  实现了一个反绑定的功能
     */
    {
        vector<char> vec;
        vec.resize(sizeof (BIND_COMMAND_NEW), 0);
        char * buf = (char*) vec.data();
        BIND_COMMAND_NEW * bind_command_reserv = (BIND_COMMAND_NEW *) (buf);
        unsigned short m_nodeid_high = this->m_localid >> 16;
        bind_command_reserv->m_nodeid_high = htons_ex(m_nodeid_high);
        unsigned short m_nodeid_low = m_localid & 0xffff;
        bind_command_reserv->m_nodeid_low = htons_ex(m_nodeid_low);
        bind_command_reserv->request_types = 0;
        this->SendPacket(conn, 1, 0, buf, vec.size());
    }
    this->SendPacket(conn, ZTP_CMD_BIND_RESP, ZTP_CMD_STATUS_SUCCESS, 0, 0);
    this->m_timestamp = time(0);
    return;
}

void ZTPServerHandler::doCheckLink(TCPSocket * conn, const unsigned char * buf, int len) {
    this->m_timestamp = time(0);
    this->SendPacket(conn, ZTP_CMD_CHECK_LINK_RESP, ZTP_CMD_STATUS_SUCCESS, 0, 0);

}

void ZTPServerHandler::doUnbind(TCPSocket * conn, const unsigned char * buf, int len) {
    this->SendPacket(conn, ZTP_CMD_UNBIND_RESP, ZTP_CMD_STATUS_SUCCESS, 0, 0);
    conn->Close();
    m_pConnStat->ConnClose(EN_SELFCLOSE);
}

void ZTPServerHandler::doSendRecord(TCPSocket * conn, const unsigned char * buf, int len, const WORD cmd_status) {
    this->m_timestamp = time(0);
    if (len <= (ZTP_HEADER_SIZE + ZTP_RECORD_HEADER_SIZE)) {
        return;
    }
    ZTP_HEADER *ztp_header = (ZTP_HEADER*) buf;
    ZTP_RECORD_HEADER * ztp_record_header = (ZTP_RECORD_HEADER*) ztp_header->ztp_data;
    int pure_data_len = len - ZTP_HEADER_SIZE - ZTP_RECORD_HEADER_SIZE;
    /*
     * 如果是分组消息的非最后一包 那么放到缓冲区里面缓存起来 等待最后合并     
     */
    if (ZTP_CMD_STATUS_PKGSEG == cmd_status) {
        if (this->m_rcvbuf.size()<(this->m_info.data_len + pure_data_len)) {
            this->m_rcvbuf.resize(this->m_info.data_len + pure_data_len, 0);
        }
        memmove(this->m_rcvbuf.data() + this->m_info.data_len, ztp_record_header->record_data, pure_data_len);
        this->m_info.data_len += pure_data_len;
        this->m_info.pkg_len = this->m_info.data_len;
        m_info.flag = ztp_record_header->flag;
        m_info.type = ntohs_ex(ztp_record_header->type);
        m_info.format = ztp_record_header->format;
        return;
    }
    int pure_data_buf_offset = 0;
    while (pure_data_buf_offset < (pure_data_len)) {
        ztp_record_header = (ZTP_RECORD_HEADER*) (ztp_header->ztp_data + pure_data_buf_offset);
        m_info.flag = ztp_record_header->flag;
        m_info.type = ntohs_ex(ztp_record_header->type);
        m_info.ver_info = ntohs_ex(ztp_record_header->ver_info);
        m_info.format = ztp_record_header->format;
        int this_package_len = ntohl_ex(ztp_record_header->data_len);
        if ((this_package_len + pure_data_buf_offset) > pure_data_len) {
            printf("error this_package_len[%d] + pure_data_buf_offset[%d] less the pure_data_len[%d]\n", this_package_len, pure_data_buf_offset, pure_data_len);
            return;
        }
        //        printf("flag[%d] type[%d] ver_info[%d] format[%d] len[%d]\n", m_info.flag, m_info.type, m_info.ver_info, m_info.format, esi);
        if (m_info.data_len == 0) {
            this->m_info.data_len = this_package_len;
            this->m_info.pkg_len = this_package_len;
        } else {
            /*
             * 是分组消息的最后一包
             */
            if (this->m_rcvbuf.size() < m_info.data_len + this_package_len) {
                this->m_rcvbuf.resize(m_info.data_len + this_package_len, 0);
            }
            memmove(this->m_rcvbuf.data() + this->m_info.data_len, ztp_record_header->record_data, this_package_len);
            m_info.data_len += this_package_len;
            m_info.pkg_len += this_package_len;
        }
        pure_data_buf_offset += (ZTP_RECORD_HEADER_SIZE);
        this->m_pConnStat->Recv(m_info.data_len, 1);
        if (this->m_hndl) {
            if (m_info.data_len > this_package_len) {
                m_hndl->OnReceive(this->m_rcvbuf.data(), &m_info);
            } else if (m_info.data_len == this_package_len) {
                m_hndl->OnReceive(ztp_record_header->record_data, &m_info);
            }
        }
        pure_data_buf_offset += m_info.pkg_len;
        this->m_info.data_len = 0;
        this->m_info.pkg_len = 0;
    }
}
