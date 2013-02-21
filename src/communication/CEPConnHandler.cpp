/* 
 * File:   CEPClientHandler.cpp
 * Author: Administrator
 * 
 * Created on 2012年3月19日, 下午3:03
 */

#include "communication/CEPConnHandler.h"
#include "utils_cstr2nbr.hpp"
#include "utils_nbr2str.hpp"
#include "communication/CEPDispatch.h"

CEPConnHandler::CEPConnHandler(unsigned int nodeid) : m_remotip(0), m_remoteport(0), m_remoteid(0), m_localid(nodeid), m_ztpService(0) {
}

CEPConnHandler::CEPConnHandler(const CEPConnHandler& orig) {
}

CEPConnHandler::~CEPConnHandler() {
}

void CEPConnHandler::OnReceive(const void * data,
        ZTP_DATA_INFO * info) {
    this->m_dispatch->doInterceptor(data, info, this);
}

void CEPConnHandler::OnIdle() {

}

void CEPConnHandler::OnClose() {
    ZTP_DATA_INFO info;
    info.flag = 203;
    info.data_len = 0;
    info.type = 0x0001;
    info.data_len = 4;
    char buf[4] = {0};
    *(unsigned int *) buf = m_remoteid;
    this->m_dispatch->doInterceptor(buf, &info, this);
}

bool CEPConnHandler::OnAuth(const char * user, const char * passwd, const WORD* node_id, int count, ZTPServerHandler* pServerHandler) {
    this->m_remoteid = *(unsigned int*) node_id;
    ZTP_DATA_INFO info;
    info.flag = 202;
    info.data_len = 0;
    info.type = 0x0001;
    info.data_len = 4;
    char buf[4] = {0};
    *(unsigned int *) buf = m_remoteid;
    this->m_dispatch->doInterceptor(buf, &info, this);
    return true;
}

void CEPConnHandler::OnConnect(ZTPService * service, const char * ip, int port) {
    m_ip.assign(ip);
    this->m_remotip = cep::str2ip(m_ip, 0);
    m_remoteport = port;
    this->m_ztpService = service;
    //        printf("connect successful ipstr[%s] ip[%x] port[%d]\n", ip, m_remotip, port);
}

ZTPHandler * CEPConnHandler::Clone() {
    CEPConnHandler*handler = new CEPConnHandler(m_localid);
    handler->m_dispatch = this->m_dispatch;
    return handler;
}

void CEPConnHandler::OnFail(const char * msg) {
    printf("CEPConnHandler::OnFail[%s]\n", msg);
    //        InterceptorPolicy* chain_temp = m_icptorchain;
    //        ZTP_DATA_INFO info;
    //        info.flag = 203;
    //        info.data_len = 0;
    //        info.type = 0x0001;
    //        info.data_len = 4;
    //        char buf[4] = {0};
    //        *(unsigned int *) buf = m_remoteid;
    //        cevent event(info, buf, false, false);
    //        while (chain_temp) {
    //            chain_temp = chain_temp->doInterceptor(event, this) > InterceptorPolicy::SUCC_BREAK ? chain_temp->next() : 0;
    //        }
}
