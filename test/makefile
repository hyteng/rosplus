CC          = g++ -Wall -g -O2 -std=c++11
LN	    = g++ -Wall -lstdc++ -lsnmp++
INCLUDE	    = /user/local/include
LIBS	    = -L/usr/local/lib
PROGS       = snmp

%.o: %.cpp
	$(CC) -I $(INCLUDE) -c $<

all: $(PROGS)

snmp: snmp.o
	$(LN) snmp.o $(LIBS) -o snmp

clean:
	rm -f *.o $(PROGS)
