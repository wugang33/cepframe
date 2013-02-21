/* 
 * File:   CEPClientHandler.h
 * Author: Administrator
 *
 * Created on 2012年3月19日, 下午3:03
 */

#ifndef CEPCLIENTHANDLER_H
#define	CEPCLIENTHANDLER_H

#include "communication/baseutils/ZTPServer.h"
class CEPDispatch;

class CEPConnHandler : public ZTPHandler {
public:

    void OnFail(const char * msg);

    /**
     @brief 克隆出一个新的对象
     */
    ZTPHandler * Clone();
    /**
     @brief 连接事件
     @param service ZTPService对象，用于发送数据包
     @param 对端IP
     @param 对端端口号
     */
    void OnConnect(ZTPService * service, const char * ip, int port);

    /**
     @brief 检查注册消息
     
 检查注册是否成功。如果返回true，则注册成功。
 如果返回false，则发送注册失败消息
     */
    bool OnAuth(const char * user, const char * passwd, const WORD* node_id, int count, ZTPServerHandler* pServerHandler);

    /**
     @brief 关闭事件
     */
    void OnClose();

    /**
     @brief 接收数据
 @param data 数据地址
     @param info 数据描述信息，包括数据长度、数据指针等
 @note 客户端只接收通过 ZTPClient::AddDataType 注册的数据类型的数据。
     */
    void OnReceive(const void * data,
            ZTP_DATA_INFO * info);

    /** 
     @brief 一段时间内没有收到任何包，即收包超时
     */
    void OnIdle();

    CEPConnHandler(unsigned int localid);
    ~CEPConnHandler();

    unsigned int getRemoteId() {
        return m_remoteid;
    }

    unsigned int getLocalId() {
        return m_localid;
    }

    unsigned int getRemoteIP() {
        return this->m_remotip;
    }

    unsigned short getRemotePort() {
        return this->m_remoteport;
    }

    void setCEPDispatch(CEPDispatch *dispatch) {
        m_dispatch = dispatch;
    }

    ZTPService* getZTPService() {
        return m_ztpService;
    }
private:
    CEPConnHandler(const CEPConnHandler& orig);
    string m_ip; //远端的IP
    unsigned int m_remotip;
    unsigned int m_remoteport;
    unsigned int m_remoteid; //remote_nodeid and m_port
    unsigned int m_localid; //local_nodeid 0是master
    ZTPService*m_ztpService;
    CEPDispatch *m_dispatch;
};

#endif	/* CEPCLIENTHANDLER_H */

