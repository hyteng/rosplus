#ifndef luxx_h
#define luxx_h

#include "../src/smBase.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

class regUint16 :public regData {
    public:
        virtual void setValueS(std::stringstream& ss) {ss>>v;};
        virtual void setValueP(const void* p) {v=*(const uint16_t*)p;};
        virtual void* getValueP() {return &v;};
        virtual std::string getValueS() {std::stringstream ss;ss<<v;return ss.str();};
        virtual regData& ptr(const void* p) {v=*(const uint16_t*)p;return *this;}; 
    private:
        uint16_t v;
};

//class luxx;
//typedef uint32_t (luxx::* luxxFunc)();
#define LUXXEVENTUINTSIZE 4

class luxx :public smBase {
    public:
        typedef uint32_t regAddrType;
        typedef uint16_t regType;

        luxx(const std::string& n);
        ~luxx();
        virtual int queryInterface(const std::string& funcName, void* para[], void* ret);

        typedef struct tty_info_t {
            int fd; // 串口设备ID
            pthread_mutex_t mt; // 线程同步互斥对象
            std::string name; // 串口设备名称，例："/dev/ttyS0"
            struct termios ntm; // 新的串口设备选项
            struct termios otm; // 旧的串口设备选项
        } TTY_INFO;

    protected:
        virtual int InitializedLOAD(std::string& ret, void* para[]=NULL);
        virtual int LoadedUNLD(std::string& ret, void* para[]=NULL);
        virtual int LoadedCONF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredUNCF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredPREP(std::string& ret, void* para[]=NULL);
        virtual int ReadySTOP(std::string& ret, void* para[]=NULL);
        virtual int ReadySATR(std::string& ret, void* para[]=NULL);
        virtual int RunningSPTR(std::string& ret, void* para[]=NULL);
        virtual int PausedSPTR(std::string& ret, void* para[]=NULL);
        virtual int accessReg(const int idx, const int rw, regData& data);
        virtual int maskRegData(regData& data, regData& mask);
        virtual int unmaskRegData(regData& data, regData& mask);

    private:
        int readyTTY();
        int cleanTTY();
        int setTTYSpeed(int speed);
        int setTTYParity(int databits, int parity, int stopbits);
        int sendnTTY(char *pbuf, int size);
        int recvnTTY(char *pbuf, int size);
        int lockTTY();
        int unlockTTY();
        int setTTY(int speed, int databits, int parity, int stopbits);

        int fillEvent(unsigned int &packSize);

        int configLuxx(std::string& ret);
        int releaseLuxx();
        int prepLuxx();
        int finishLuxx();
        int startLuxx();
        int stopLuxx();
        int accessRegNormal(const regAddrType addr, const int rw, regType* data);

        // tty setting
        std::string ttyName;
        TTY_INFO* ptty;
        int ttySpeed, ttyDataBits, ttyParity, ttyStopBits;
        // control setting
        int mode, autoP, autoS;
        double power;
        uint32_t regValue[10], confValue[10];
        // status
        int status, powerLevel, powerOn, laserOn;
};

#endif
