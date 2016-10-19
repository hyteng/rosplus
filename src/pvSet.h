#ifndef pvSet_h
#define pvSet_h

#include <map>
#include "smBase.h"

class stateMachine;
class pvSet :public smBase {

    public:
        pvSet(const std::string& n);
        ~pvSet();

        void setMachine(stateMachine* m);

    protected:
        virtual int InitializedLOAD(std::string& ret, void* para[]=NULL);
        virtual int LoadedUNLD(std::string& ret, void* para[]=NULL);
        
    private:
        /* data */
        stateMachine* theFrame;
        std::string libDir;
        std::map<std::string, std::pair<void*, int> > sharedLibHandle;
        std::map<std::string, std::string> dev2libMap;
};

#endif
