#define STDCXX_98_HEADERS

#include <snmp_pp/snmp_pp.h>
#include <iostream>
#define SYSDESCR "1.3.6.1.2.1.1.1.0"    // Object ID for System Descriptor

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
    Snmp snmp( status);    // Create a SNMP++ session 创建一个新的snmp对象，
    //第一个参数表示创建状态，0表示成功
    if ( status != SNMP_CLASS_SUCCESS) {    // check creation status
        cout << snmp.error_msg(status);    // if fail, print error string
        return 1; }

    //-------[ Invoke a SNMP++ Get ]-------------------------------------------------------
    //invoke的意思是requests help or protection(from a deity)
    pdu += vb;    // add the variable binding
    if ((status = snmp.get(pdu, ctarget)) != SNMP_CLASS_SUCCESS)
        cout << snmp.error_msg(status);
    else {
        pdu.get_vb(vb,0);    // extract the variable binding
        cout << "System Descriptor = "<< vb.get_printable_value(); }    // print out
    Snmp::socket_cleanup(); //socket_cleanup()静态成员函数，释放被上面函数初始化的winsocket的资源

    return 1;
}
