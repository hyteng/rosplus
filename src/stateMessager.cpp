#include "stateMessager.h"
#include "stateMachine.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

#define MESSAGEPORT 4000
#define DATAPORT 4001
#define CTRLPORT 4002
#define BACKLOG 10
#define MAXMSG 200

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::thread;

//typedef int callFunc(const string&);

stateMessager::stateMessager() {
}

stateMessager::~stateMessager() {
    finish();
}

int stateMessager::init(stateMachine* m) {
    status = 1;
    pMachine = m;
    //dispatch = func;
    t0 = new thread(&stateMessager::setMsgSocket, this);
    t1 = new thread(&stateMessager::setDataSocket, this);
    t2 = new thread(&stateMessager::setCtrlSocket, this);
    return 1;
}

int stateMessager::finish() {
    status = 0;
    t0->join();
    t1->join();
    t2->join();
    return 0;
}

int stateMessager::stateOut(const int& stKey, const string& stMsg) {
    std::unique_lock<std::mutex> lock(debugMutex);
    cout << "Key level " << stKey << ", " << stMsg << endl;
    return 1;
}

int stateMessager::stateOut(stringstream& msg) {
    std::unique_lock<std::mutex> lock(debugMutex);
    cout << msg.str() << endl;
    sendMsg(msg.str()+"\n");
    msg.str("");
    return 1;
}

int stateMessager::setMsgSocket() {
    hostAddr[0].sin_family = AF_INET; // host byte order
    hostAddr[0].sin_port = htons(MESSAGEPORT); // short, network byte order
    hostAddr[0].sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
    //bzero(&(hostAddr[0].sin_zero), 8); // zero the rest of the struct, need stream.h
    memset(&(hostAddr[0].sin_zero), 0, 8*sizeof(unsigned char));

    if((msgSocket=socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return 0;
    if((bind(msgSocket, (struct sockaddr *)&hostAddr[0], sizeof(struct sockaddr))) == -1)
        return 0;
    if(listen(msgSocket, BACKLOG) == -1)
        return 0;

    string quitMsg = "cmd#ctrl#quitMsgThread";
    clientMsg = -1;
    int remoteMsg;
    while(status) {
        int sinSize = sizeof(struct sockaddr_in);
        if((remoteMsg=accept(msgSocket, (struct sockaddr*)&clientAddr[0], (socklen_t*)&sinSize)) == -1)
            continue;
        std::unique_lock <std::mutex> lock(msgMutex);
        int flags = fcntl(remoteMsg, F_GETFL, 0);
        if(flags < 0) {
            shutdown(remoteMsg, 2);
            continue;
        }
        fcntl(remoteMsg, F_SETFL, flags|O_NONBLOCK);
        if(clientMsg != -1)
            shutdown(clientMsg, 2);
        clientMsg = remoteMsg;
        lock.unlock();
        lock.release();
    }
    shutdown(clientMsg, 2);
    return 1;
}

int stateMessager::setDataSocket() {
    hostAddr[1].sin_family = AF_INET; // host byte order
    hostAddr[1].sin_port = htons(DATAPORT); // short, network byte order
    hostAddr[1].sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
    //bzero(&(hostAddr[1].sin_zero), 8); // zero the rest of the struct, need stream.h
    memset(&(hostAddr[1].sin_zero), 0, 8*sizeof(unsigned char));

    if((dataSocket=socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return 0;
    if((bind(dataSocket, (struct sockaddr *)&hostAddr[1], sizeof(struct sockaddr))) == -1)
        return 0;
    if(listen(dataSocket, BACKLOG) == -1)
        return 0;

    clientData = -1;
    int remoteData;
    while(status) {
        int sinSize = sizeof(struct sockaddr_in);
        if((remoteData=accept(dataSocket, (struct sockaddr*)&clientAddr[0], (socklen_t*)&sinSize)) == -1)
            continue;
        std::unique_lock<std::mutex> lock(dataMutex);
        int flags = fcntl(remoteData, F_GETFL, O_NONBLOCK);
        if(flags < 0) {
            shutdown(remoteData, 2);
            continue;
        }
        fcntl(remoteData, F_SETFL, flags|O_NONBLOCK);
        if(clientData != -1)
            shutdown(clientData, 2);
        clientData = remoteData;
        lock.unlock();
        lock.release();
    }
    shutdown(clientData, 2);
    return 1;
}

int stateMessager::setCtrlSocket() {
    hostAddr[2].sin_family = AF_INET; // host byte order
    hostAddr[2].sin_port = htons(CTRLPORT); // short, network byte order
    hostAddr[2].sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
    //bzero(&(hostAddr[2].sin_zero), 8); // zero the rest of the struct, need stream.h
    memset(&(hostAddr[2].sin_zero), 0, 8*sizeof(unsigned char));

    if((ctrlSocket=socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return 0;
    if((bind(ctrlSocket, (struct sockaddr *)&hostAddr[2], sizeof(struct sockaddr))) == -1)
        return 0;
    if(listen(ctrlSocket, BACKLOG) == -1)
        return 0;

    string quitMsg = "cmd#ctrl#quitMsgThread";
    clientCtrl = -1;
    int remoteCtrl;
    while(status) {
        int sinSize = sizeof(struct sockaddr_in);
        if((remoteCtrl=accept(ctrlSocket, (struct sockaddr*)&clientAddr[2], (socklen_t*)&sinSize)) == -1)
            continue;
        std::unique_lock <std::mutex> lock(ctrlMutex);
        if(clientCtrl != -1) {
            shutdown(clientCtrl, 2);
            //int result = send(clientCtrl, quitMsg.c_str(), quitMsg.length()+1, 0);
            tc->join();
        }
        clientCtrl = remoteCtrl;
        tc = new thread(&stateMessager::contrlMsg, this, clientCtrl);
        lock.unlock();
        lock.release();
    }
    shutdown(clientCtrl, 2);
    tc->join();
    return 1;
}

int stateMessager::contrlMsg(int socketMsg) {
    std::unique_lock<std::mutex> lock(ctrlMutex);
    int rval, res;
    char msg[MAXMSG];
    string ctrlMsg;
    while(status) {
        if(socketMsg < 0) {
            return 0;
        }
        
        //lock(ctrlMutex);
        memset(msg, '\0', MAXMSG);
        if ((rval = read(socketMsg, msg, MAXMSG)) <= 0) {
            break;
        }
        ctrlMsg = string(msg);
        cout << "ctrlMsg: " << ctrlMsg << endl;
        if(ctrlMsg == "cmd#ctrl#quitMsgThread")
            return 1;
        res = pMachine->dispatch2(msg);
        if(res == 0)
            break;

        //lock.unlock();
        //lock.release();
    }
    return 1;
}

int stateMessager::sendMsg(const string& msg) {
    std::unique_lock<std::mutex> lock(msgMutex);
    if(clientMsg==-1)
        return 0;
    unsigned int tranSize = 0;
    int result;
    char* p1 = (char*)msg.c_str();
    unsigned int nBytes = msg.length()+1;//strlen(msg.c_str())+1;
    while(tranSize < nBytes) {
        result = send(clientMsg, (char*)p1, nBytes, 0);
        if(result <= 0)
            return tranSize;
        else {
            p1 += result;
            nBytes -= result;
            tranSize += result;
        }
    }
    return tranSize;
}

int stateMessager::sendData(void* p0, const unsigned int &nBytes) {
    std::unique_lock<std::mutex> lock(dataMutex);
    if(clientData==-1)
        return 0;
    unsigned int tranSize=0, totalSize=nBytes;
    int result;
    char* p1 = (char*)p0;
    while(tranSize < totalSize) {
        result = send(clientData, p1, totalSize, 0);
        if(result <= 0)
            return tranSize;
        else {
            p1 += result;
            totalSize -= result;
            tranSize += result;
        }
    }
    //cout << "stateMessager: sendData " << nBytes << ", transfer " << tranSize << " bytes." << endl;
    return tranSize;
}

