TARGET = zkfifo
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

CC = g++
LIBS = -lzookeeper_mt -lzkbase -lzkthread
LIBPATH = -L$(ROOT_DIR)/lib/
CFLAGS = -g -Wall -fPIC -I$(ROOT_DIR)/include/zookeeper/ -I$(ROOT_DIR)/include/
CLINKFLAGS = -g -Wall -fPIC

all:$(TARGET)
.PHONY:all

$(TARGET):$(OBJS)
	$(CC) $^ $(LIBS) $(LIBPATH) $(CLINKFLAGS) -o $@

$(OBJS):%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f $(OBJS) $(TARGET)
