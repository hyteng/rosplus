#ifndef configSet_h
#define configSet_h

#include <string>
#include <map>
#include <stdint.h>

class configSet {

    public:
        configSet();
        ~configSet();

        typedef enum {
            NOTYPE=-1,UINT, ULONG, DOUBLE, STRING
        } infoType;

        typedef struct {
            uint32_t i;
            uint64_t l;
            double d;
            std::string s;
        } infoValue;

        typedef struct {
            infoType type;
            infoValue value;
        } infoNode;

        int infoCreate();
        int infoRelease();
        int infoAdd(const std::string& name, const infoType& type);
        int infoGetType(const std::string& name, infoType& type);
        int infoGetValue(const std::string& name, void* value);
        int infoModify(const std::string& name, const void* value);
        int infoExist(const std::string& name);
        int infoDelete(const std::string& name);
        int infoClear();

        int infoSetUint(const std::string& name, const uint32_t& value);
        int infoSetUlong(const std::string& name, const uint64_t& value);
        int infoSetDouble(const std::string& name, const double& value);
        int infoSetString(const std::string& name, const std::string& value);

        int infoGetUint(const std::string& name, uint32_t& value);
        int infoGetUlong(const std::string& name, uint64_t& value);
        int infoGetDouble(const std::string& name, double& value);
        int infoGetString(const std::string& name, std::string& value);

        int readConfig(const std::string& cfg);

    private:
        /* data */
        bool isCreated;
        std::map<std::string, infoNode> *pInfoMap;
};

#endif
