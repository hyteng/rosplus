####################################################
# Generic makefile - 万能Makefile, 没错老子就是抄来用的
# for compiling and linking C++ projects on Linux 
# Author: George Foot  Modified:Jackie Lee	Current User: tenghy
####################################################
### Customising
#
# Adjust the following if necessary; EXECUTABLE is the target
# executable's filename, and LIBS is a list of libraries to link inc
# (e.g. alleg, stdcx, iostr, etc). You can override these on make's
# command line of course, if you prefer to do it that way.
#
#
EXECUTABLE := 	# 可执行文件名
LIBDIR := ../universeII/vmelib	# 静态库目录
LIBS :=        	# 静态库文件名
INCLUDES := ../universeII/vmelib    # 头文件目录
SRCDIR:=        # 除了当前目录外，其他的源代码文件目录
#
# # Now alter any implicit rules' variables if you like, e.g.:

CC:=g++
CFLAGS := -g -Wall -std=c++11 -pthread -fPIC
CPPFLAGS := $(CFLAGS)
CPPFLAGS += $(addprefix -I,$(INCLUDES))
CPPFLAGS += -MMD
LINKOPT:= -pthread -lstdc++ -Wall -fPIC
#
# # The next bit checks to see whether rm is in your djgpp bin
# # directory; if not it uses del instead, but this can cause (harmless)
# # `File not found' error messages. If you are not using DOS at all,
# # set the variable to something which will unquestioningly remove
# # files.
#

RM-F := rm -f


# # You shouldn't need to change anything below this point.
#
SRCS := $(wildcard *.cpp) $(wildcard $(addsuffix /*.cpp, $(SRCDIR)))
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))

EXECUTABLE := $(patsubst %.cpp,%.so,$(SRCS))
 
.PHONY : all deps objs clean veryclean rebuild info

all: $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) *.o
	@$(RM-F) *.d
	@$(RM-F) *.so
veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean all
ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif
-include $(DEPS)

daq.so: daq.o
	$(CC)  $(LINKOPT) -shared -o $@ $< $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
	mv daq.so ../lib/
dataPacker.so: dataPacker.o
	$(CC)  $(LINKOPT) -shared -o $@ $< $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
	mv dataPacker.so ../lib/
vme.so: vme.o
	$(CC)  $(LINKOPT) -shared -o $@ $< $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS)) -Wl,-rpath=../universeII/vmelib -lvmelib
	mv vme.so ../lib/

adc1785.so: adc1785.o
	$(CC)  $(LINKOPT) -shared -o $@ $< $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS)) -Wl,-rpath=../universeII/vmelib -lvmelib
	mv adc1785.so ../lib/

mqdc32.so: mqdc32.o
	$(CC)  $(LINKOPT) -shared -o $@ $< $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS)) -Wl,-rpath=../universeII/vmelib -lvmelib
	mv mqdc32.so ../lib/

#$(EXECUTABLE) : $(OBJS)
#	$(CC)  $(LINKOPT) -shared -o $(EXECUTABLE) $(OBJS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
#	mv $(EXECUTABLE) ../lib/

info:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(MISSING_DEPS)
	@echo $(MISSING_DEPS_SOURCES)


