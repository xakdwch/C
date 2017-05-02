TARGET = publisher subscriber
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

CPP = g++
LIBS = -lzookeeper_mt -lzkbase -lzkthread
LIBPATH = -L$(ROOT_DIR)/lib/ -L$(ROOT_DIR)/lib/zookeeper/
CFLAGS = -g -Wall -fPIC -I$(ROOT_DIR)/include/zookeeper/ -I$(ROOT_DIR)/include/
CLINKFLAGS = -g -Wall -fPIC

all:$(TARGET)
.PHONY:all

publisher:publisher.o
	$(CPP) $^ $(LIBS) $(LIBPATH) $(CLINKFLAGS) -o $@

subscriber:subscriber.o
	$(CPP) $^ $(LIBS) $(LIBPATH) $(CLINKFLAGS) -o $@

$(OBJS):%.o:%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f $(OBJS) $(TARGET)
