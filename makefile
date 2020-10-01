TARGET_SRC=src/main.cpp src/common.cpp src/log.cpp src/BackwardCalcor.cpp
TARGET_OBJ=$(TARGET_SRC:.cpp=.o)
TARGET_DEP=$(TARGET_OBJ:.o=.d)
TARGET=bin/24GamePro

TARGET_OPT=-D_LINUX

ifeq ($(F_DBG),on)
  DEBUG_OPT=-D_DEBUG -g
else
  DEBUG_OPT=-O2
endif

CC=g++ -fpermissive
AR=ar
ARFLAG=-cr
INCLUDE=-Isrc

ifeq ($(F_BIT32),on)
  CFLAG=$(TARGET_OPT) $(DEBUG_OPT) $(INCLUDE) -MMD -Wall -m32
else
  CFLAG=$(TARGET_OPT) -D_LINUX64 $(DEBUG_OPT) $(INCLUDE) -MMD -Wall
endif

LD_TARGET_FLAG=-lrt

All:$(TARGET)

$(TARGET):$(TARGET_OBJ)
	@mkdir -p bin
	$(CC) $(CFLAG) -o $@ $(TARGET_OBJ) $(LD_TARGET_FLAG) -Wl,-rpath,'$$ORIGIN'

-include */*.d */*/*.d

%.d:%.cpp
	rm -f $(@:.d=.o)

%.o:%.cpp
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -f $(TARGET_OBJ) $(TARGET_DEP) $(TARGET)
