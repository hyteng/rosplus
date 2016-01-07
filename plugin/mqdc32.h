#ifndef mqdc32_h
#define mqdc32_h

#include "../src/smBase.h"
#include <stdint.h>
#include <queue>
#include <vector>
#include "vmelib.h"

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

#define MQDC32EVENTUINTSIZE 66
class mqdc32 :public smBase {
    public:
        typedef uint32_t regAddrType;
        typedef uint16_t regType;
        typedef uint32_t (*mqdc32Event)[MQDC32EVENTUINTSIZE];

        mqdc32(const std::string& n);
        ~mqdc32();
        virtual int queryInterface(const std::string& funcName, void* para[], void* ret);

    protected:
        virtual int InitializedLOAD(std::string& ret, void* para[]=NULL);
        virtual int LoadedUNLD(std::string& ret, void* para[]=NULL);
        virtual int LoadedCONF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredUNCF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredPREP(std::string& ret, void* para[]=NULL);
        virtual int ReadySTOP(std::string& ret, void* para[]=NULL);
        virtual int ReadySATR(std::string& ret, void* para[]=NULL);
        virtual int RunningSPTR(std::string& ret, void* para[]=NULL);
        virtual int RunningPAUS(std::string& ret, void* para[]=NULL);
        virtual int PausedSPTR(std::string& ret, void* para[]=NULL);
        virtual int PausedRESU(std::string& ret, void* para[]=NULL);
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
        int fillEvent(unsigned int &packSize);
        int flushData();

        int configAdc(std::string& ret);
        int releaseAdc();
        int prepAdc();
        int finishAdc();
        int startAdc();
        int stopAdc();
        int enableAdc();
        int disableAdc();
        int setDWAdc(int dw);
        int runAdc();
        int accessRegNormal(const regAddrType addr, const int rw, regType* data);

        VMEBridge *pvme;
        smBase *vmeCtrl;

        int image;
        uint32_t base, length, imgCtrlAddr;
        uint16_t confValue[200], regValue[200]; //reg is 16bit, use uint32 and cut later

        std::mutex semMutex;

        mqdc32Event eventSet;
        int eventPtrR, eventPtrW;
        std::queue<unsigned int> *eventIdx;
        char v[4];
};

#endif
