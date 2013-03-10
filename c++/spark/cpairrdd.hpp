#ifndef __CPAIRRDD_HPP__
#define __CPAIRRDD_HPP__

#include <jni.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdarg.h>

#include "spark/crdd.hpp"
#include "spark/Class.h"

class CPairRDD : public CRDD {
public:
	CPairRDD(JNIEnv* env, jobject jobj, jobject sparksc);
	virtual ~CPairRDD();


public:
	// inline template function.

	template<typename T1, typename T2, typename R>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname) {
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
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname, const Partitioner& partitioner) {
		return NULL;
	}

	template<typename V, typename K, typename C>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2,
			const Partitioner& partitioner) {
		return NULL;
	}
	//template<class T, class R>
	//std::map<T, R> reduceByKeyLocally(const ReduceContext<T1, T2, R>& fname);
};

#endif // __CPAIR_RDD_HPP__
