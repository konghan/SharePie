
CROSS = arm-none-linux-gnueabi-
CROSS_SYSPATH = /opt/linux-arm-system
CROSS_INC = $(CROSS_SYSPATH)/include
CROSS_LD = $(CROSS_SYSPATH)/lib

CROSS =
CORSS_SYSPATH =
CROSS_INC = .
CROSS_LD = .

CC	= $(CROSS)gcc
CFLAGS	= -Wall -g -I. -I$(CROSS_INC) -I../common
#CFLAGS += -DSQLITE_OMIT_LOAD_EXTENSION

LDFLAGS = -static -L$(CROSS_LD)

STRIP =$(CROSS)strip

TARGET = SharePieCli

#objs = tlv.o array.o hashset.o
objs += md5.o cmp.o spie_mgmt.o osapi.o spie_stor.o fixbuffer.o fixarray.o
objs += cli-trans.o cli-mgmt.o cli-stor.o session.o main-mgmt.o main-stor.o

tlv-objs = tlv-test.o tlv.o

test-objs = $(tlv-objs)

vpath %.c ../common


%.o:%.c
	-$(CC) $(CFLAGS) -c -o $@ $<


test:$(objs) $(lib-objs) test-mgmt.o test-stor.o
	$(CC) -Wall -o TestMgmt test-mgmt.o $(objs) $(lib-objs) $(LDFLAGS)
	$(CC) -Wall -o TestStor test-stor.o $(objs) $(lib-objs) $(LDFLAGS)


all:$(objs) $(lib-objs) main.o
	$(CC) -Wall -o $(TARGET) main.o $(objs) $(lib-objs) $(LDFLAGS)
#	$(STRIP) -s $(TARGET)


clean:
	rm -f $(objs) $(TARGET) main.o test-mgmt.o test-stor.o TestMgmt TestStor

