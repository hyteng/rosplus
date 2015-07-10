#ifndef adc1785_h
#define adc1785_h

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
class adc1785 :public smBase {
    public:
        adc1785(const std::string& n);
        ~adc1785();
        virtual void* getHelp() {return (void*)&base;};
        typedef uint32_t regAddrType;
        typedef uint16_t regType;

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
        //virtual int OTFCTRL(void* argv[]=NULL);
        virtual int accessReg(const int idx, const int rw, regData& data);
        virtual int maskRegData(regData& data, regData& mask);
        virtual int unmaskRegData(regData& data, regData& mask);

    private:
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
        uint32_t base, length;
        unsigned int regValue[80], confValue[100]; //reg is 16bit, use uint32 and cut later
};

#endif
