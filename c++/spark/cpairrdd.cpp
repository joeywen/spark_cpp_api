#include "spark/cpairrdd.hpp"
#include "spark/crdd.hpp"
#include "spark/csparkcontext.hpp"
#include "spark/convert.hpp"
#include "spark/require.hpp"
#include "spark/Class.h"

#include <jni.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>
#include <stdarg.h>

using namespace std;

CPairRDD::CPairRDD(JNIEnv* _env, jobject obj, jobject sc) :
		CRDD(_env, obj, sc)/*,env(_env), jobj(obj), sparksc(sc) */{
}

CPairRDD::~CPairRDD() {

}

/*template<typename T1, typename T2, typename R>
CPairRDD& CPairRDD::reduceByKey(const ReduceContext<T1, T2, R>& fname) {

	const char* name = typeid(fname).name();

	jclass funcls = env->FindClass("spark/api/java/jni/Function2Impl");
	assert(funcls != NULL);

	jmethodID cons = env->GetMethodID(funcls, "<init>",
			"(Ljava/lang/String;)V");
	assert(cons != NULL);

	jstring jparam = env->NewStringUTF(name);

	jobject funobj = env->NewObject(funcls, cons, jparam);
	assert(funobj != NULL);

	jmethodID reduceMid =
			env->GetMethodID(rddClazz, "reduceByKey",
					"(Lspark/api/java/function/Function2;)Lspark/api/java/JavaPairRDD;");
	assert(reduceMid != NULL);

	jobject obj = env->CallObjectMethod(rdd, reduceMid, funobj);
	assert(obj != NULL);

	rdd = obj;
	return *this;
}

template<typename T1, typename T2, typename R>
CPairRDD& CPairRDD::reduceByKey(const ReduceContext<T1, T2, R>& fname, const Partitioner& partitioner) {
	return NULL;
}

template<typename V, typename K, typename C>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2,
			const Partitioner& partitioner) {
	return NULL;
}*/
