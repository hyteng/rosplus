#ifndef daq_h
#define daq_h

#include <thread>
#include <fstream>
#include "../src/smBase.h"


class daq :public smBase {

    public:
        daq(const std::string& n);
        ~daq();
        
    protected:
        virtual int LoadedCONF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);
        virtual int PausedSPTR(int para);

    private:
        int beforDaq();
        int afterDaq();
        int startDaq();
        int stopDaq();
        void runDaq(void* para);
        int sendData(void* p0, const unsigned int& nBytes);

    private:
        int runDaqCtrl;
        int daqStatus;
        int netMsgQue;
        unsigned int daqCount;
        unsigned int totalDaqSize;
        streamMsg daqMsg;
        std::thread *t0;

        std::string fileName;
        fstream outFile;
};

#endif
