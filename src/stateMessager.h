/*=============================================================================
FileName: stateMessager.h
Desc: message handler for states
Author: Haiyun.teng
Email: Haiyun.teng@gmail.com
Version: 0.0.1
=============================================================================*/ 

#ifndef stateMessager_h
#define stateMessager_h
    
#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>

class stateMessager {

    public:
        stateMessager();
        ~stateMessager();
        int init();
        int finish();
        int setMsgSocket();
        int setDataSocket();
        int stateOut(const int& stKey, const std::string& stMsg);
        unsigned int sendMsg(const std::string& msg);
        unsigned int sendData(void *p0, const unsigned int nBytes);
    private:
        int status;
        int msgSocket, dataSocket; //socket0: message, socket1: data
        int clientMsg, clientData;
        struct sockaddr_in hostAddr[2];
        struct sockaddr_in clientAddr[2];
        std::thread *t0, *t1;
};


#endif

