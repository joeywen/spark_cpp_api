CXX = g++
CC = gcc
INCLUDES = -I/home/software/java/jdk1.6.0_37/include \
		-I/home/software/java/jdk1.6.0_37/include/linux \
		-I/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ \
		-I/home/bolei/workspace/spark-0.6.0/c++/

CLIBS =	-L/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ -ljvm -lpthread

OBJS = convert.o \
	   cpairrdd.o \
	   crdd.o \
	   csparkcontext.o \
	   Class.o \
	   mapping.o
	  

TARGET = libsparkAPI.so
CPPFLAGS = -shared -WI
LDLIB = 

$(TARGET): $(OBJS)
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIB) 

$(OBJS) : %.o : %.cpp
	@echo Building "$@"
	$(CXX) -c -fPIC $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS)
