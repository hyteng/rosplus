#include "frameEngine.h"
#include "stateMachine.h"
#include <dlfcn.h>
#include <sstream>

using std::string;
using std::stringstream;

frameEngine::frameEngine(const string& n) : smBase(n) {
}

frameEngine::~frameEngine() {
}

void frameEngine::setMachine(stateMachine* m) {
    theFrame = m;
}

int frameEngine::InitializedLOAD(std::string& ret, void* para[]) {
    
    cfgInfo->infoClear();
    cfgInfo->readConfig("runSet.conf");

    int res;
    string modeList;
    if((res=cfgInfo->infoGetString("config.module", modeList)) != 1) {
        return -1;
    }

    if((res=cfgInfo->infoGetString("config.libDir", libDir)) != 1) {
        return -1;
    }
    
    sharedLibHandle.clear();
    dev2libMap.clear();
    moduleStringSplit(modeList);
    std::map<std::string, std::string>::const_iterator modeIter;
    for(modeIter=dev2libMap.begin(); modeIter!=dev2libMap.end(); modeIter++) {
        //std::cout << "loop: " << modeIter->second << ", " << modeIter->first << std::endl;
        if(loadSharedModule(modeIter->second, modeIter->first, libDir)) {
            debugMsg << name << "# " << "loadSharedModule: (mode) " << modeIter->second << " (dev) " << modeIter->first;
            stMsg->stateOut(debugMsg);
        }
        else {
            return -1;
        }
    }
    stId = 2;
    ret = "2;";
    return 2;
}

int frameEngine::LoadedUNLD(std::string& ret, void* para[]) {
    std::map<std::string, std::string>::const_iterator modeIter;
    for(modeIter=dev2libMap.begin(); modeIter!=dev2libMap.end(); modeIter++) {
        if(unloadSharedModule(modeIter->first)) {
            debugMsg << name << "# " << "unloadSharedModule: (dev)" << modeIter->first;
            stMsg->stateOut(debugMsg);
        }
        else {
            return -1;
        }
    }
    stId = 1;
    ret = "1;";
    return 1;
}

int frameEngine::moduleStringSplit(const string& modeList) {
    std::cout << modeList << std::endl;
    stringstream sList;
    string mode, lib, dev;
    sList << modeList;
    while(getline(sList, mode, ';')) {
        std::cout << mode << std::endl;
        stringstream sMode(mode);
        getline(sMode, lib, '.');
        getline(sMode, dev, '.');
        dev2libMap[dev] = lib;
        std::cout << lib << ", " << dev << std::endl;
    }
    return dev2libMap.size();
}

int frameEngine::loadSharedModule(const string& modeName, const string& devName, const string& dir) {

    char* dlsym_error;
    string libPath = dir+"/"+modeName+".so";
    if(sharedLibHandle.find(modeName) == sharedLibHandle.end()) {
        void* libHandle = dlopen(libPath.c_str(), RTLD_LAZY);
        dlsym_error = dlerror();
        if(dlsym_error != NULL) {
            debugMsg << name << "# " << string(dlsym_error);
            stMsg->stateOut(debugMsg);
            return 0;
        }

        if(libHandle == NULL) {
            debugMsg << name << "# " << "shared lib not opened";
            stMsg->stateOut(debugMsg);
            return 0;
        }
        else {
            sharedLibHandle[modeName].first = libHandle;
            sharedLibHandle[modeName].second = 1;
            debugMsg << name << "# " << "shared lib opened";
            stMsg->stateOut(debugMsg);
        }
    }
    else {
        sharedLibHandle[modeName].second++;
    }

    pCreate createModule = (pCreate) dlsym(sharedLibHandle[modeName].first, "create");
    dlsym_error = dlerror();
    if(dlsym_error != NULL) {
        sharedLibHandle[modeName].second--;
        if(sharedLibHandle[modeName].second == 0)
            sharedLibHandle.erase(modeName);
        return 0;
    }

    smBase* pModule = (*createModule)(devName);
    if(pModule == NULL)
        return 0;

    //dev2libMap[devName] = modeName;
    pModule->init(stMsg, cfgInfo, dataPool, &theFrame->getModuleList());
    theFrame->addMode(devName, pModule);

    return 1;
}

int frameEngine::unloadSharedModule(const string& devName) {

    char* dlsym_error;

    string modeName = dev2libMap[devName];
    string libPath = libDir+"/lib"+modeName+".so";
    pDestroy destroyModule = (pDestroy) dlsym(sharedLibHandle[modeName].first, "destroy");
    dlsym_error = dlerror();
    if(dlsym_error != NULL) {
        debugMsg << name << "# " << string(dlsym_error);
        stMsg->stateOut(debugMsg);
        return 0;
    }

    smBase* pModule;
    pModule = theFrame->eraseMode(devName);
    if(pModule == NULL) {
        debugMsg << name << "# " << "shared lib not loaded in list";
        stMsg->stateOut(debugMsg);
        return 0;
    }
    (*destroyModule)(pModule);

    dev2libMap.erase(devName);

    sharedLibHandle[modeName].second--;
    if(sharedLibHandle[modeName].second == 0) {
        dlclose(sharedLibHandle[modeName].first);
        sharedLibHandle.erase(modeName);
    }
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new frameEngine(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
