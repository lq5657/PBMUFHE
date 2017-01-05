objects = common.o policy_lang.o setup.o serialize.o prvGen.o dec.o enc.o compute.o
obj = common.o serialize.o policy_lang.o
obj1 = common.o serialize.o setup.o
obj2 = $(obj) prvGen.o
obj3 = $(obj) dec.o
obj4 = $(obj) enc.o
obj5 = $(obj) compute.o

CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags glib-2.0`
CCOMPILE = $(CC) $(CPPFLAGS) $(CFLAGS) -c
CXX = g++
CXXFLAGS = -Wall -g `pkg-config --cflags glib-2.0`  
CXXCOMPILE = $(CXX) $(CXXFLAGS) -c
LDFLAGS = `pkg-config --libs glib-2.0` -L./ABE_FHE -labefhe -L/usr/local/glib/lib -lglib-2.0 -lgthread-2.0 -L/usr/local/lib -lntl -lpbc -lgmp -L/usr/lib/i386-linux-gnu -lcrypto -lpthread
INCLUDE =  -I./ -I/usr/include -I/usr/glib/include/glib-2.0 -I/usr/local/include      

TARGET = setup prvGen abefheDec abefheEnc abefheCom
all: $(TARGET)

setup : $(obj1)
	$(CXX) $(INCLUDE) -o setup $(obj1) $(LDFLAGS)
prvGen : $(obj2)
	$(CXX) $(INCLUDE) -o prvGen $(obj2) $(LDFLAGS)
abefheDec : $(obj3)
	$(CXX) $(INCLUDE) -o abefheDec $(obj3) $(LDFLAGS)
abefheEnc : $(obj4)
	$(CXX) $(INCLUDE) -o abefheEnc $(obj4) $(LDFLAGS)
abefheCom : $(obj5)
	$(CXX) $(INCLUDE) -o abefheCom $(obj5) $(LDFLAGS)

%.o:%.c  
	    $(CCOMPILE) $< -o $@  
%o:%.cpp  
	    $(CXXCOMPILE) $< -o $@  
.PHONY:clean  
	clean:  
	    rm -f $(objects) core* tmp*

