#include "stateMessager.h"
#include "stateMachine.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define MESSAGEPORT 4000
#define DATAPORT 4001
#define BACKLOG 10
#define MAXMSG 200

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::thread;

typedef int callFunc(const string&);

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
    //t2 = new thread(&stateMessager::contrlMsg, this);
    return 1;
}

int stateMessager::finish() {
    status = 0;
    t0->join();
    t1->join();
    //t2->join();
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

    clientMsg = -1;
    int remoteMsg;
    while(status) {
        int sinSize = sizeof(struct sockaddr_in);
        if((remoteMsg=accept(msgSocket, (struct sockaddr*)&clientAddr[0], (socklen_t*)&sinSize)) == -1)
            continue;

        std::unique_lock <std::mutex> lck(msgMutex);
        std::unique_lock <std::mutex> cvlck(cvMutex);
        if(clientMsg != -1) {
            t2->join();
            close(clientMsg);
        }
        clientMsg = remoteMsg;
        t2 = new thread(&stateMessager::contrlMsg, this);
        //if(clientMsg == -1)
            //cv.notify_all();
        lck.unlock();
    }
    t2->join();
    close(clientMsg);
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
        if(clientData != -1)
            close(clientData);
        clientData = remoteData;
        lock.unlock();
    }
    close(clientData);
    return 1;
}

int stateMessager::contrlMsg() {

    std::unique_lock <std::mutex> lck(cvMutex);
    int rval, res;
    char msg[MAXMSG];
    while(status) {

        if(clientMsg == -1) {
            //cv.wait(lck);
            return 1;
        }
        
        //if(ctrlSocket != clientMsg)
            //ctrlSocket = clientMsg;

        if ((rval = read(ctrlSocket, msg, MAXMSG)) < 0) {
            continue;
        }


        ctrlMsg = string(msg);
        res = pMachine->dispatch2(msg);
        if(res == 0)
            break;
    }

    return 1;
}

int stateMessager::sendMsg(const string& msg) {
    std::unique_lock<std::mutex> lock(msgMutex);
    if(clientMsg==-1)
        return 0;
    int tranSize = 0;
    int result;
    char* p1 = (char*)msg.c_str();
    unsigned int nBytes = strlen(msg.c_str())+1;
    while(tranSize < nBytes) {
        result = send(clientData, (char*)p1, nBytes, 0);
        if(result < 0)
            return tranSize;
        else {
            p1 += result;
            nBytes -= result;
            tranSize += result;
        }
    }
    return tranSize;
}

int stateMessager::sendData(void* p0, int nBytes) {
    std::unique_lock<std::mutex> lock(dataMutex);
    if(clientData==-1)
        return 0;
    int tranSize = 0;
    int result;
    char* p1 = (char*)p0;
    while(tranSize < nBytes) {
        result = send(clientData, (char*)p1, nBytes, 0);
        if(result < 0)
            return tranSize;
        else {
            p1 += result;
            nBytes -= result;
            tranSize += result;
        }
    }
    return tranSize;
}

