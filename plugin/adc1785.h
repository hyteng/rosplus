#ifndef adc1785_h
#define adc1785_h

#include "../src/smBase.h"
#include "vmelib.h"

class adc1785 :public smBase {
    public:
        adc1785(const std::string& n);
        ~adc1785();
        virtual void* getHelp() {return (void*)&base;};

    protected:
        virtual int InitializedLOAD(int argc=0, void* argv[]=NULL);
        virtual int LoadedUNLD(int argc=0, void* argv[]=NULL);
        virtual int LoadedCONF(int argc=0, void* argv[]=NULL);
        virtual int ConfiguredUNCF(int argc=0, void* argv[]=NULL);
        virtual int ConfiguredPREP(int argc=0, void* argv[]=NULL);
        virtual int ReadySTOP(int argc=0, void* argv[]=NULL);
        virtual int ReadySATR(int argc=0, void* argv[]=NULL);
        virtual int RunningSPTR(int argc=0, void* argv[]=NULL);
        virtual int RunningPAUS(int argc=0, void* argv[]=NULL);
        virtual int PausedSPTR(int argc=0, void* argv[]=NULL);
        virtual int PausedRESU(int argc=0, void* argv[]=NULL);
        virtual int OTFCONF(int argc=0, void* argv[]=NULL);

    private:
        int configAdc();
        int releaseAdc();
        int prepAdc();
        int finishAdc();
        int startAdc();
        int stopAdc();
        int getStatus();
        int clearBuff();
        //bool offline, clearData, overRange, lowTh, slideScale, stepTh, autoInc, emptyProg, slideSub, allTrigger, blkend, progResetMod, busError, align64, busErrorFlag, selAddr, softReset;
        //unsigned int base, lenght;
        // read only status
        //bool buffEmpty, buffFull, CSel0, CSel1, DSel0, DSel1, DRDY, gDRDY, busy, gbusy, purged, termOn, EVRDY;
        VMEBridge *pvme;

        int image;
        uint32_t base, length;
        unsigned int regValue[80], confValue[100]; //reg is 16bit, use uint32 and cut later
};

#endif
