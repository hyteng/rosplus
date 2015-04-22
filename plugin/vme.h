#ifndef vme_h
#define vme_h

#include <../src/smBase.h>
#include <stdint.h>

class vme :public smBase {
    public:
        vme(const std::string& n);
        ~vme();

    private:
        uint32_t dmaBase;
};
#endif
