#include "configSet.h"
#include <sstream>
#include <fstream>
#include <iostream>

using std::string;
using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

configSet::configSet() {
    isCreated = false;
}

configSet::~configSet() {
}

int configSet::infoCreate() {
    if(isCreated == true)
        return -1;

    pInfoMap = new std::map<string, infoNode>;
    isCreated = true;

    return 1;
}

int configSet::infoRelease() {

    if(isCreated == true) {
        pInfoMap->clear();
        delete pInfoMap;
    }
    isCreated = false;

    return 1;
}

int configSet::infoAdd(const string& name, const infoType& type) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        return 0;
    }
    else {
        infoNode node;
        node.type = type;
        (*pInfoMap)[name] = node;
    }

    return 1;
}

int configSet::infoGetType(const string& name, infoType& type) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        type = (*pInfoMap)[name].type;
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoGetValue(const string& name, void* value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        switch((*pInfoMap)[name].type) {
            case UINT:
                (*(uint32_t*)value) = (*pInfoMap)[name].value.i;
                break;
            case ULONG:
                (*(uint64_t*)value) = (*pInfoMap)[name].value.l;
                break;
            case DOUBLE:
                (*(double*)value) = (*pInfoMap)[name].value.d;
                break;
            case STRING:
                (*(string*)value) = (*pInfoMap)[name].value.s;
                break;
            default:
                return 0;
        }
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoModify(const string& name, const void* value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        switch((*pInfoMap)[name].type) {
            case UINT:
                (*pInfoMap)[name].value.i = (*(const uint32_t*)value);
                break;
            case ULONG:
                (*pInfoMap)[name].value.l = (*(const uint64_t*)value);
                break;
            case DOUBLE:
                (*pInfoMap)[name].value.d = (*(const double*)value);
                break;
            case STRING:
                (*pInfoMap)[name].value.s = (*(const string*)value);
                break;
            default:
                return 0;
        }
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoExist(const string& name) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end())
        return 1;
    else
        return 0;
}

int configSet::infoDelete(const string& name) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end())
        pInfoMap->erase(pInfoMap->find(name));

    return 1;
}

int configSet::infoClear() {
    if(isCreated == false)
        return -1;

    pInfoMap->clear();
    return 1;
}

int configSet::infoSetUint(const string& name, const uint32_t& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == UINT)
            (*pInfoMap)[name].value.i = value;
        else
            return 0;
    }
    else {
        return 0;
    }

    cout << "infoSet" << name << ", " << value << endl;
    return 1;
}

int configSet::infoSetUlong(const string& name, const uint64_t& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == ULONG)
            (*pInfoMap)[name].value.l = value;
        else
            return 0;
    }
    else {
        return 0;
    }

    cout << "infoSet" << name << ", " << value << endl;
    return 1;
}

int configSet::infoSetDouble(const string& name, const double& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == DOUBLE)
            (*pInfoMap)[name].value.d = value;
        else
            return 0;
    }
    else {
        return 0;
    }

    cout << "infoSet" << name << ", " << value << endl;
    return 1;
}

int configSet::infoSetString(const string& name, const string& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == STRING)
            (*pInfoMap)[name].value.s = value;
        else
            return 0;
    }
    else {
        return 0;
    }

    cout << "infoSet" << name << ", " << value << endl;
    return 1;

}

int configSet::infoGetUint(const string& name, uint32_t& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == UINT)
            value = (*pInfoMap)[name].value.i;
        else
            return 0;
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoGetUlong(const string& name, uint64_t& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == ULONG)
            value = (*pInfoMap)[name].value.l;
        else
            return 0;
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoGetDouble(const string& name, double& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == DOUBLE)
            value = (*pInfoMap)[name].value.d;
        else
            return 0;
    }
    else {
        return 0;
    }

    return 1;
}

int configSet::infoGetString(const string& name, string& value) {
    if(isCreated == false)
        return -1;

    if(pInfoMap->find(name) != pInfoMap->end()) {
        if((*pInfoMap)[name].type == STRING)
            value = (*pInfoMap)[name].value.s;
        else
            return 0;
    }
    else {
        return 0;
    }

    return 1;
}


int configSet::readConfig(const string& cfg) {

    string cfgLine, s[3];
    ifstream cfgFile(cfg);
    while(getline(cfgFile, cfgLine)) {
        stringstream ss(cfgLine);
        getline(ss, s[0], '\t');
        getline(ss, s[1], '\t');
        getline(ss, s[2], '\t');

        int bit = infoExist(s[1]);
        if(bit != 0) {
            cout << "conflict! type: " << s[0] << ", name: " << s[1] << ", value: " << s[2] << endl;
            continue;
        }

        int t;
        stringstream sType(s[0]), sValue(s[2]);
        sType >> t;
        configSet::infoType type = configSet::infoType(t);
        infoAdd(s[1], type);
        cout << "type: " << t << ", name " << s[1] << endl;

        if(type == configSet::UINT) {
            uint32_t v;
            if(s[2].substr(0,2) == "0x" || s[2].substr(0,2) == "0X")
                sValue >> std::hex >> v;
            else
                sValue >> v;
            infoSetUint(s[1], v);
        }
        if(type == configSet::ULONG) {
            uint64_t v;
            if(s[2].substr(0,2) == "0x" || s[2].substr(0,2) == "0X")
                sValue >> std::hex >> v;
            else
                sValue >> v;
            infoSetUlong(s[1], v);
        }
        if(type == configSet::DOUBLE) {
            double v;
            sValue >> v;
            infoSetDouble(s[1], v);
        }
        if(type == configSet::STRING) {
            string v;
            sValue >> v;
            infoSetString(s[1], v);
        }
    }
    cfgFile.close();
    return 1;
}

