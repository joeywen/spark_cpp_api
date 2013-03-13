CXX = g++
CC = gcc

HEADER := $(shell cd ../../ && pwd)

INCLUDES = -I$(JAVA_HOME)/include \
		-I$(JAVA_HOME)/include/linux \
		-I$(JAVA_HOME)/jre/lib/amd64/server/ \
		-I$(HEADER) \
		-I$(CURDIR)
		
CLIBS =	-L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm
OBJS = spark_api_java_jni_Native.o

TARGET = libsparkJNI.so
CPPFLAGS = -shared -WI
LDLIB = 

$(TARGET): $(OBJS)
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIB) 

$(OBJS) : %.o : %.cpp
	$(CXX) -c -fPIC $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS)
