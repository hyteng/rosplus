#include "ldmTA.h"
#include <algorithm>
#include <string.h>
#include <unistd.h>

using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::hex;
using std::dec;


static map<string, vector<string> > ldmTA_ctrl2conf;
static map<string, int> ldmTA_conf2reg;
static map<string, uintptr_t> ldmTA_conf2mask;
static vector<uintptr_t> ldmTA_regAddr;
static vector<int> ldmTA_regRWIdx;
/*
static int setCtrl() {
    ldmTA_ctrl2conf.clear();
    for(int i=0,j=0; i<ctrlSize; i++, j++) {
        ldmTA_ctrl2conf[ctrl1785[i]] = vector<string>(0);
        ldmTA_ctrl2conf[ctrl1785[i]].push_back(confName1785[j]);
        
        if(i==4 || i==31 || i==70) {
            j++;
            ldmTA_ctrl2conf[ctrl1785[i]].push_back(confName1785[j]);
        }

        if(i>=ctrlReg && i<ctrlReg+ctrlBitSet1)
            ldmTA_ctrl2conf[ctrl1785[i]].push_back(confName1785[j+ctrlBitSet1]);
        if(i==ctrlReg+ctrlBitSet1-1)
            j+=ctrlBitSet1;

        if(i>=ctrlReg+ctrlBitSet1+ctrlCtrl1 && i<ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2)
            ldmTA_ctrl2conf[ctrl1785[i]].push_back(confName1785[j+ctrlBitSet2]);
        if(i==ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2-1)
            j+=ctrlBitSet2;
    }

    ldmTA_conf2reg.clear();
    ldmTA_conf2mask.clear();
    for(int i=0, j=0; i<confSize; i++,j++) {
        if(i>=confReg && i<confReg+confBitSet1)
            j = confReg;
        if(i>=confReg+confBitSet1 && i<confReg+confBitSet1+confBitClear1)
            j = confReg+1;
        if(i>=confReg+confBitSet1+confBitClear1 && i<confReg+confBitSet1+confBitClear1+confCtrl1)
            j = confReg+2;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2)
            j = confReg+3;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2)
            j = confReg+4;
        if(i==confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2)
            j = confReg+5;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1)
            j = confReg+6;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1+confStatus2)
            j = confReg+7;

        ldmTA_conf2reg[confName1785[i]] = j;
        ldmTA_conf2mask[confName1785[i]] = (uintptr_t)(&confMask1785[i]);
    }

    ldmTA_regAddr.clear();
    ldmTA_regRWIdx.clear();
    for(int i=0; i<regSize; i++) {
        ldmTA_regAddr.push_back((uintptr_t)(&regAddr1785[i]));
        int rw = 0;
        if(i>=regRW && i<regRW+regWO)
            rw = 2;
        if(i>=regRW+regWO+regBitSet)
            rw = 1;
        ldmTA_regRWIdx.push_back(rw);
    }

    return 1;
}

static int dummy = setCtrl();
*/
