# -*- makefile -*-
# 
#	makefile for writing configurations into a file
#
# @author Yvan Castilloux
# @author David Moss

include ${IOTSDK}/c/support/make/Makefile.include

# What is the main file we want to compile
TARGET = rtoaagent

# Which file(s) are we using
SOURCES_C = ${TARGET}.c

SOURCES_C += ./control/rtoacontrol.c
SOURCES_C += ./manager/rtoamanager.c
SOURCES_C += ./discovery/rtoadiscovery.c
SOURCES_C += ./heartbeat/rtoaheartbeat.c
SOURCES_C += ./measure/rtoameasure.c
SOURCES_C += ${IOTSDK}/c/iot/client/clientsocket.c

# Which test(s) are we trying to run
SOURCES_CPP = 

# Where is the IOT include directory
CFLAGS += -I${IOTSDK}/c/include

# What directories should we include
CFLAGS += -I./
CFLAGS += -I./control
CFLAGS += -I./manager
CFLAGS += -I./discovery
CFLAGS += -I./heartbeat
CFLAGS += -I./measure
CFLAGS += -I${IOTSDK}/c/apps/proxyserver
CFLAGS += -I${IOTSDK}/c/iot/client
CFLAGS += -I${IOTSDK}/c/iot/proxy 

# What 3rd party library headerse should we include. 
# Version information is pulled from support/make/Makefile.include
CFLAGS += -I${IOTSDK}/c/lib/3rdparty/${LIBXML2_VERSION}/include
CFLAGS += -I${IOTSDK}/c/lib/3rdparty/${LIBCURL_VERSION}/include
CFLAGS += -I${IOTSDK}/c/lib/3rdparty/${OPENSSL_VERSION}/include
CFLAGS += -I${IOTSDK}/c/lib/3rdparty/${CJSON_VERSION}

OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)

LDEXTRA += -L${IOTSDK}/c/lib -liotxml -lhttpcomm -lpipecomm -lxml2 -lconfigio -lcJSON -lcurl -lpthread -lm

# Note the path to the cJSON .so library in our IOTSDK below
LDFLAGS += -Wl,-rpath,${IOTSDK}/c/lib

CFLAGS += -Wall


.bin:
	@mkdir -p ./bin
  
.c.o:
	@$(CC) -c $(CFLAGS) $(LDFLAGS) -o ./bin/$(shell basename $@) $<
	
.cpp.o:
	@mkdir ./bin
	@$(CPP) -c $(CFLAGS) $(LDFLAGS) -o ./bin/$(shell basename $@) $<

test: clean $(TARGET)

clean:
	@rm -rf ./*.o $(TARGET) ./bin
	
$(TARGET): .bin $(OBJECTS_C) $(OBJECTS_CPP)
	@$(CC) ${CFLAGS} $(LDFLAGS) -o ./bin/$(shell basename $@) ./bin/*.o $(LDEXTRA)


	