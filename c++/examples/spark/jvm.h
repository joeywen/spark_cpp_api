#ifndef _JVM_H_
#define _JVM_H_

#include <string>
#include <jni.h>
#include <iostream>
#include <cassert>
#include <cstdlib>

#define OPTIONS_COUNT 8
#define V_1_6 JNI_VERSION_1_6

using namespace std;

JNIEnv* createJvm(const string classpath, const string jarpath) {
	string cp = string("-Djava.class.path=") + classpath;
	string jp = string("-Djava.library.path=") + jarpath;
	
//	cout << cp << endl << jp << endl;

	JavaVM* vm;
	JNIEnv* env;
	JavaVMInitArgs vm_args;
	JavaVMOption options[OPTIONS_COUNT];

	options[0].optionString = "-Djava.compiler=NONE"; // disable JIT compiler
	options[1].optionString = const_cast<char*>(cp.c_str()); // user java class path
	options[2].optionString = const_cast<char*>(jp.c_str()); // user third jars
	options[3].optionString = "-verbose:class, gc, jni"; //enable verbose output message
	options[4].optionString = "-Xms512m";
	options[5].optionString = "-Xmx2048m";
	options[6].optionString = "-XX:MaxPermSize=1024m";
	options[7].optionString = "-XX:-ReduceInitialCardMarks";
//	options[8].optionString = "-XX:+TraceClassLoading";
//	options[9].optionString = "-XX:+TraceClassUnloading";

	vm_args.version = V_1_6;
	vm_args.options = options;
	vm_args.nOptions = OPTIONS_COUNT;
	vm_args.ignoreUnrecognized = JNI_TRUE;

	jint res = JNI_CreateJavaVM(&vm, (void **) &env, &vm_args);

	if (res < 0) {
		fprintf(stderr, "create jvm error.\n");
		exit(-1);
	}

	res = vm->AttachCurrentThread((void**)&vm, &vm_args);
	if (res < 0) {
		fprintf(stderr, "Attach current thread failed . \n");
		exit(-1);
	}

	return env;
}

#endif
