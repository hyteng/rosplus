. /etc/init.d/functions
EPICS_BASE=/opt/egcs/epics/base
EPICS_ROOT=/opt/egcs/epics
EPICS_HOST_ARCH=linux-x86_64

EGCS_EPICS_CONF="/opt/egcs/iocs/conf"
EGCS_EPICS_APPS="/home/csns/vme/rosplus/"
EGCS_EPICS_ROOT="/opt/egcs/iocs"
EGCS_EPICS_VAR="/home/csns/vme/rosplus/ioc/var"

LOGFILES=$EGCS_EPICS_ROOT/log
UNIT_NAME="ioc"
IOC_NAME="iocros"

.  $EGCS_EPICS_ROOT/script/services/ioc-startup

