CXX = g++
CC = gcc

HEADER := $(shell cd ../ && pwd)

INCLUDES = -I$(JAVA_HOME)/include \
		-I$(JAVA_HOME)/include/linux \
		-I$(JAVA_HOME)/jre/lib/amd64/server/ \
		-I${HEADER}

#.PHONY: print_vars

#print_vars:
#	@echo $(JAVA_HOME)
#	@echo ${CURDIR}
#	@echo $(HEADER)

CLIBS =	-L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm -lpthread

OBJS = convert.o \
	   crdd.o \
	   cpairrdd.o \
	   csparkcontext.o \
	   Class.o \
	   mapping.o \
	   storageLevel.o
	  

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
