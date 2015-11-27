CC          = g++ -Wall -g -O2
LN	    = g++ -lstdc++ -Wall -rdynamic -ldl -Wl,-rpath=../vmelib -lvmelib
INCLUDE	    = ../vmelib
LIBS	    = -L ../vmelib -l vmelib 
PROGS       = test_simple test_mmap test_dma test_llist test_mbx adc_dma adc_test adc_dma2 mqdctool

%.o: %.cpp
	$(CC) -I $(INCLUDE) -c $<

all: $(PROGS)

test_simple: test_simple.o
	$(LN) test_simple.o $(LIBS) -o test_simple

test_mmap: test_mmap.o
	$(LN) test_mmap.o $(LIBS) -o test_mmap

test_dma: test_dma.o
	$(LN) test_dma.o $(LIBS) -o test_dma

test_llist: test_llist.o
	$(LN) test_llist.o $(LIBS) -o test_llist

test_mbx: test_mbx.o
	$(LN) test_mbx.o $(LIBS) -o test_mbx

adc_dma: adc_dma.o
	$(LN) adc_dma.o $(LIBS) -o adc_dma

adc_test: adc_test.o
	$(LN) adc_test.o $(LIBS) -o adc_test

adc_dma2: adc_dma2.o
	$(LN) adc_dma2.o $(LIBS) -o adc_dma2

mqdctool: mqdctool.o
	$(LN) mqdctool.o $(LIBS) -o mqdctool

clean:
	rm -f *.o $(PROGS)
