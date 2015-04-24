#include "stateMessager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define MESSAGEPORT 4000
#define DATAPORT 4001
#define BACKLOG 10

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::thread;

stateMessager::stateMessager() {
    init();
}

stateMessager::~stateMessager() {
    finish();
}

int stateMessager::init() {
    status = 1;
    t0 = new thread(&stateMessager::setMsgSocket, this);
    t1 = new thread(&stateMessager::setDataSocket, this);
    return 1;
}

int stateMessager::finish() {
    status = 0;
    t0->join();
    t1->join();
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

        if(clientMsg != -1)
            close(clientMsg);
        clientMsg = remoteMsg;
    }
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

        if(clientData != -1)
            close(clientData);
        clientData = remoteData;
    }
    close(clientData);
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

