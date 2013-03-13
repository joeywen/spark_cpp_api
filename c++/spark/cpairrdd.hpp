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

	CPairRDD& groupByKey() {
		return groupByKey(0);
	}

	CPairRDD& groupByKey(int numSplits) {
		assert(numSplits >= 0);
		if (numSplits == 0) {
			jmethodID mid = env->GetMethodID(rddClazz, "groupByKey", "()Lspark/api/java/JavaPairRDD;");
			assert(mid != NULL);

			jobject obj = env->CallObjectMethod(rdd, mid);
			assert(obj != NULL);

			rdd = obj;
			return *this;
		} else {
			jmethodID mid = env->GetMethodID(rddClazz, "groupByKey",
					"(I)Lspark/api/java/JavaPairRDD;");
			assert(mid != NULL);

			jobject obj = env->CallObjectMethod(rdd, mid, numSplits);
			assert(obj != NULL);

			rdd = obj;
			return *this;
		}
	}

	template<typename T>
	CPairRDD& groupByKey(const Partitioner<T>& partitioner) {
		const char* partition = typeid(partitioner).name();
		jclass pcls = env->FindClass("spark/api/java/jni/PartitionerImpl");
		assert(pcls != NULL);

		jmethodID pmid = env->GetMethodID(pcls, "<init>",
				"(ILjava/lang/String;)V");
		assert(pmid != NULL);

		jstring pparam = env->NewStringUTF(partition);
		assert(pparam);

		jobject pobj = env->NewObject(pcls, pmid, partitioner.numPartitions,
				pparam);
		assert(pobj != NULL);

		jmethodID mid = env->GetMethodID(rddClazz, "groupByKey", "(Lspark/Partitioner;)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mid, pobj);
		assert(obj != NULL);

		rdd = obj;
		return *this;
	}

	CPairRDD& sortByKey(){
		return sortByKey(true);
	}

	CPairRDD& sortByKey(bool ascending) {
		jmethodID mid = env->GetMethodID(rddClazz, "sortByKey", "(Z)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mid);
		assert(obj != NULL);

		rdd = obj;
		return *this;
	}

	template<typename T1, typename T2, typename R>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname) {
		reduceByKey(fname, 0);
	}

	template<typename T1, typename T2, typename R>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname, int numPartitions) {
		assert(numPartitions >= 0);
		const char* name = typeid(fname).name();

		jclass funcls = env->FindClass("spark/api/java/jni/Function2Impl");
		assert(funcls != NULL);

		jmethodID cons = env->GetMethodID(funcls, "<init>",
				"(Ljava/lang/String;)V");
		assert(cons != NULL);

		jstring jparam = env->NewStringUTF(name);

		jobject funobj = env->NewObject(funcls, cons, jparam);
		assert(funobj != NULL);

		if (numPartitions == 0) {
			jmethodID reduceMid =
					env->GetMethodID(rddClazz, "reduceByKey",
							"(Lspark/api/java/function/Function2;)Lspark/api/java/JavaPairRDD;");
			assert(reduceMid != NULL);

			jobject obj = env->CallObjectMethod(rdd, reduceMid, funobj);
			assert(obj != NULL);

			rdd = obj;
			return *this;
		} else {
			jmethodID reduceMid =
					env->GetMethodID(rddClazz, "reduceByKey",
							"(Lspark/api/java/function/Function2;I)Lspark/api/java/JavaPairRDD;");
			assert(reduceMid != NULL);

			jobject obj = env->CallObjectMethod(rdd, reduceMid, funobj);
			assert(obj != NULL);

			rdd = obj;
			return *this;
		}
	}

	template<typename T1, typename T2, typename R, typename P>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname, const Partitioner<P>& partitioner) {
		const char* reduce = typeid(fname).name();
		const char* partition = typeid(partitioner).name();

		jclass funcls = env->FindClass("spark/api/java/jni/Function2Impl");
		assert(funcls);

		jmethodID funmid = env->GetMethodID(funcls, "<init>", "(Ljava/lang/String;)V");
		assert(funmid != NULL);

		jstring funparam = env->NewStringUTF(reduce);
		assert(funparam != NULL);

		jobject funcobj = env->NewObject(funcls, funmid, funparam);
		assert(funcobj != NULL);

		///////////////////// partitioner /////////////////////////////
		jclass pcls = env->FindClass("spark/api/java/jni/PartitionerImpl");
		assert(pcls != NULL);

		jmethodID pmid = env->GetMethodID(pcls, "<init>", "(ILjava/lang/String;)V");
		assert(pmid != NULL);

		jstring pparam = env->NewStringUTF(partition);
		assert(pparam);

		jobject pobj = env->NewObject(pcls, pmid, partitioner.numPartitions, pparam);
		assert(pobj != NULL);

		jmethodID mid = env->GetMethodID(rddClazz, "reduceByKey", "(Lspark/Partitioner;Lspark/api/java/function/Function2;)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mid, pobj, funcobj);
		assert(obj != NULL);

		rdd = obj;

		return *this;
	}

	template<typename V, typename K, typename C, typename P>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2,
			const Partitioner<P>& partitioner) {
		/*const char* mapfname = typeid(mctx).name();
		const char* reduce1 = typeid(rctx).name();
		const char* reduce2 = typeid(rctx2).name();
		const char* partition = typeid (partitioner).name();

		/////////////////////////// new map function ///////////////////////////////
		jclass mapcls = env->FindClass("spark/api/java/jni/FunctionImpl");
		assert(mapcls != NULL);

		jmethodID mapcons = env->GetMethodID(mapcls, "<init>",
				"(Ljava/lang/String;)V");
		assert(mapcons != NULL);

		jstring mapparam = env->NewStringUTF(mapfname);
		assert(mapparam != NULL);

		jobject mapobj = env->NewObject(mapcls, mapcons, mapparam);
		assert(mapobj);
		//////////////////////reduce function////////////////////////////////////
		jclass reducecls = env->FindClass("spark/api/java/jni/Function2Impl");
		assert(reducecls != NULL);

		jmethodID reducecons = env->GetMethodID(reducecls, "<init>",
				"(Ljava/lang/String;)V");
		assert(reducecons != NULL);

		jstring reduce1param = env->NewStringUTF(reduce1);

		jobject redeuce1obj = env->NewObject(reducecls, reducecons,
				reduce1param);
		assert(redeuce1obj != NULL);

		jstring reduce2param = env->NewStringUTF(reduce2);
		jobject redeuce2obj = env->NewObject(reducecls, reducecons,
				reduce2param);
		assert(redeuce2obj != NULL);
		///////////////////// partitioner ///////////////////////////////////////////////
		jclass pcls = env->FindClass("spark/api/java/jni/PartitionerImpl");
		assert(pcls != NULL);

		jmethodID pmid = env->GetMethodID(pcls, "<init>",
				"(ILjava/lang/String;)V");
		assert(pmid != NULL);

		jstring pparam = env->NewStringUTF(partition);
		assert(pparam);

		jobject pobj = env->NewObject(pcls, pmid, partitioner.numPartitions,
				pparam);
		assert(pobj != NULL);
		//////////////////////////////////////////////////////////////////////
		jmethodID mid =
				env->GetMethodID(rddClazz, "combineByKey",
						"(Lspark/api/java/function/Function;Lspark/api/java/function/Function2;Lspark/api/java/function/Function2;I)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);
		jobject obj = env->CallObjectMethod(rdd, mid, mapobj, reduce1obj,
				reduce2obj, numPartitions);
		assert(obj != NULL);

		rdd = obj;*/

		return *this;
	}

	template<typename V, typename K, typename C, typename P>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2,
			int numPartitions) {
		assert(numPartitions >= 0);

		const char* mapfname = typeid(mctx).name();
		const char* reduce1 = typeid(rctx).name();
		const char* reduce2 = typeid(rctx2).name();

		/////////////////////////// new map function ///////////////////////////////
		jclass mapcls = env->FindClass("spark/api/java/jni/FunctionImpl");
		assert(mapcls != NULL);

		jmethodID mapcons = env->GetMethodID(mapcls, "<init>", "(Ljava/lang/String;)V");
		assert(mapcons != NULL);

		jstring mapparam = env->NewStringUTF(mapfname);
		assert(mapparam != NULL);

		jobject mapobj = env->NewObject(mapcls, mapcons, mapparam);
		assert(mapobj);
		//////////////////////reduce function////////////////////////////////////
		jclass reducecls = env->FindClass("spark/api/java/jni/Function2Impl");
		assert(reducecls != NULL);

		jmethodID reducecons = env->GetMethodID(reducecls, "<init>", "(Ljava/lang/String;)V");
		assert(reducecons != NULL);

		jstring reduce1param = env->NewStringUTF(reduce1);

		jobject reduce1obj = env->NewObject(reducecls, reducecons, reduce1param);
		assert(reduce1obj != NULL);

		jstring reduce2param = env->NewStringUTF(reduce2);
		jobject reduce2obj = env->NewObject(reducecls, reducecons, reduce2param);
		assert(reduce2obj != NULL);

		//////////////////////////////////////////////////////////////////////
		jmethodID mid =
				env->GetMethodID(rddClazz, "combineByKey",
						"(Lspark/api/java/function/Function;Lspark/api/java/function/Function2;Lspark/api/java/function/Function2;I)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);
		jobject obj = env->CallObjectMethod(rdd, mid, mapobj, reduce1obj, reduce2obj, numPartitions);
		assert(obj != NULL);

		rdd = obj;

		return *this;
	}

	template<typename V, typename K, typename C, typename P>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2) {
		return combineByKey(mctx, rctx, rctx2, 0);
	}

	template<typename T>
	CPairRDD& partitionBy(const Partitioner<T>& partitioner) {
		jmethodID mid = env->GetMethodID(rddClazz, "partitionBy", "(Lspark/Partitioner;)Lspark/api/java/JavaPairRDD;");
		assert(mid != NULL);

		jobject obj = env->CallObjectMethod(rdd, mid);
		assert(obj != NULL);

		rdd = obj;
		return *this;
	}
	//template<class T, class R>
	//std::map<T, R> reduceByKeyLocally(const ReduceContext<T1, T2, R>& fname);
	// (Lspark/api/java/function/Function2;)Ljava/util/Map;
};

#endif // __CPAIR_RDD_HPP__
