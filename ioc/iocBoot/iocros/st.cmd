#!../../bin/linux-x86_64/ros

## You may have to change ros to something else
## everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/ros.dbd"
ros_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadTemplate "db/user.substitutions"
dbLoadRecords "db/dbSubExample.db", "user=hyteng"

dbLoadRecords("$(TOP)/db/ros.db", "P=ABC:,Q=TC1,PORT=rosIP,ADDR=0")
drvAsynIPPortConfigure("rosIP", "127.0.0.1:4002", 0, 0, 0)
asynOctetSetInputEos("rosIP",0,"\0")
asynOctetSetOutputEos("rosIP",0,"\0")

epicsEnvSet ("STREAM_PROTOCOL_PATH", "$(TOP)/rosApp/Db")

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncExample, "user=hyteng"
