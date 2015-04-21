#ifndef vme_h
#define vme_h

#include <../src/smBase.h>

class vme :public smBase {
    public:
        vme();
        ~vme();

    private:
        unsigned int base;
        unsigned int length;
        unsigned int image;
};
#endif
