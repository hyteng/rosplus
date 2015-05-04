CC          = g++ -Wall -g -O2
AR	    = ar rs
SO_VERSION  = 0.9.4

vmelib: vmelib.o
	@echo Generating static link library
	$(AR) libvmelib.a vmelib.o
	@echo

	@echo Generating dynamic link library
	$(CC) -shared -Wl,-soname,libvmelib.so.0 \
	-o libvmelib.so.$(SO_VERSION) vmelib.o -lc

vmelib.o: vmelib.cpp
	$(CC) -fPIC -I../driver-3.10 vmelib.cpp -c -o vmelib.o

install:
	mv libvmelib.so.$(SO_VERSION) /usr/local/lib
	ln -s /usr/local/lib/libvmelib.so.$(SO_VERSION) /usr/local/lib/libvmelib.so
	ldconfig

clean:
	rm -f *.o libvmelib.a libvmelib.so.$(SO_VERSION)
	rm -rf /usr/local/lib/libvmelib.so.*
	rm -rf /usr/local/lib/libvmelib.so
