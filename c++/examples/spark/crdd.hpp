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

	template<typename T>
	list<vector<T> > takeSample(bool withReplacement, int num, int seed);

	template<typename T>
	vector<T> collect();

	template<typename T, typename R>
	CRDD& flatMap(const FlatMapContext<T, R>& fname);

	template<typename T, typename K, typename V>
	CPairRDD& PairflatMap(const PairFlatMapContext<T, K, V>& fname);

	template<typename T, typename R>
	CRDD& map(const MapContext<T, R>& fname);

	template<typename T, typename K, typename V>
	CPairRDD& PairMap(const PairMapContext<T, K, V>& fname);

	CRDD& cache();
	CRDD& persist();
	CRDD& persist(const StorageLevel level);
	
	CRDD& distinct();
	CRDD& distinct(int numSplits);

	void saveAsTextFile(const string& path);
	void saveAsObjectFile(const string& path);
};

#endif // __CRDD_HPP__
