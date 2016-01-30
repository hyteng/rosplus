#define STDCXX_98_HEADERS


#include <sys/socket.h>
#include <snmp_pp/snmp_pp.h>
#include <iostream>
//#define SYSDESCR "1.3.6.1.2.1.1.1.0"    // Object ID for System Descriptor
#define SYSDESCR "1.3.6.1.4.1.19947.1.1.1.0"

using namespace std;
#ifdef SNMP_PP_NAMESPACE                                                        
using namespace Snmp_pp;                                                        
#endif

int main() {

    Snmp::socket_startup(); //socket_startup()静态成员函数，初始化Winsocket的相关函数
    int status;    // return status      
    CTarget ctarget((IpAddress)"192.168.1.100");    // SNMP++ v1 target
    //使用一个Address对象构造一个Ctarget对象
    Vb vb(SYSDESCR);    // SNMP++ Variable Binding
    Pdu pdu;    // SNMP++ PDU

    //-------[ Construct a SNMP++ SNMP Object ]---------------------------------------
    Snmp snmp(status);    // Create a SNMP++ session 创建一个新的snmp对象，
    //第一个参数表示创建状态，0表示成功
    if(status != SNMP_CLASS_SUCCESS) {    // check creation status
        cout << snmp.error_msg(status);    // if fail, print error string
        return 1; 
    }

    /*
    //-------[ Invoke a SNMP++ Get ]-------------------------------------------------------
    //invoke的意思是requests help or protection(from a deity)
    pdu += vb;    // add the variable binding
    if((status = snmp.get(pdu, ctarget)) != SNMP_CLASS_SUCCESS)
        cout << snmp.error_msg(status);
    else {
        pdu.get_vb(vb,0);    // extract the variable binding
        int v0;
        cout << vb.get_value(v0) << endl;
        cout << "System Descriptor = "<< vb.get_printable_value() << ". " << v0 << endl;
    }    //print out
    */

    vb.set_oid("1.3.6.1.4.1.19947.1");// get next starting seed
    pdu += vb;// add vb to the pdu

    string oidLevel;
    int len = std::string("1.3.6.1.4.1.19947.1.7.6.0").length();
    //cout << len << ", " << std::string("1.3.6.1.4.1.19947.1.7.6.0").substr(0,len) << endl;
    status = SNMP_CLASS_SUCCESS;
    while(status == SNMP_CLASS_SUCCESS) {
        if((status = snmp.get_next(pdu, ctarget)) == SNMP_CLASS_SUCCESS) {
            pdu.get_vb(vb,0);                               // extract the vb
            oidLevel = std::string(vb.get_printable_oid());
            if(oidLevel.length() >= len)
                if(oidLevel.substr(0,len)=="1.3.6.1.4.1.19947.1.7.6.0")
                    break;
            cout << "Mib Object = " << vb.get_printable_oid() << endl;
            cout << "Mib Value = " << vb.get_printable_value() << endl;
            pdu.set_vb(vb,0);                               // use last vb as the next one
        }
        else
            cout << "SNMP++ Error = " << snmp.error_msg(status);
    }

    //Snmp::socket_cleanup(); //socket_cleanup()静态成员函数，释放被上面函数初始化的winsocket的资源

    return 1;
}
