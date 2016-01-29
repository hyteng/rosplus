#ifndef mpodHV_h
#define mpodHV_h

#include "../src/smBase.h"
#include <stdint.h>
#include <queue>
#include <vector>
#include "vmelib.h"

//#include "snmp_pp.h"
//#define SYSDESCR "1.3.6.1.2.1.1.1.0"

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

//class mpodHV;
//typedef uint32_t (mpodHV::* mpodHVFunc)();
#define MPODHVEVENTUINTSIZE 18

class mpodHV :public smBase {
    public:
        typedef uint32_t regAddrType;
        typedef uint16_t regType;
        typedef uint32_t (*mpodHVEvent)[MPODHVEVENTUINTSIZE];

        mpodHV(const std::string& n);
        ~mpodHV();
        virtual int queryInterface(const string& funcName, void* para[], void* ret);

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
        uintptr_t getBuffAddr();
        uint32_t getEventTh();
        uint32_t getTranSize();
        int waitTrigger();
        int afterTransfer();
        int ackTrigger();
        int packData(unsigned int &packSize);
        int packDataTest(unsigned int& packSize);
        int fillEvent(unsigned int &packSize);
        int flushData();

        int configAdc();
        int releaseAdc();
        int prepAdc();
        int finishAdc();
        int startAdc();
        int stopAdc();
        int enableAdc();
        int disableAdc();
        int accessRegNormal(const regAddrType addr, const int rw, regType* data);

        VMEBridge *pvme;

        int image;
        uint32_t base, length; // 32bit VME address
        uint32_t regValue[80], confValue[100]; //reg is 16bit, use uint32 and cut later
        mpodHVEvent eventSet;
        int eventPtrR, eventPtrW;
        std::queue<unsigned int> *eventIdx;
        char v[4];
        bool sendData;
};

#endif
