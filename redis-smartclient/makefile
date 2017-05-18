TARGET=smartclient
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CC=g++
INC=
LIBS=-lhiredis -lpthread
LIBPATH=
CFLAGS=-g -Wall -fPIC -I/usr/local/include/hiredis/
CLINKFLAGS=

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(CLINKFLAGS) $(LIBPATH) $(LIBS) -o $@

$(OBJS):%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean

clean:
	rm -f $(TARGET) $(OBJS)
