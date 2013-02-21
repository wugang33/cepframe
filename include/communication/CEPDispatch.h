/* 
 * File:   DispatchInterceptor.h
 * Author: Administrator
 *
 * Created on 2012年3月20日, 下午9:06
 */

#ifndef DISPATCHINTERCEPTOR_H
#define	DISPATCHINTERCEPTOR_H
#include <vector>
#include <map>
#include <list>
#include "communication/SMatrixOL.h"
#include <assert.h>
#include "communication/baseutils/ZTPServer.h"
#include "communication/baseutils/ZTPClient.h"

#define CEP_MAX_COMMAND_LEN 256
#define  CEP_SLAVE_REQUEST 200
#define CEP_MASTER_COMMAND 201
#define CEP_CONNECTED 202
#define CEP_CONN_CLOSE 203
#define CEP_DATA_FLAG 1
#define CEP_MASTER_NODEID 0
#define DEFAULT_MASTERPORT 3389
class ZTPServer;
class ZTPClient;
class CEPConnHandler;
class ConnConfig;
class TiXmlElement;
typedef TiXmlElement xml_element;
namespace cep {
    class event;
};

/*
 * cepnode 用来表示一个Node开启的服务
 */
class CEPServerNode {
public:
    unsigned int m_nodeid;
    unsigned int m_ip;
    unsigned int m_port;
};

struct DispatchStat {
    unsigned long SendCountSucc;
    unsigned long SendCountFail;
    unsigned long SendDatalenSucc;
    unsigned long SendDatalenFail;
    unsigned long RecvDataCount;
    unsigned long RecvDataLen;
    unsigned long SendCommand2MasterSucc;
    unsigned long SendCommand2MasterFail;
    unsigned long DiscardCount;
    unsigned int LastDiscardPN;

    DispatchStat() {
        SendCountSucc = 0;
        SendCountFail = 0;
        SendDatalenSucc = 0;
        SendDatalenFail = 0;
        SendCommand2MasterSucc = 0;
        SendCommand2MasterFail = 0;
        LastDiscardPN = 0;
        RecvDataCount = 0;
        RecvDataCount = 0;
    }
};

/*
 * master 的命令过滤器  如果不是master节点 直接返回
 * must have a route_table 存储了所有的现有节点的连接情况
 */
class CEPDispatch {
public:
    typedef map<unsigned int, CEPServerNode*> ServerMap; /*一个节点最多只需要开一个服务就行了*/
    typedef multimap<unsigned int, CEPConnHandler*> ConnMap; /*一个节点可能有多条连接*/
    typedef SMatrixOL<unsigned int > SMatrix; /*用一个稀疏矩阵表示当前客户端的连接情况*/
public:
    CEPDispatch(unsigned int localid);

    static CEPDispatch & instance() {
        assert(m_dispatch != 0);
        return *m_dispatch;
    }
    void doInterceptor(const void *data, ZTP_DATA_INFO*info, ZTPHandler*ztphandler = 0);

    void dispatch(const cep::event &, unsigned int nodeid);

    void run();

    void addServer(ZTPServer*);

    void addClient(ZTPClient*);

    bool Start();

    void initConfig(unsigned int localid);

    void reload_metadata(const xml_element&);

    virtual ~CEPDispatch();

    void setMatrix(SMatrix *sm) {
        assert(m_smatrix == 0);
        this->m_smatrix = sm;
    }

    void setMasterZTPClient(ZTPClient*zptclient) {
        this->m_masterclient = zptclient;
    }
    bool sendCommand(CEPConnHandler*, unsigned char flag, unsigned type, const char *buf, unsigned int buflen);
private:
    bool requestConnection(unsigned int nodeid);
    void resetConfig();
    void doMasterCommand(const void*, ZTP_DATA_INFO*info, CEPConnHandler *cephandler = 0); //处理master发送过来的命令 命令字 201
    void doConnect(const void*, ZTP_DATA_INFO*info, CEPConnHandler *cephandler = 0); //处理master发送过来的命令 命令字 201
    void doClose(const void*, ZTP_DATA_INFO*info, CEPConnHandler *cephandler = 0); //处理master发送过来的命令 命令字 201
    void doSlaveRequest(const void*, ZTP_DATA_INFO*info, CEPConnHandler *cephandler = 0); //处理slave发送过来的请求   命令字 200
    CEPDispatch(const CEPDispatch& orig);

    void start() {
        if (this->thread_)return;
        this->isrunning = true;
        this->thread_ = new boost::thread(boost::bind(&CEPDispatch::run, this));
    }
private:

    DispatchStat dispatchstat_;
    boost::thread * thread_;
    bool isrunning;
    std::vector<ZTPServer*> m_server; //这个不能删除  删除的只能是connect
    std::vector<ZTPClient*> m_client; //
    unsigned int m_localid;
    ServerMap m_nodemap; //整个网络的服务开启情况
    ConnMap m_connmap; //本地连接情况
    boost::mutex m_connmaplock;
    boost::mutex m_nodemaplock;
    boost::mutex m_serverlock;
    boost::mutex m_clientlock;
    ZTPClient *m_masterclient;
    ZTPServer::Builder m_srvbuilder; /*配置信息 ZTPServer的配置*/
    ZTPClient::Builder m_clibuilder; /*配置信息 ZTPClient的配置*/
    CEPConnHandler * m_cephandler;
    /*
     * 基于十字链表的稀疏矩阵  由于这里只需要进行行于列的遍历  所以这样的结构非常适合
     *nodeid的行的元素 代表要通知这一行的非0元的元素来连接nodeid这个server
     *nodeid 列的元素 代表这个pn要连接那些服务器
     */
    boost::mutex m_smatrix_lock_;
    SMatrix* m_smatrix;
    ConnConfig *m_config;
    static CEPDispatch * m_dispatch;
};

#endif	/* DISPATCHINTERCEPTOR_H */

