#ifndef daq_h
#define daq_h

#include <string>
#include <thread>
#include "ringBuffer.h"
#include "../src/smBase.h"

class daq :public smBase {

    public:
        daq(const std::string& n);
        ~daq();

    protected:
        virtual int InitializedLOAD(int para);
        virtual int LoadedUNLD(int para);
        virtual int LoadedCONF(int para);
        virtual int ConfiguredUNCF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);
        virtual int RunningPAUS(int para);
        virtual int PausedSTOP(int para);
        virtual int PausedRESU(int para);
        virtual int PausedSATR(int para);
        virtual int SelfTrans(int para);
        virtual int AnyIMPO(int para);
        virtual int AnyEXIT(int para);

        int beforeDaq();
        int afterDaq();
        int startDaq();
        int stopDaq();

    private:
        ringBuffer *ring0, *ring1;
        unsigned int size0, size1, dmaAddr;
        std::thread *t0;
};

#endif
