#include <jni.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "spark/csparkcontext.hpp"
#include "spark/require.hpp"
#include "spark/convert.hpp"

#define OPTIONS_COUNT 8
#define V_1_6 JNI_VERSION_1_6

using namespace std;

CSparkContext::CSparkContext(const std::string& _master,
		const std::string& _jobName) :
		master(_master), jobName(_jobName) {
	//must be modified in futrue
	string classpath =
			"/home/bolei/workspace/spark-0.6.0/core/target/scala-2.9.2/classes";

	string jarpath = "/usr/local/lib";
	env = createJvm(classpath, jarpath);
	assert(env != NULL);
	////////////////////////////
	jclass scClazz = env->FindClass("spark/api/java/JavaSparkContext");
	assure(scClazz != NULL, env);

	jmethodID constructor = env->GetMethodID(scClazz, "<init>",
			"(Ljava/lang/String;Ljava/lang/String;)V");
	assure(constructor != NULL, env);

	sc = env->NewObject(scClazz, constructor, convertToJObject<string>(env, master),
			convertToJObject<string> (env, jobName));
	assert(sc != NULL);
}

CSparkContext::~CSparkContext() {
	const jsize vmBufLength = 1;
	JavaVM* vmBuf[vmBufLength];
	jint rv = 0;
	jint noVMs = 0;
	
	rv = JNI_GetCreatedJavaVMs(&(vmBuf[0]), vmBufLength, &noVMs);
	if (rv == 0) {
		fprintf(stderr, "JNI_GetCreatedJavaVMs failed with error: %d\n", rv);
		return;
	} else {
		JavaVM* vm = vmBuf[0];
		vm->DetachCurrentThread();
		assure(0 == vm->DestroyJavaVM(), env);
	}
}

CRDD& CSparkContext::textFile(const string& path) {
	jclass scClazz = env->GetObjectClass(sc);
	assert(scClazz != NULL);

	jmethodID textFileID = env->GetMethodID(scClazz, "textFile",
			"(Ljava/lang/String;)Lspark/api/java/JavaRDD;");
	assert(textFileID != NULL);

	jobject rddObj = env->CallObjectMethod(sc, textFileID,
			convertToJObject<string> (env, path));
	assert(rddObj != NULL);

	CRDD rdd(env, rddObj, sc);
	return rdd;
}

JNIEnv* createJvm(const string classpath, const string jarpath) {
	string cp = string("-Djava.class.path=") + classpath;
	string jp = string("-Djava.library.path=") + jarpath;

//	cout << cp << endl << jp << endl;

	JNIEnv* env;
	JavaVM* vm;
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

	res = vm->AttachCurrentThread((void**) &vm, &vm_args);
	if (res < 0) {
		fprintf(stderr, "Attach current thread failed . \n");
		exit(-1);
	}

	return env;
}

