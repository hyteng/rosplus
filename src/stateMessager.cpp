#include "stateMessager.h"

using std::string;
using std::cout;
using std::endl;

stateMessager::stateMessager() {    
}

stateMessager::~stateMessager() {
}

int stateMessager::stateOut(const int& stKey, const string& stMsg) {

    cout << "Key level " << stKey << ", " << stMsg << endl;
    return 1;
}
