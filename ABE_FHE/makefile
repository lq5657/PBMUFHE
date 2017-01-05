objects = abe_core.o ABE_mkey.o ABE_FHE.o Ctxt.o matrix_fhe.o timing.o
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags glib-2.0`
CCOMPILE = $(CC) $(CPPFLAGS) $(CFLAGS) -c
CXX = g++
CXXFLAGS = -Wall -g `pkg-config --cflags glib-2.0`
CXXCOMPILE = $(CXX) $(CXXFLAGS) -c
LDFLAGS = `pkg-config --libs glib-2.0` -L/usr/local/glib/lib -lglib-2.0 -lgthread-2.0 -L/usr/local/lib -lntl -lpbc -lgmp -L/usr/lib/i386-linux-gnu -lcrypto -lpthread
LINKCXX = $(CXX) $(LDFLAGS)

LIB = libabefhe.a
AR = ar
$(LIB):$(objects)
	 $(AR) -r $(LIB) $(objects)

%.o:%.c  
	$(CCOMPILE) $< -o $@  
%o:%.cpp  
	$(CXXCOMPILE) $< -o $@  
.PHONY:clean
clean:  
	rm -f $(objects) $(*.a) core* tmp*
