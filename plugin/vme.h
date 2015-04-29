#ifndef vme_h
#define vme_h

#include <thread>
#include <stdint.h>
#include <../src/smBase.h>
#include <../universeII/vmelib/vmelib.h>

class vme :public smBase {
    public:
        vme(const std::string& n);
        ~vme();

    protected:
        virtual int LoadedCONF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);

    private:
        int configVme();
        int startVme();
        int stopVme();
        void runVme();

    private:
        int runVmeCtrl;
        int vmeStatus;
        int devMsgQue;
        unsigned int vmeCount;
        unsigned int totalVmeSize;
        streamMsg vmeMsg;
        std::thread *t0;

        VMEBridge *pvme;
        uint32_t dmaBase;
        uint32_t dmaSize;
        uint32_t adc0Base;
};
#endif
