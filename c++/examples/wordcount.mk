CXX = g++
CC = gcc

INCLUDES = -I$(JAVA_HOME)/include \
		-I$(JAVA_HOME)/include/linux \
		-I$(JAVA_HOME)/jre/lib/amd64/server/ \
		-I$(CURDIR) \
		-I/usr/local/include
		
CLIBS =	-L/usr/local/lib -lsparkAPI \
		-L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm 

EXEC_OBJ = main.o 

EXEC = client

$(EXEC): $(EXEC_OBJ)
	@echo buidling...
	$(CXX) $(INCLUDES) -o $(EXEC) $(EXEC_OBJ) $(CLIBS)

$(EXEC_OBJ): main.cpp
	$(CXX) $(INCLUDES) -c main.cpp 

.PHONY: clean
clean:
	rm -rf *.o 
