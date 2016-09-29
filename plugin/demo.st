/*************************************************************************\
  This file is distributed subject to a Software License Agreement found
  in the file LICENSE that is included with this distribution.
  \*************************************************************************/
/* Demo EPICS sequence */

#define PRINTF errlogPrintf

/* message system choice */
#ifdef KTL

#define PVSYS    "pvsys=ktl"

#define LIGHT    "{prefix}.light"
#define LIGHTON  "{prefix}.lighton"
#define LIGHTOFF "{prefix}.lightoff"
#define VOLTAGE  "{prefix}.voltage"
#define LOLIMIT  "{prefix}.lolimit"
#define HILIMIT  "{prefix}.hilimit"

#else

#define PVSYS    "pvsys=ca"

#define LIGHT    "{prefix}:light"
#define LIGHTON  "{prefix}:lightOn"
#define LIGHTOFF "{prefix}:lightOff"
#define VOLTAGE  "{prefix}:voltage"
#define LOLIMIT  "{prefix}:loLimit"
#define HILIMIT  "{prefix}:hiLimit"

#endif

program demo( PVSYS )

    %%#include "errlog.h"
    %%#include "math.h"
    /* options */
    option  +r;

    /* control variables and transition points for light control */
    float x[481][1106], y[481][1106], dxx, dyx, dxy, dyy;
    int sizeX, sizeY;
//    int test=1; /* test type*/

    /* for pmt */
    float d1=62.,d2=61.,d3=61.,d4=60.;
    float si[8]={0., 62., 123., 184., 244., 305., 366., 428.};
    float sj[8]={0., 62., 123., 184., 244., 305., 366., 428.};
    int i,j,idx;

/* for SD */
int testIdxX[2] = {0,223};
int testIdxY[2] = {0,95};

int     count;
assign  count    to "Count2D";
monitor count;

int index;
assign index  to "IndexPV";
monitor index;

int test;
assign test  to "TestPV";
monitor test;

double     x1;
assign  x1    to "2DX1";
monitor x1;

double     x2;
assign  x2    to "2DX2";
monitor x2;

double     x3;
assign  x3    to "2DX3";
monitor x3;

double     x4;
assign  x4    to "2DX4";
monitor x4;

double     y1;
assign  y1    to "2DY1";
monitor y1;

double     y2;
assign  y2    to "2DY2";
monitor y2;

double     y3;
assign  y3    to "2DY3";
monitor y3;

double     y4;
assign  y4    to "2DY4";
monitor y4;

int    submit;
assign  submit to "submit2D";
monitor submit;

int    abort;
assign  abort to "Abort2D";
monitor abort;

int    next;
assign  next to "NextPoint";
monitor next;

float   xtarget;
assign  xtarget to "XTarget";
monitor xtarget;

double  ytarget;
assign  ytarget to "YTarget";
monitor ytarget;

double  xcurrent;
assign  xcurrent to "XCurrent";
monitor xcurrent;

double  ycurrent;
assign  ycurrent to "YCurrent";
monitor ycurrent;

string   cmd;
string   cmd1="all#2##";
assign  cmd to "ABC:ROS:TC1:CTRL";
monitor cmd;

float  delta;
/* light control state-set */
ss RUN2D {
    state INITIAL {
        when(){
            PRINTF("initialized.....\n");
            index=0; pvPut(index,SYNC);
        }state STOP
    }

    state STOP {
        when (submit==1) {
            PRINTF("stop status.\n");
        } state START
    }

    state START {
        when (test==0) {

            sizeX=8;sizeY=8;
            x[0][0]=x1;y[0][0]=y1; x[0][sizeX-1]=x2;y[0][sizeX-1]=y2; x[sizeY-1][0]=x3;y[sizeY-1][0]=y3; x[sizeY-1][sizeX-1]=x4;y[sizeY-1][sizeX-1]=y4;
            dxx=(x2-x1)/428.;dyx=(y2-y1)/428.;dyy=(y3-y1)/428.;dxy=(x3-x1)/428.;
            for(i=0; i<sizeX; i++)
                for(j=0; j<sizeY; j++) {
                    x[j][i]=x1+dxx*si[i]+dxy*sj[j];
                    y[j][i]=y1+dyy*sj[j]+dyx*si[i];
                }

            sleep(1);
            {
                // strcpy(cmd,"all#0##");pvPut(cmd,SYNC); sleep(5);
                // strcpy(cmd,"all#2##");pvPut(cmd,SYNC); sleep(5);
                index=0;
                for(j=0;j<sizeY;j++) {
                    for(i=0;i<sizeX;i++)
                    {
                        count=j*sizeX+i+1;pvPut(count,SYNC);
                        if(count<index)
                            continue;

                        while(next==0){
                            if(abort==1){break;}
                            sleep(2);
                        }
                        if(abort==1){break;}
                        printf("x[%d][%d] is %f;  ",j,i,x[j][i]);
                        printf("y[%d][%d] is %f\n ",j,i,y[j][i]);
                        xtarget=x[j][i]; pvPut(xtarget,SYNC);delta=xtarget-xcurrent;
                        while(delta>3||delta<-3)  {sleep(1);delta=xtarget-xcurrent;}
                        sleep(1);
                        ytarget=y[j][i]; pvPut(ytarget,SYNC);delta=ytarget-ycurrent;
                        while(delta>3||delta<-3)  {sleep(1);delta=ytarget-ycurrent;}
                        sleep(1);
                        printf("move is done: %f, %f\n",xcurrent,ycurrent);
                        strcpy(cmd,"all#4##");pvPut(cmd,SYNC); sleep(10);
                        strcpy(cmd,"all#6##");pvPut(cmd,SYNC); sleep(13);
                        strcpy(cmd,"all#7##");pvPut(cmd,SYNC); sleep(5);
                        strcpy(cmd,"all#5##");pvPut(cmd,SYNC); sleep(1);
                    }
                    if(abort==1){break;}
                }
                abort=0;pvPut(abort,SYNC);
                submit=0;pvPut(submit,SYNC);
                count=0;pvPut(count,SYNC);
            }
        }
        state STOP

        when (test==1) {
            
            sizeX=1106;sizeY=481;
            x[0][0]=x1;y[0][0]=y1; x[0][sizeX-1]=x2;y[0][sizeX-1]=y2; x[sizeY-1][0]=x3;y[sizeY-1][0]=y3; x[sizeY-1][sizeX-1]=x4;y[sizeY-1][sizeX-1]=y4;
            dxx=(x2-x1)/(sizeX-1.);dyx=(y2-y1)/(sizeX-1.);dyy=(y3-y1)/(sizeY-1.);dxy=(x3-x1)/(sizeY-1.);
            for(i=0; i<sizeX; i++)
                for(j=0; j<sizeY; j++) {
                    x[j][i]=x1+dxx*i+dxy*j;
                    y[j][i]=y1+dyy*j+dyx*i;
                }
            sleep(1);
            {
                //strcpy(cmd,"all#0##");pvPut(cmd,SYNC); sleep(5);
                //strcpy(cmd,"all#2##");pvPut(cmd,SYNC); sleep(5);
                for(idx=0;idx<1;idx++) {
                    j=testIdxY[idx];
                    for(i=0;i<sizeX;i++) {
                        while(next==0){
                            if(abort==1){break;}
                            sleep(2);
                        }

                        count=j*sizeX+i+1;pvPut(count,SYNC);
                        if(count<index)
                            continue;

                        if(abort==1){break;}
                        printf("x[%d][%d] is %f;  ",j,i,x[j][i]);
                        printf("y[%d][%d] is %f\n ",j,i,y[j][i]);
                        xtarget=x[j][i]; pvPut(xtarget,SYNC);delta=xtarget-xcurrent;
                        while(delta>3.||delta<-3.)  {sleep(1);delta=xtarget-xcurrent;}
                        sleep(1);
                        ytarget=y[j][i]; pvPut(ytarget,SYNC);delta=ytarget-ycurrent;
                        while(delta>3.||delta<-3.)  {sleep(1);delta=ytarget-ycurrent;}
                        sleep(1);
                        printf("move is done: %f, %f\n",xcurrent,ycurrent);
                        strcpy(cmd,"all#4##");pvPut(cmd,SYNC); sleep(10);
                        strcpy(cmd,"all#6##");pvPut(cmd,SYNC); sleep(13);
                        strcpy(cmd,"all#7##");pvPut(cmd,SYNC); sleep(5);
                        strcpy(cmd,"all#5##");pvPut(cmd,SYNC); sleep(1);
                    }
                    if(abort==1){break;}
                }
                abort=0;pvPut(abort,SYNC);
                submit=0;pvPut(submit,SYNC);
                count=0;pvPut(count,SYNC);
            }
        }
        state STOP
    }
}

