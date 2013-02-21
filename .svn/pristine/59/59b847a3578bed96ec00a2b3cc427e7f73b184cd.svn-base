/* 
 * File:   cdrclient.cpp
 * Author: Administrator
 *
 * Created on 2012年7月15日, 下午12:22
 * 可以接受指定类型xdr 然后输出到标准输出流的工具
 * -i ip -p 端口 -c 命令字 -s 接受的数量 如果小于0 会一直接接受
 * cdrclient -i 127.0.0.1 -p 3389 -c 0x1404,0x1203,0x0030 -s 100
 * 说明 -s 小于0 那么会一直输出 如果-s >0 那么会输出指定个数
 */

#include <cstdlib>
#include <getopt.h>
#include <stdio.h>
#include <string>
#include "communication/baseutils/ZTPClient.h"
#include "utils_cstr2nbr.hpp"
#include <boost/algorithm/string/split.hpp>
#include <time.h>
#include <boost/algorithm/string.hpp>
#include <assert.h>
using namespace std;

static unsigned int str2nbr(const char *str, int defval) {
    unsigned int x = 0;
    if (str == NULL || *str == 0)return defval;
    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        std::stringstream ss;
        ss << std::hex << (str + 2);
        ss >> x;
        return x;
    } else {
        std::stringstream ss;
        ss << (str);
        ss >> x;
        return x;
    }
}

string milsecond2str(long milsecond, const char *format) {
    unsigned int mils = milsecond % 1000;
    long second = milsecond / 1000;
    struct tm ptm;
    ptm.tm_year = 0;
    ptm.tm_mon = 0;
    ptm.tm_mday = 0;
    ptm.tm_hour = 0;
    ptm.tm_min = 0;
    ptm.tm_sec = 0;
    localtime_r(&second, &ptm);
    char buffer[32] = {0};
    sprintf(buffer, format, ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, mils);
    return string(buffer);
}

class cdrclient_handler : public ZTPHandler {
public:

    cdrclient_handler(int count) {
        this->count_ = count;
    }

    void OnFail(const char * msg) {
    };

    /**
     @brief 克隆出一个新的对象
     */
    ZTPHandler * Clone() {
        return new cdrclient_handler(this->count_);
    }

    /**
     @brief 连接事件
     @param service ZTPService对象，用于发送数据包
     @param 对端IP
     @param 对端端口号
     */
    void OnConnect(ZTPService * service, const char * ip, int port) {
    }

    /**
     @brief 检查注册消息
     
 检查注册是否成功。如果返回true，则注册成功。
 如果返回false，则发送注册失败消息
     */
    bool OnAuth(const char * user, const char * passwd, const WORD* node_id, int count, ZTPServerHandler* pServerHandler) {
        return true;
    }

    /**
     @brief 关闭事件
     */
    void OnClose() {
    }

    /**
     @brief 接收数据
 @param data 数据地址
     @param info 数据描述信息，包括数据长度、数据指针等
 @note 客户端只接收通过 ZTPClient::AddDataType 注册的数据类型的数据。
     */
    void OnReceive(const void * data,
            ZTP_DATA_INFO * info) {
        char buf[8] = {0};
        *(unsigned int*) (buf) = (unsigned int) info->type;
        *(unsigned int*) (buf + 4) = (unsigned int) info->data_len;
        fwrite(buf, 1, sizeof (buf), stdout);
        fwrite(data, 1, info->data_len, stdout);
        fflush(stdout);
        if (count_ > 0) {
            count_--;
            if (count_ == 0) {
                exit(0);
            }
        }
    }

    /** 
     @brief 一段时间内没有收到任何包，即收包超时
     */
    void OnIdle() {
    }
public:
    int count_;
};

/*
 * 
 */
int main(int argc, char** argv) {
    int ch;
    string ip;
    int port;
    string cmd_str;
    int count = -1;
    string username = "cdrclient";
    string password = "cdrclient";
    while ((ch = getopt(argc, argv, "i:p:c:s:U:P:")) != -1) {
        switch (ch) {
            case 'i':
                //                printf("option i:'%s'\n", optarg);
                ip = string(optarg);
                break;
            case 'p':
                //                printf("option p:'%s'\n", optarg);
                port = str2nbr(optarg, 0);
                break;
            case 'c':
                //                printf("option c:'%s'\n", optarg);
                cmd_str = string(optarg);
                break;
            case 's':
                //                printf("option s:'%s'\n", optarg);
                count = str2nbr(optarg, -1);
                break;
            case 'U':
                username = string(optarg);
                break;
            case 'P':
                password = string(optarg);
                break;
            default:
                break;
                //                printf("other option:%c\n", ch);
        }
    }
    if (ip.compare("") == 0 || port == 0 || cmd_str.compare("") == 0) {
        printf("usage %s  -i 127.0.0.1 -p 3389 -c 0x1404,0x1203,0x0030 -U username -P password -s 100\n", argv[0]);
        return 0;
    }
    if (username == "") {
        username = "cdrclient";
    }
    vector<string> vec;
    ZTPClient::Builder build;
    cdrclient_handler *handler = new cdrclient_handler(count);
    ZTPClient *client = new ZTPClient(handler, build);
    boost::algorithm::split(vec, cmd_str, boost::is_any_of(","));
    client->Login(username.c_str(), password.c_str(), 0);
    fprintf(stderr, "ip[%s] port[%d] count[%d] username[%s] password[%s] request type[", ip.c_str(), port, count, username.c_str(), password.c_str());
    for (int i = 0; i < vec.size(); i++) {
        client->AddDataType(str2nbr(vec[i].c_str(), 0));
        fprintf(stderr, " 0x%x ", str2nbr(vec[i].c_str(), 0));
        //        cout << "add type[" << str2nbr(vec[i].c_str(), 0) << "]" << endl;
    }
    fprintf(stderr, " ]\n");
    client->Start(ip.c_str(), port);
    ZTPNetInfo *info = (ZTPNetInfo*) client->GetStat();
    assert(info != 0);
    unsigned long oldflowrecvB = 0;
    unsigned long oldrecvcount = 0;
    while (true) {
        time_t t = time(0);
        string time_str = milsecond2str(t * 1000, "%4d-%02d-%02d %02d:%02d:%02d");
        unsigned long recvbyte = info->flow.SucRcvB;
        unsigned long recvpkg = info->flow.RecvPkt;
        double recvrate = ((double) (recvbyte - oldflowrecvB)) / (1024 * 1024);
        double totalrecvbyte = ((double) recvbyte) / (1024 * 1024);
        fprintf(stderr, "%s\ttotal recv[%.2fMB]\trate[%.2fMB/s]\ttotal recvpkg[%lu]\tincreasepkg[%lu]\n",
                time_str.c_str(),
                totalrecvbyte,
                recvrate,
                recvpkg,
                recvpkg - oldrecvcount);
        oldrecvcount = recvpkg;
        oldflowrecvB = recvbyte;
        sleep(1);
    }
    return 0;
}
