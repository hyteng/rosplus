TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = ros
# ros.dbd will be created and installed
DBD += ros.dbd

# ros.dbd will be made up from these files:
ros_DBD += base.dbd
ros_DBD += calcSupport.dbd
ros_DBD += stream.dbd
ros_DBD += streamApp.dbd
ros_DBD += streamSynApps.dbd

# Include dbd files from all support applications:
#ros_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#ros_LIBS += xxx

# ros_registerRecordDeviceDriver.cpp derives from ros.dbd
ros_SRCS += ros_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
ros_SRCS_DEFAULT += rosMain.cpp
ros_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#ros_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
ros_LIBS += $(EPICS_BASE_IOC_LIBS)
ros_LIBS += calc
ros_LIBS += seq
ros_LIBS += asyn
ros_LIBS += stream
ros_LIBS += streamSynApps

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

