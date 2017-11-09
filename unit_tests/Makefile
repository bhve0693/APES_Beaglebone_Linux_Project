CC=arm-linux-gnueabihf-gcc

INCLUDES  = -I../inc 
INCLUDES += -I../Unity/src

CFLAGS= -g -O0 $(INCLUDES)



LIBS= -pthread -lrt -lm 


SRCS1 = test_temperature.c  ../Unity/src/unity.c ../src/i2c-temp.c ../src/i2c-drive.c ../src/i2c-light.c 
SRCS2 = test_light.c  ../Unity/src/unity.c ../src/i2c-temp.c ../src/i2c-drive.c ../src/i2c-light.c
OBJECTS = $(patsubst %.c,%.o,$(SRCS))

all: temptest templight

temptest:$(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS1) $(LIBS) -o temptest


templight:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS2) $(LIBS) -o templight

.PHONY: clean

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f temptest templight

