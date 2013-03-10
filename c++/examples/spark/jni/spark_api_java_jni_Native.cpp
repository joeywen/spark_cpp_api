#include <jni.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <cassert>
#include <typeinfo>
#include <dlfcn.h>

#include "spark/jni/spark_api_java_jni_Native.h"
#include "spark/convert.hpp"
#include "spark/require.hpp"
#include "spark/Class.h"
#include "spark/mapping.hpp"

using namespace std;

#define METHOD_NAME "call"

/*
 * Class:     spark_api_java_jni_Native
 * Method:    callFlatMap
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Iterable;
 */
JNIEXPORT jobject JNICALL Java_spark_api_java_jni_Native_callFlatMap
  (JNIEnv *env, jobject jobj, jstring jstr1, jstring fname) {
	string name = jstrTocstr(env, fname);
	Class *clazz = Class::forName(name);
	assert(clazz != NULL);
	Object *obj = clazz->newInstance();
	assert(obj != NULL);

	const string supername = clazz->getSuperClassName();

	return mapping(env, obj, supername, jstr1);
}

/*
 * Class:     spark_api_java_jni_Native
 * Method:    function2call
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_spark_api_java_jni_Native_function2call
  (JNIEnv *env, jobject jobj, jobject o1, jobject o2, jstring fname) {
	string name = jstrTocstr(env, fname);
	Class *clazz = Class::forName(name);
	assert(clazz != NULL);
	Object *obj = clazz->newInstance();
	assert(obj != NULL);

	const string supername = clazz->getSuperClassName();
	return mapping(env, obj, supername, o1, o2);
}

/*
 * Class:     spark_api_java_jni_Native
 * Method:    functionCall
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_spark_api_java_jni_Native_functionCall
  (JNIEnv *env, jobject jobj, jobject obj, jstring fname) {
	string name = jstrTocstr(env, fname);
	Class *clazz = Class::forName(name);
	assert(clazz != NULL);
	Object *object = clazz->newInstance();
	assert(object != NULL);

	const string supername = clazz->getSuperClassName();

	return mapping(env, object, supername, obj);
}

/*
 * Class:     spark_api_java_jni_Native
 * Method:    pairCall
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)Ljava/util/HashMap;
 */
JNIEXPORT jobject JNICALL Java_spark_api_java_jni_Native_pairCall
  (JNIEnv *env, jobject jobj, jobject obj, jstring fname) {
	string name = jstrTocstr(env, fname);
	Class *clazz = Class::forName(name);
	assert(clazz != NULL);
	Object *object = clazz->newInstance();
	assert(object != NULL);

	const string supername = clazz->getSuperClassName();
	return mapping(env, object, supername, obj);
}
