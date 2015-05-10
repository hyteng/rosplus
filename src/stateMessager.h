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
#include <mutex>
#include <condition_variable>
#include <sstream>

class stateMachine;

class stateMessager {

    //typedef int (stateMachine::*callFunc)(const std::string&);
    public:
        stateMessager();
        ~stateMessager();
        int init(stateMachine* m);
        int finish();
        int stateOut(const int& stKey, const std::string& stMsg);
        int stateOut(std::stringstream& msg);
        int setMsgSocket();
        int setDataSocket();
        int setCtrlSocket();
        int contrlMsg(int socketMsg);
        int sendMsg(const std::string& msg);
        int sendData(void *p0, const unsigned int &nBytes);
    private:
        stateMachine* pMachine;
        //callFunc dispatch;
        int status;
        int msgSocket, dataSocket, ctrlSocket; //socket0: message, socket1: data
        int clientMsg, clientData, clientCtrl;
        struct sockaddr_in hostAddr[3];
        struct sockaddr_in clientAddr[3];
        std::thread *t0, *t1, *t2, *tc;
        std::mutex dataMutex, msgMutex, debugMutex, ctrlMutex;
        //std::condition_variable cv;
};


#endif

