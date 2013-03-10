CXX = g++
CC = gcc

INCLUDES = -I/home/software/java/jdk1.6.0_37/include \
		-I/home/software/java/jdk1.6.0_37/include/linux \
		-I/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ \
        -I/home/bolei/workspace/spark-0.6.0/c++/

CLIBS =	-L/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ -ljvm -lsparkAPI
#		-L/home/bolei/workspace/spark-0.6.0/c++/spark/ -lapi

TARGET_OBJS = wordcount.o

EXEC_OBJ = main.o
TARGET = libClassLoader.so

EXEC = client

CPPFLAGS = -shared
LDLIB = 

$(TARGET): $(TARGET_OBJS)
	@echo building so...
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(TARGET_OBJS) $(CLIBS) -lpthread 

$(EXEC): $(EXEC_OBJ)
	@echo buidling...
	$(CXX) -o $(EXEC) $(EXEC_OBJ) $(CLIBS)

$(TARGET_OBJS) : wordcount.cpp
	@echo Building "$@"
	$(CXX) -c -fPIC $(INCLUDES) wordcount.cpp

$(EXEC_OBJ): main.cpp
	$(CXX) -c $(INCLUDES) main.cpp

.PHONY: clean
clean:
	rm -rf *.o
