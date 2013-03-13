#include <jni.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <typeinfo>
#include <cassert>
#include <list>

#include "spark/crdd.hpp"
#include "spark/cpairrdd.hpp"
#include "spark/require.hpp"
#include "spark/convert.hpp"
#include "spark/Class.h"
#include "spark/storageLevel.h"

using namespace std;

CRDD::CRDD(JNIEnv* _env, jobject obj, jobject scObj) :
		env(_env), rdd(obj), sparkctx(scObj){

	sparkClazz = env->GetObjectClass(sparkctx);
	rddClazz = env->GetObjectClass(rdd);

	level = NONE;
}

CRDD::~CRDD() {
}

CRDD& CRDD::cache() {
	jmethodID mid = env->GetMethodID(rddClazz, "cache",
			"()Lspark/api/java/JavaRDD;");
	assure(mid != NULL, env);

	jobject obj = env->CallObjectMethod(rdd, mid);
	assure(obj != NULL, env);

	rdd = obj;
	return *this;
}

CRDD& CRDD::persist() {
	return persist(MEMORY_ONLY);
}

CRDD& CRDD::persist(const StorageLevel newlevel) {
	if (level != NONE && newlevel != level) {
		cerr
				<< "Cannot change storage level of an RDD after it was already assigned a level"
				<< endl;
	}

	level = newlevel;
	char* ss = getStorageLevel(level);

	jclass storagecls = env->FindClass("spark/api/java/StorageLevel");
	assert(storagecls != NULL);

	jfieldID fid = env->GetStaticFieldID(storagecls, ss,
			"Lspark/storage/StorageLevel;");
	assert(fid != NULL);

	jobject levelObj = env->GetStaticObjectField(storagecls, fid);
	assert(levelObj != NULL);

	jmethodID mid = env->GetMethodID(rddClazz, "persist",
			"(Lspark/storage/StorageLevel;)Lspark/api/java/JavaRDD;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid, levelObj);
	assert(obj != NULL);

	rdd = obj;
	return *this;
}

CRDD& CRDD::distinct() {
	jmethodID mid = env->GetMethodID(rddClazz, "distinct", "()Lspark/api/java/JavaRDD;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj);

	rdd = obj;
	return *this;
}

CRDD& CRDD::distinct(int numSplits) {
	jmethodID mid = env->GetMethodID(rddClazz, "distinct",
			"(I)Lspark/api/java/JavaRDD;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid, numSplits);
	assert(obj);

	rdd = obj;
	return *this;
}

void CRDD::saveAsTextFile(const string& path) { // (Ljava/lang/String;)V
	if (path.empty()) {
		cout << "save path is null or empty !" << endl;
		return;
	}

	const char* strpath = const_cast<char*>(path.c_str());
	jstring jpath = env->NewStringUTF(strpath);
	assure(jpath != NULL, env);

	jclass cls = env->GetObjectClass(rdd);
	assure(cls != NULL, env);

	jmethodID mid = env->GetMethodID(cls, "saveAsTextFile", "(Ljava/lang/String;)V");
	assure(mid != NULL, env);

	env->CallVoidMethod(rdd, mid, jpath);
}

void CRDD::saveAsObjectFile(const string& path) { // (Ljava/lang/String;)V
	if (path.empty()) {
		cout << "save path is null or empty !" << endl;
		return;
	}

	const char* strpath = const_cast<char*>(path.c_str());
	jstring jpath = env->NewStringUTF(strpath);
	assure(jpath != NULL, env);

	jclass cls = env->GetObjectClass(rdd);
	assure(cls != NULL, env);

	jmethodID mid = env->GetMethodID(cls, "saveAsObjectFile",
			"(Ljava/lang/String;)V");
	assure(mid != NULL, env);

	env->CallVoidMethod(rdd, mid, jpath);
}

/*template<>
list<vector<double> > CRDD::takeSample(bool withReplacement, int num, int seed) {
	jclass cls = env->GetObjectClass(rdd);
	assert(cls != NULL);

	jmethodID mid = env->GetMethodID(cls, "takeSample", "(ZII)Ljava/util/List;");
	assert(mid != NULL);

	jobject listObj = env->CallObjectMethod(rdd, mid, withReplacement, num, seed);
	assert(listObj != NULL);

	list<vector<double> > lst;

	jclass listCls = env->GetObjectClass(listObj);
	assert(listCls != NULL);

	jmethodID sizeMethod = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMethod != NULL);

	jmethodID get = env->GetMethodID(listCls, "get", "(I)Ljava/lang/Object;");
	assert(get!=NULL);

	int len = env->CallIntMethod(listObj, sizeMethod);
	while(len > 0) {
		jobject iterable = env->CallObjectMethod(listObj, get, --len);
		assert(iterable != NULL);

		vector<double> vtr = convertFromJObject<vector<double> >(env, iterable);
		lst.push_back(vtr);
	}

	return lst;
}

template<typename T, typename R>
CRDD& CRDD::flatMap(const FlatMapContext<T, R>& fname) {

	const char* name = typeid(fname).name();

	jclass funcls = env->FindClass("spark/api/java/jni/FlatMapFunctionImpl");
	assert(funcls != NULL);

	jmethodID cons = env->GetMethodID(funcls, "<init>", "(Ljava/lang/String;)V");
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

template<typename T, typename R>
CRDD& CRDD::map(const MapContext<T, R>& fname) {

	const char* name = typeid(fname).name();

	jclass funcls = env->FindClass("spark/api/java/jni/FunctionImpl");
	assert(funcls != NULL);

	jmethodID cons = env->GetMethodID(funcls, "<init>", "(Ljava/lang/String;)V");
	assert(cons != NULL);

	jstring jparam = env->NewStringUTF(name);

	jobject funobj = env->NewObject(funcls, cons, jparam);
	assert(funobj != NULL);

	jmethodID mapMid =
			env->GetMethodID(rddClazz, "map",
					"(Lspark/api/java/function/Function;)Lspark/api/java/JavaRDD;");
	assert(mapMid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mapMid, funobj);
	assert(obj != NULL);

	rdd = obj;
	return *this;
}

template<typename T, typename K, typename V>
CPairRDD& CRDD::pairMap(const PairMapContext<T, K, V>& fname) {

	const char* name = typeid(fname).name();

	jclass funcls = env->FindClass("spark/api/java/jni/PairFunctionImpl");
	assert(funcls != NULL);

	jmethodID cons = env->GetMethodID(funcls, "<init>",
			"(Ljava/lang/String;)V");
	assert(cons != NULL);

	jstring jparam = env->NewStringUTF(const_cast<char*>(fname.c_str()));

	jobject funobj = env->NewObject(funcls, cons, jparam);
	assert(funobj != NULL);

	jmethodID mapMid =
			env->GetMethodID(rddClazz, "map",
					"(Lspark/api/java/function/PairFunction;)Lspark/api/java/JavaPairRDD;");
	assert(mapMid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mapMid, funobj);
	assert(obj != NULL );

	CPairRDD result(env, obj, sparkctx);
	return result;
}*/

/*template<>
std::vector<std::map<string, int> > CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid); // return List<Tuple2<String, int>>
	assert(obj != NULL);

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls != NULL);

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL);

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	std::vector < std::map<std::string, int> > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL);

		result.push_back(
				convertFromJObject < std::map<string, int> > (env, tuple));
	}
	////////////print ////////////////////////
	std::vector < std::map<std::string, int> >::iterator iter;
	for (iter = result.begin(); iter != result.end(); ++iter) {
		std::map<std::string, int> tmp = *iter;
		std::map<std::string, int>::iterator mapiter;
		for (mapiter = tmp.begin(); mapiter != tmp.end(); ++mapiter)
		{
			cout << mapiter->first << ":" << mapiter->second << endl;
		}
	}
	//////////////////////////////////////////
	assert(!result.empty());
	return result;
}

template<>
std::vector<std::vector<string> > CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj != NULL);

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls != NULL);

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL);

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	std::vector < std::vector<std::string> > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL);

		result.push_back(
				convertFromJObject < std::vector<string> > (env, tuple));
	}
	////////////print ////////////////////////
	std::vector < std::vector<std::string> >::iterator iter;
	for (iter = result.begin(); iter != result.end(); ++iter) {
		std::vector<std::string> tmp = *iter;
		std::vector<std::string>::iterator mapiter;
		for (mapiter = tmp.begin(); mapiter != tmp.end(); ++mapiter)
		{
			cout << *mapiter <<  endl;
		}
	}
	//////////////////////////////////////////
	assert(!result.empty());
	return result;
}

template<>
vector<vector<double> > CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL);

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj != NULL);

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls != NULL);

	jclass test = env->FindClass("java/util/List");
	assert(test != NULL);

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL);

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	vector<vector<double> > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL);

		result.push_back(
				convertFromJObject < std::vector<double> > (env, tuple));
	}
	////////////print ////////////////////////
	vector <vector<double> >::iterator iter;
	for (iter = result.begin(); iter != result.end(); ++iter) {
		vector<double>  tmp = *iter;
		vector<double>::iterator mapiter;
		for (mapiter = tmp.begin(); mapiter != tmp.end(); ++mapiter)
		{
			cout << *mapiter <<  endl;
		}
	}
	//////////////////////////////////////////
	assert(!result.empty());
	return result;
}

template<>
std::vector<string> CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL);

	jclass test = env->FindClass("spark/api/java/JavaPairRDD");
	if (env->IsInstanceOf(rdd, test)) {
		cout << "rdd is JavaPairRDD" << endl;
	}

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj != NULL);

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls != NULL);

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL);

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	std::vector < string > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL);

		result.push_back(
				convertFromJObject <string> (env, tuple));
	}
	////////////print ////////////////////////
	std::vector <string>::iterator iter;
	for (iter = result.begin(); iter != result.end(); ++iter) {
		cout << *iter <<  endl;
	}
	//////////////////////////////////////////
	assert(!result.empty());
	return result;
}

template<>
std::vector<std::map<int, std::list<double> > > CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL );

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj != NULL );

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls );

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL );

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	std::vector<std::map<int, std::list<double> > > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL );
		result.push_back(
				convertFromJObject < std::map<int, std::list<double> > > (env, tuple));
	}
	assert(!result.empty());
	return result;
}

template<>
std::vector<std::map<int, std::map<int, std::vector<double> > > > CRDD::collect() {

	jmethodID mid = env->GetMethodID(rddClazz, "collect", "()Ljava/util/List;");
	assert(mid != NULL );

	jobject obj = env->CallObjectMethod(rdd, mid);
	assert(obj != NULL );

	jclass listCls = env->GetObjectClass(obj);
	assert(listCls );

	jmethodID getMid = env->GetMethodID(listCls, "get",
			"(I)Ljava/lang/Object;");
	assert(getMid != NULL );

	jmethodID sizeMid = env->GetMethodID(listCls, "size", "()I");
	assert(sizeMid != NULL );

	jint size = env->CallIntMethod(obj, sizeMid);
	assert(size != 0 );

	cout << size << endl;
	std::vector<std::map<int, std::map<int, std::vector<double> > > > result;
	for (int i = 0; i < size; ++i) {
		jobject tuple = env->CallObjectMethod(obj, getMid, i);
		assert(tuple != NULL );
		result.push_back(
				convertFromJObject < std::map<int, std::map<int, std::vector<double> > > >(env, tuple));
	}

	////////////print ////////////////////////
	vector<std::map<int, std::map<int, std::vector<double> > > >::iterator iter;
	for (iter = result.begin(); iter != result.end(); ++iter) {
		std::map<int, std::map<int, std::vector<double> > > tmp = *iter;
		std::map<int, std::map<int, std::vector<double> > >::iterator mapiter;
		for (mapiter = tmp.begin(); mapiter != tmp.end(); ++mapiter) {
			cout << mapiter->first << endl;
		}
	}
	//////////////////////////////////////////
	assert(!result.empty());
	return result;
}*/

