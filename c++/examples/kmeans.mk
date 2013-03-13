CXX = g++
CC = gcc

INCLUDES = -I$(JAVA_HOME)/include \
		-I$(JAVA_HOME)/include/linux \
		-I$(JAVA_HOME)/jre/lib/amd64/server/ \
		-I$(CURDIR) \
		-I/usr/local/include
		
CLIBS =	-lsparkAPI \
		-L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm

EXEC_OBJ = kmeans.o

EXEC = kmeans

$(EXEC): $(EXEC_OBJ)
	@echo buidling...
	$(CXX) $(INCLUDES) -o $(EXEC) $(EXEC_OBJ) $(CLIBS)

$(EXEC_OBJ): kmeans.cpp
	$(CXX) $(INCLUDES) -c kmeans.cpp $(CLIBS)

.PHONY: clean
clean:
	rm -rf *.o 
