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

	template<typename T1, typename T2, typename R>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname);

	template<typename T1, typename T2, typename R>
	CPairRDD& reduceByKey(const ReduceContext<T1, T2, R>& fname, const Partitioner& partitioner);

	template<typename V, typename K, typename C>
	CPairRDD& combineByKey(const MapContext<V, C>& mctx,
			const ReduceContext<C, V, C>& rctx,
			const ReduceContext<C, C, C>& rctx2,
			const Partitioner& partitioner);
	//template<class T, class R>
	//std::map<T, R> reduceByKeyLocally(const ReduceContext<T1, T2, R>& fname);
};

#endif // __CPAIR_RDD_HPP__
