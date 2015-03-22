/*=============================================================================
FileName: stateMessager.h
Desc: message handler for states
Author: Haiyun.teng
Email: Haiyun.teng@gmail.com
Version: 0.0.1
=============================================================================*/ 

#ifndef stateMessager_h
#define stateMessager_h
    
#include <iostream>
#include <string>

/*
#define DEBUG_LEVEL 2
#define DEBUG(level, msg) \
        if(level<DEBUG_LEVEL) {\
            std::cout << "DEBUG_LEVEL " << DEBUG_LEVEL << ": " << msg << endl; \
        }

#define WARNING_LEVEL 2
#define WARNING(level, msg) \
        if(level<WARNING_LEVEL) \
        {printf("WARNING%d: ", level);  \
        printf msg; \    
        printf("\n");   }


#define ERROR_LEVEL 2    
#define ERROR(level, msg) \
        if(level<ERROR_LEVEL)   \
        {printf("ERROR%d: ", level) ;   \
        printf msg; \    
        printf("\n");   }
*/

/*error leval*/          
#define STATUS_LEVEL 0   /*set the flag of status*/
#define SUCC_ERR    0   /*fatal error, must stop running*/
#define FATAL_ERR   1   /*fatal error, must stop running*/
#define COMM_ERR    2   /*common error */
#define WRNG_ERR    3   /*warning*/
#define REST_ERR    4   /*restart daq when error*/


/*error type*/
#define HW_ERROR        0
#define COFIG_ERROR     1
#define NET_ERROR       2
#define PACK_ERROR      3
#define CBLT_ERROR      4
#define SEMPH_ERROR 5
#define MSG_ERROR       6
#define BUFF_ERROR      7

/*status light*/
#define LIGHT_RUNNING 102
#define LIGHT_FINISHED 103
#define LIGHT_IDLE 101
#define LIGHT_ERROR 104

#define PROGRESS(vv)((vv)+1000)

class stateMessager {

    public:
        stateMessager();
        ~stateMessager();
        
        int stateOut(const int& stKey, const std::string& stMsg);
    private:
        /* data */
};


#endif

