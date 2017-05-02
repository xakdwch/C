TARGET = libzkthread.so
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

CPP = g++
INC = zkthread.h
LIBS = -lpthread
LIBPATH = 
CFLAGS = -g -Wall -fPIC
CLINKFLAGS = -g -Wall -fPIC -shared

all:$(TARGET)
.PHONY:all

$(TARGET):$(OBJS)
	$(CPP) $< $(LIBS) $(LIBPATH) $(CLINKFLAGS) -o $@
	cp $(INC) $(ROOT_DIR)/include/ -f
	cp $(TARGET) $(ROOT_DIR)/lib/ -f

$(OBJS):%.o:%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f $(OBJS) $(TARGET)

