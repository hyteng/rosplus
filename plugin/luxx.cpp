#include "luxx.h"
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/file.h>

using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::hex;
using std::dec;


static map<string, vector<string> > luxx_ctrl2conf;
static map<string, vector<int> > luxx_ctrl2level;
static map<string, int> luxx_conf2reg;
static map<string, uintptr_t> luxx_conf2mask;
static vector<uintptr_t> luxx_regAddr;
static vector<int> luxx_regRWIdx;

static int setCtrl() {
    luxx_ctrl2conf.clear();

    luxx_conf2reg.clear();
    luxx_conf2mask.clear();

    luxx_regAddr.clear();
    luxx_regRWIdx.clear();

    return 1;
}

static int dummy = setCtrl();

luxx::luxx(const string& n): smBase(n) {
    ctrl2conf = &luxx_ctrl2conf;
    ctrl2level = &luxx_ctrl2level;
    conf2reg = &luxx_conf2reg;
    conf2mask = &luxx_conf2mask;
    regAddr = &luxx_regAddr;
    regRWIdx = &luxx_regRWIdx;

    vd = (regData*) new regUint16();
    vm = (regData*) new regUint16();
}

luxx::~luxx() {
}

int luxx::queryInterface(const std::string& funcName, void* para[], void* ret) {
    if(funcName == "fillEvent") {
        *(int*)ret = fillEvent(*(unsigned int*)para[0]);
        return 1;
    }
    return 0;
}

int luxx::InitializedLOAD(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return smBase::InitializedLOAD(ret, para);
}

int luxx::LoadedUNLD(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return smBase::LoadedUNLD(ret, para);
}

int luxx::LoadedCONF(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    if(!configLuxx(ret))
        return -1;
    ret = "3;"+ret;
    stId = 3;
    return 3;
}

int luxx::ConfiguredUNCF(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    releaseLuxx();
    return smBase::ConfiguredUNCF(ret, para);
}

int luxx::ConfiguredPREP(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepLuxx();
    return smBase::ConfiguredPREP(ret, para);
}

int luxx::ReadySTOP(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    finishLuxx();
    return smBase::ReadySTOP(ret, para);
}

int luxx::ReadySATR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    return smBase::ReadySATR(ret, para);
}

int luxx::RunningSPTR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    return smBase::RunningSPTR(ret, para);
}

int luxx::PausedSPTR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    return smBase::PausedSPTR(ret, para);
}

int luxx::accessReg(const int idx, const int rw, regData& data) {
    int res;
    regAddrType addr = *(regAddrType*)((*regAddr)[idx]);
    res = accessRegNormal(addr, rw, (regType*)data.getValueP());
    return res;
}

int luxx::maskRegData(regData& data, regData& mask) {
    regType mData = *(regType*)(data.getValueP());
    regType mTest = *(regType*)(mask.getValueP());
    data.setValueP(&mData);
    return 1;
}

int luxx::unmaskRegData(regData& data, regData& mask) {
    regType mData = *(regType*)(data.getValueP());
    regType mTest = *(regType*)(mask.getValueP());
    data.setValueP(&mData);
    return 1;
}

// 初始化串口设备并进行原有设置的保存
int luxx::readyTTY() {
    ptty = (TTY_INFO *)new TTY_INFO;
    if(ptty == NULL)
        return 1;
    printf("open tty \n");
    //memset(ptty, 0, sizeof(TTY_INFO));
    pthread_mutex_init(&(ptty->mt), NULL);
    ptty->name = "/dev/tty"+ttyName;
    printf("open %s \n", (ptty->name).c_str());
    // 打开并且设置串口
    ptty->fd = open((ptty->name).c_str(), O_RDWR | O_NOCTTY |O_NDELAY);
    if(ptty->fd <0) {
        free(ptty);
        return 1;
    }
    // 取得并且保存原来的设置
    tcgetattr(ptty->fd, &ptty->otm);
    return 0;
}

// 清理串口设备资源
int luxx::cleanTTY() {
    // 关闭打开的串口设备
    if(ptty->fd>0) {
        tcsetattr(ptty->fd, TCSANOW,&ptty->otm);
        close(ptty->fd);
        ptty->fd = -1;
        delete ptty;
        ptty = NULL;
    }
    return 0;
}

int luxx::setTTYSpeed(int speed) {
    // 进行新的串口设置,数据位为8位
    bzero(&ptty->ntm, sizeof(ptty->ntm));
    tcgetattr(ptty->fd, &ptty->ntm);
    ptty->ntm.c_cflag = /*CS8 |*/ CLOCAL | CREAD;

    switch(speed) {
    case 300:
        ptty->ntm.c_cflag |= B300;
        break;
    case 1200:
        ptty->ntm.c_cflag |= B1200;
        break;
    case 2400:
        ptty->ntm.c_cflag |= B2400;
        break;
    case 4800:
        ptty->ntm.c_cflag |= B4800;
        break;
    case 9600:
        ptty->ntm.c_cflag |= B9600;
        break;
    case 19200:
        ptty->ntm.c_cflag |= B19200;
        break;
    case 38400:
        ptty->ntm.c_cflag |= B38400;
        break;
    case 57600:
        ptty->ntm.c_cflag |= B57600;
        break;
    case 115200:
        ptty->ntm.c_cflag |= B115200;
        break;
    }
    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;

    tcflush(ptty->fd, TCIFLUSH);
    tcsetattr(ptty->fd, TCSANOW, &ptty->ntm);

    return 0;
}
// 设置串口数据位，停止位和效验位
// ptty 参数类型(TTY_INFO *),已经初始化的串口设备信息结构指针
// databits 参数类型(int), 数据位,取值为7或者8
// stopbits 参数类型(int),停止位,取值为1或者2
// parity 参数类型(int),效验类型 取值为N,E,O,,S (0,1,2,3)
// return 返回值类型(int),函数执行成功返回零值，否则返回大于零的值
int luxx::setTTYParity(int databits, int parity, int stopbits) {
    // 取得串口设置
    if(tcgetattr(ptty->fd, &ptty->ntm) != 0) {
        return 1;
    }

    bzero(&ptty->ntm, sizeof(ptty->ntm));
    ptty->ntm.c_cflag = CS8 | CLOCAL | CREAD;
    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;
    // 设置串口的各种参数
    printf("setting tty databits: %d\n", databits);
    ptty->ntm.c_cflag &= ~CSIZE;
    switch(databits) { //设置数据位数
    case 7:
        ptty->ntm.c_cflag |= CS7;
        break;
    case 8:
        ptty->ntm.c_cflag |= CS8;
        break;
    default:
        printf("Unsupported data size\n");
        return 5;
    }

    printf("setting tty parity: %d\n", parity);
    switch(parity) { // 设置奇偶校验位数
    //case n:
    case 0:
        ptty->ntm.c_cflag &= ~PARENB; /* Clear parity enable */
        ptty->ntm.c_iflag &= ~INPCK; /* Enable parity checking */
        break;
    //case o:
    case 1:
        ptty->ntm.c_cflag |= (PARODD|PARENB); /* 设置为奇效验*/
        ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    //case e:
    case 2:
        ptty->ntm.c_cflag |= PARENB; /* Enable parity */
        ptty->ntm.c_cflag &= ~PARODD; /* 转换为偶效验*/
        ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    //case S:
    case 3: /*as no parity*/
        ptty->ntm.c_cflag &= ~PARENB;
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    default:
        printf("Unsupported parity\n");
        return 2;
    }
    // 设置停止位
    printf("setting tty stopbits: %d\n", stopbits);
    switch (stopbits) {
    case 1:
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    case 2:
        ptty->ntm.c_cflag |= CSTOPB;
        break;
    default:
        printf("Unsupported stop bits\n");
        return 3;
    }

    ptty->ntm.c_lflag = 0;
    ptty->ntm.c_cc[VTIME] = 0; // inter-character timer unused

    ptty->ntm.c_cc[VMIN] = 1; // blocking read until 1 chars received

    tcflush(ptty->fd, TCIFLUSH);
    if(tcsetattr(ptty->fd, TCSANOW, &ptty->ntm) != 0) {
        printf("SetupSerial \n");
        return 4;
    }

    return 0;
}

int luxx::recvnTTY(char *pbuf, int size) {
    int ret,left,bytes;

    left = size;
    while(left>0) {
        ret = 0;
        bytes = 0;

        pthread_mutex_lock(&ptty->mt);
        ioctl(ptty->fd, FIONREAD, &bytes);
        if(bytes>0) {
            ret = read(ptty->fd,pbuf,left);
        }
        pthread_mutex_unlock(&ptty->mt);
        if(ret >0) {
            left -= ret;
            pbuf += ret;
        }
        usleep(100);
    }
    return size - left;
}

int luxx::sendnTTY(char *pbuf, int size) {
    int ret,nleft;
    char *ptmp;

    ret = 0;
    nleft = size;
    ptmp = pbuf;

    while(nleft>0) {
        pthread_mutex_lock(&ptty->mt);
        ret = write(ptty->fd,ptmp,nleft);
        pthread_mutex_unlock(&ptty->mt);

        if(ret >0) {
            nleft -= ret;
            ptmp += ret;
        }
        //usleep(100);
    }
    return size - nleft;
}

int luxx::lockTTY() {
    if(ptty->fd < 0) {
        return 1;
    }
    return flock(ptty->fd, LOCK_EX);
}

int luxx::unlockTTY() {
    if(ptty->fd < 0) {
        return 1;
    }
    return flock(ptty->fd, LOCK_UN);
}

// set TTY
int luxx::setTTY(int speed, int databits, int parity, int stopbits) {
    int status;
    if(ptty->fd < 0) {
        return 1;
    }

    status = setTTYSpeed(speed);
    if(status != 0)
        return status;

    status = setTTYParity(databits, parity, stopbits);
    if(status != 0)
        return status;

    printf("setting done.\n");
    return status;
}

int luxx::configLuxx(std::string& ret) {

    ttyName = "USB0";
    readyTTY();
    if(ptty == NULL) {
        printf("readyTTY(0) error\n");
        return 0;
    }
    lockTTY();
    setTTY(57600, 8, 0, 1);

    char answer[100];
    
    sendnTTY("?GOM\r", 5);
    recvnTTY(answer, 9);
    printf("LD mode: %s\n", answer);
    
    sendnTTY("?LOf\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    sendnTTY("?POf\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    sendnTTY("?SAP0\r", 6);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    sendnTTY("?SAS0\r", 6);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    sendnTTY("?GAS\r", 5);
    recvnTTY(answer, 9);
    printf("LD mode: %s\n", answer);

    sendnTTY("?ROM3\r", 6);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    sendnTTY("?SLPFFF\r", 8);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    return 1;
}

int luxx::releaseLuxx() {
    unlockTTY();
    cleanTTY();
    return 1;
}

int luxx::prepLuxx() {
    char answer[100];   
    sendnTTY("?POn\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    usleep(1000000);

    sendnTTY("?LOn\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);
    return 1;
}

int luxx::finishLuxx() {
    char answer[100];
    sendnTTY("?LOf\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);

    usleep(1000000);

    sendnTTY("?POf\r", 5);
    recvnTTY(answer, 6);
    printf("LD mode: %s\n", answer);
    return 1;
}

int luxx::accessRegNormal(const regAddrType addr, const int rw, regType* data) {
    return 1;
}

int luxx::fillEvent(unsigned int &packSize) {
    packSize = 0;
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new luxx(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
