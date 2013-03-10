#ifndef __CRDD_HPP__
#define __CRDD_HPP__

#include <jni.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdarg.h>
#include <list>

#include "spark/storageLevel.h"
#include "spark/Class.h"
#include "spark/convert.hpp"

using namespace std;

class CPairRDD;

class CRDD {
protected:
	JNIEnv* env;
	jobject sparkctx;
	jobject sparkClazz;

	jobject rdd;
	jclass rddClazz;

	StorageLevel level;

public:
	
	CRDD(JNIEnv* env, jobject jobj/**/, jobject sc/*CSparkContext instance*/);
	virtual ~CRDD();

	CRDD& cache();
	CRDD& persist();
	CRDD& persist(const StorageLevel level);

	CRDD& distinct();
	CRDD& distinct(int numSplits);

	void saveAsTextFile(const string& path);
	void saveAsObjectFile(const string& path);

public:

	// inline template function.

	template<typename T>
	list<vector<T> > takeSample(bool withReplacement, int num, int seed) {
		jclass cls = env->GetObjectClass(rdd);
		assert(cls != NULL);

		jmethodID mid = env->GetMethodID(cls, "takeSample",
				"(ZII)Ljava/util/List;");
		assert(mid != NULL);

		jobject listObj = env->CallObjectMethod(rdd, mid, withReplacement, num,
				seed);
		assert(listObj != NULL);

		list<vector<T> > lst;

		jclass listCls = env->GetObjectClass(listObj);
		assert(listCls != NULL);

		jmethodID sizeMethod = env->GetMethodID(listCls, "size", "()I");
		assert(sizeMethod != NULL);

		jmethodID get = env->GetMethodID(listCls, "get",
				"(I)Ljava/lang/Object;");
		assert(get!=NULL);

		int len = env->CallIntMethod(listObj, sizeMethod);
		while (len > 0) {
			jobject iterable = env->CallObjectMethod(listObj, get, --len);
			assert(iterable != NULL);

			vector<T> vtr = convertFromJObject < vector<T>
					> (env, iterable);
			lst.push_back(vtr);
		}

		return lst;
	}

	template<typename T>
	vector<T> collect() {
		jmethodID mid = env->GetMethodID(rddClazz, "collect",
				"()Ljava/util/List;");
		assert(mid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mid);
		assert(obj != NULL);

		jclass listCls = env->GetObjectClass(obj);
		assert(listCls);

		jmethodID getMid = env->GetMethodID(listCls, "get",
				"(I)Ljava/lang/Object;");
		assert(getMid != NULL);

		jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
		assert(sizeMid != NULL);

		jint size = env->CallIntMethod(obj, sizeMid);
		assert(size != 0);

		std::vector<T> result;
		for (int i = 0; i < size; ++i) {
			jobject tuple = env->CallObjectMethod(obj, getMid, i);
			assert(tuple != NULL);

			result.push_back(convertFromJObject<T>(env, tuple));
		}

		assert(!result.empty());
		return result;
	}

	template<typename T, typename R>
	CRDD& flatMap(const FlatMapContext<T, R>& fname) {
		const char* name = typeid(fname).name();

		jclass funcls = env->FindClass(
				"spark/api/java/jni/FlatMapFunctionImpl");
		assert(funcls != NULL);

		jmethodID cons = env->GetMethodID(funcls, "<init>",
				"(Ljava/lang/String;)V");
		assert(cons != NULL);
		jstring jparam = env->NewStringUTF(name);

		jobject funobj = env->NewObject(funcls, cons, jparam);
		assert(funobj != NULL);

		jmethodID flatmap =
				env->GetMethodID(rddClazz, "flatMap",
						"(Lspark/api/java/function/FlatMapFunction;)Lspark/api/java/JavaRDD;");
		assert(flatmap != NULL);

		jobject obj = env->CallObjectMethod(rdd, flatmap, funobj);
		assert(obj != NULL);

		rdd = obj;
		return *this;
	}

	template<typename T, typename K, typename V>
	CPairRDD& pairflatMap(const PairFlatMapContext<T, K, V>& fname) {
		return NULL;
	}

	template<typename T, typename R>
	CRDD& map(const MapContext<T, R>& fname) {

		const char* name = typeid(fname).name();

		jclass funcls = env->FindClass("spark/api/java/jni/FunctionImpl");
		assert(funcls != NULL);

		jmethodID cons = env->GetMethodID(funcls, "<init>",
				"(Ljava/lang/String;)V");
		assert(cons != NULL);

		jstring jparam = env->NewStringUTF(name);

		jobject funobj = env->NewObject(funcls, cons, jparam);
		assert(funobj != NULL);

		jmethodID mapMid = env->GetMethodID(rddClazz, "map",
				"(Lspark/api/java/function/Function;)Lspark/api/java/JavaRDD;");
		assert(mapMid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mapMid, funobj);
		assert(obj != NULL);

		rdd = obj;
		return *this;
	}

	template<typename T, typename K, typename V>
	CPairRDD& pairMap(const PairMapContext<T, K, V>& fname) {
		const char* name = typeid(fname).name();

		jclass funcls = env->FindClass("spark/api/java/jni/PairFunctionImpl");
		assert(funcls != NULL);

		jmethodID cons = env->GetMethodID(funcls, "<init>",
				"(Ljava/lang/String;)V");
		assert(cons != NULL);

		jstring jparam = env->NewStringUTF(name);

		jobject funobj = env->NewObject(funcls, cons, jparam);
		assert(funobj != NULL);

		jmethodID mapMid =
				env->GetMethodID(rddClazz, "map",
						"(Lspark/api/java/function/PairFunction;)Lspark/api/java/JavaPairRDD;");
		assert(mapMid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mapMid, funobj);
		assert(obj != NULL);

		CPairRDD result(env, obj, sparkctx);
		return result;
	}

};

#endif // __CRDD_HPP__
