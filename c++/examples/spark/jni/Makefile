CXX = g++
CC = gcc
INCLUDES = -I/home/software/java/jdk1.6.0_37/include \
		-I/home/software/java/jdk1.6.0_37/include/linux \
		-I/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ \
		-I/home/bolei/workspace/spark-0.6.0/c++/
CLIBS =	-L/home/software/java/jdk1.6.0_37/jre/lib/amd64/server/ -ljvm
OBJS = spark_api_java_jni_Native.o
#spark_api_java_jni_IIIFunction2.o \
	   spark_api_java_jni_PairSISFunction.o \
	   spark_api_java_jni_StringFlatMapFunction.o
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
