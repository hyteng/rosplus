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
        virtual int InitializedLOAD(int para);
        virtual int LoadedUNLD(int para);
        virtual int LoadedCONF(int para);
        virtual int ConfiguredUNCF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);

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
