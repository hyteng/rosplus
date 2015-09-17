#ifndef mqdc32_h
#define mqdc32_h

#include "../src/smBase.h"
#include <stdint.h>
#include "vmelib.h"

class regUint16 :public regData {
    public:
        regUint16();
        ~regUint16();
        virtual void setValueS(std::stringstream& ss) {ss>>v;};
        virtual void setValueP(const void* p) {v=*(const uint16_t*)p;};
        virtual void* getValueP() {return &v;};
        virtual std::string getValueS() {std::stringstream ss;ss<<v;return ss.str();};
        virtual regData& ptr(const void* p) {v=*(const uint16_t*)p;return *this;}; 
    private:
        uint16_t v;
};

class mqdc32 :public smBase {
    public:
        typedef uint32_t regAddrType;
        typedef uint16_t regType;

        mqdc32(const std::string& n);
        ~mqdc32();
        virtual bool queryInterface(const std::string& funcName, void* para[], void* ret);

    protected:
        virtual int InitializedLOAD(void* argv[]=NULL);
        virtual int LoadedUNLD(void* argv[]=NULL);
        virtual int LoadedCONF(void* argv[]=NULL);
        virtual int ConfiguredUNCF(void* argv[]=NULL);
        virtual int ConfiguredPREP(void* argv[]=NULL);
        virtual int ReadySTOP(void* argv[]=NULL);
        virtual int ReadySATR(void* argv[]=NULL);
        virtual int RunningSPTR(void* argv[]=NULL);
        virtual int RunningPAUS(void* argv[]=NULL);
        virtual int PausedSPTR(void* argv[]=NULL);
        virtual int PausedRESU(void* argv[]=NULL);
        virtual int OTFCTRL(void* argv[]=NULL);
        virtual int accessReg(const int idx, const int rw, regData& data);
        virtual int maskRegData(regData& data, regData& mask);
        virtual int unmaskRegData(regData& data, regData& mask);

    private:
        uintptr_t getBuffAddr() {return base;};
        int run();

        int configAdc();
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
        uint32_t  regValue[200], confValue[200]; //reg is 16bit, use uint32 and cut later

        std::mutex semMutex;
};


#endif
