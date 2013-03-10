#include <jni.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <iterator>
#include <cstring>
#include <cstdlib>
#include <typeinfo>
#include <search.h>
#include <pthread.h>
#include <errno.h>
#include <stdexcept>
#include <exception>
#include <list>

#include "spark/require.hpp"
#include "spark/convert.hpp"

using std::string;

static pthread_mutex_t hashMutex =
						PTHREAD_MUTEX_INITIALIZER;
						static pthread_mutex_t vectorMutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_HASH_MAP() pthread_mutex_lock(&hashMutex)
#define UNLOCK_HASH_MAP() pthread_mutex_unlock(&hashMutex)
#define LOCK_VECTOR() pthread_mutex_lock(&vectorMutex)
#define UNLOCK_VECTOR() pthread_mutex_unlock(&vectorMutex)

string jstrTocstr(JNIEnv *env, jstring jstr) {
	const char* s = env->GetStringUTFChars(jstr, NULL);
	assure(s != NULL, env);
	string result(s);
	env->ReleaseStringUTFChars(jstr, s);

	return result;
}

/**
  * convert c++ string to jni jstring
  */
template<>
jobject convertToJObject(JNIEnv* env, const string& s) {
	jstring ss = env->NewStringUTF(s.c_str());
	assure(ss != NULL, env);

	return ss;
}

/**
 * convert cpp int to java Integer
 */
template<>
jobject convertToJObject(JNIEnv* env, const int& val) {
	jclass cls = env->FindClass("java/lang/Integer");
	assure(cls != NULL, env);
	jmethodID cons = env->GetMethodID(cls, "<init>", "(I)V");
	assure(cons != NULL, env);
	jobject obj = env->NewObject(cls, cons, val);
	assure(obj != NULL, env);
	return obj;
}

template<>
jobject convertToJObject(JNIEnv* env, const std::list<string>& lst) {
	int size = lst.size();
	jclass strcls = env->FindClass("java/lang/String");
	assure(strcls != NULL, env);
	jobjectArray objArr = env->NewObjectArray(size, strcls, NULL);
	assure(objArr != NULL, env);
	std::list<string>::const_iterator v;
	int i = -1;
	for (v = lst.begin(); v != lst.end(); v++) {
		string ss = *v;
		jstring str = env->NewStringUTF(const_cast<char*>(ss.c_str()));
		assure(str != NULL, env);
		env->SetObjectArrayElement(objArr, ++i, str);
	}

	//lst.clear();
	return objArr;
}

/**
 * convert list<double> to java Iterable<Double>
 */
template<>
jobject convertToJObject(JNIEnv* env, const std::list<double>& ll) {

	int size = ll.size();
	double arr[size];
	std::copy(ll.begin(), ll.end(), arr);

	jclass doubleClazz = env->FindClass("java/lang/Double");
	assure(doubleClazz != NULL, env);

	jmethodID cons = env->GetMethodID(doubleClazz, "<init>", "(D)V");
	assure(cons != NULL, env);

	jobjectArray dArray = env->NewObjectArray(size, doubleClazz, NULL);
	assure(dArray != NULL, env);
	try {
		for (int i = 0; i < size; ++i) {
			double d = arr[i];
			jobject dd = env->NewObject(doubleClazz, cons, (jdouble)d);
			assure(dd != NULL, env);
			env->SetObjectArrayElement(dArray, i, dd);

		}
	} catch (std::out_of_range exc) {
		std::cerr << exc.what() << std::endl;
	}
	jclass arraysCls = env->FindClass("java/util/Arrays");
	assure(arraysCls != NULL, env);
	jmethodID mid = env->GetStaticMethodID(arraysCls, "asList",
									"([Ljava/lang/Object;)Ljava/util/List;");
	assure(mid != NULL, env);
	jobject iterable = env->CallStaticObjectMethod(arraysCls, mid, dArray);
	assure(iterable != NULL, env);

	return iterable;
}

/**
 * convert vector<string> to java Iterbale<Object>
 */
template<>
jobject convertToJObject(JNIEnv* env, const std::vector<string>& vec) {
	LOCK_VECTOR();
	int size = vec.size();
	string arr[size];
	std::copy(vec.begin(), vec.end(), arr);
	jclass stringClazz = env->FindClass("java/lang/String");
	assure(stringClazz != NULL, env);
	jobjectArray strArray = env->NewObjectArray(size, stringClazz, NULL);
	assure(strArray != NULL, env);
	for (int i = 0; i < size; ++i) {
		try {
			const char* ss = const_cast<char*>(vec[i].c_str());
			jstring str = env->NewStringUTF(ss);
			assure(str != NULL, env);
			ss = NULL;
			delete ss;
			env->SetObjectArrayElement(strArray, i, str);
		} catch (std::out_of_range exc) {
			std::cerr << exc.what() << std::endl;
		}
		//assure(false, env);
	}

	jclass arraysCls = env->FindClass("java/util/Arrays");
	assure(arraysCls != NULL, env);
	jmethodID mid = env->GetStaticMethodID(arraysCls, "asList",
									"([Ljava/lang/Object;)Ljava/util/List;");
	assure(mid != NULL, env);

	jobject iterable = env->CallStaticObjectMethod(arraysCls, mid, strArray);
	assure(iterable != NULL, env);
	UNLOCK_VECTOR();
	return iterable;
}

/**
* convert c++ map to java hashmap
 */
template<>
jobject convertToJObject(JNIEnv* env, const std::map<string, int>& mm) {
	jclass hashmapcls = env->FindClass("java/util/HashMap");
	assure(hashmapcls != NULL, env);
	jmethodID constructor = env->GetMethodID(hashmapcls, "<init>", "()V");
	assure(constructor != NULL, env);
	jobject hashMap = env->NewObject(hashmapcls, constructor);
	assure(hashMap != NULL, env);
	jmethodID putMid = env->GetMethodID(hashmapcls, "put",
			"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	assure(putMid != NULL, env);
	LOCK_HASH_MAP();
	for (std::map<string, int>::const_iterator iter = mm.begin();
			iter != mm.end(); ++iter) {
			string str = iter->first;
			const char* ss = const_cast<char*>(str.c_str());
			jstring key = env->NewStringUTF(ss); //env->NewStringUTF(ss);
			assure(key != NULL, env);
			int value = iter->second;
			jobject integer = convertToJObject<int>(env, value);
			env->CallObjectMethod(hashMap, putMid, key, integer);
	}
	UNLOCK_HASH_MAP();
	return hashMap;
}

/**
 * convert cpp::vector<double> to java::Iterable<Double>
 */
template<>
jobject convertToJObject(JNIEnv* env, const std::vector<double>& ll) {
	/*jclass clazz = env->FindClass("java/util/Vector");
	assure(clazz != NULL, env);

	jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
	assure(constructor != NULL, env);

	jmethodID addMid = env->GetMethodID(clazz, "addElement", "(Ljava/lang/Object;)V");
	assure(addMid != NULL, env);

	jobject vectorObj = env->NewObject(clazz, constructor);
	assure(vectorObj != NULL, env);

	std::vector<double>::const_iterator iter;
	for (iter = v.begin(); iter != v.end(); ++iter) {
		double d = *iter;
		env->CallVoidMethod(vectorObj, addMid, d);
	}

	jmethodID size = env->GetMethodID(clazz, "size", "()I");
	assure(size != 0, env);
*/

	int size = ll.size();
	double arr[size];
	std::copy(ll.begin(), ll.end(), arr);
	jclass doubleClazz = env->FindClass("java/lang/Double");
	assure(doubleClazz != NULL, env);

	jmethodID cons = env->GetMethodID(doubleClazz, "<init>", "(D)V");
	assure(cons != NULL, env);

	jobjectArray dArray = env->NewObjectArray(size, doubleClazz, NULL);
	assure(dArray != NULL, env);
	try {
		for (int i = 0; i < size; ++i) {
			double d = arr[i];
			jobject dd = env->NewObject(doubleClazz, cons, (jdouble)d);
			assure(dd != NULL, env);
			env->SetObjectArrayElement(dArray, i, dd);

		}
	} catch (std::out_of_range exc) {
		std::cerr << exc.what() << std::endl;
	}
	jclass arraysCls = env->FindClass("java/util/Arrays");
	assure(arraysCls != NULL, env);
	jmethodID mid = env->GetStaticMethodID(arraysCls, "asList",
										"([Ljava/lang/Object;)Ljava/util/List;");
	assure(mid != NULL, env);
	jobject iterable = env->CallStaticObjectMethod(arraysCls, mid, dArray);
	assure(iterable != NULL, env);
	return iterable;
}

/**
 * convert CPP::map<int, map<int, vector<double> > > to
 * java::HashMap<Integer, HashMap<Integer, Iterable<Double>>>
 */
template<>
jobject convertToJObject(JNIEnv *env, const std::map<int, std::map<int, std::vector<double> > >& hashmap) {
	std::cout << "convertToJObject." << std::endl;

	jclass hmcls = env->FindClass("java/util/HashMap");
	assure(hmcls != NULL, env);
	jmethodID constructor = env->GetMethodID(hmcls, "<init>", "()V");
	assure(constructor != NULL, env);
	jobject hmObj = env->NewObject(hmcls, constructor);
	assure(hmObj != NULL, env);
	jmethodID putMid = env->GetMethodID(hmcls, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	assure(putMid != NULL, env);
	std::map<int, std::map<int, std::vector<double> > >::const_iterator iter;
	for (iter = hashmap.begin(); iter != hashmap.end(); ++iter) {
		int mkey = iter->first;
		std::map<int, std::vector<double> > tmp = iter->second;
		std::map<int, std::vector<double> >::iterator tmpiter;
		jobject tmpobj = env->NewObject(hmcls, constructor);
		assure(tmpobj != NULL, env);
		for (tmpiter = tmp.begin(); tmpiter != tmp.end(); ++ tmpiter) {
			int key = tmpiter->first;
			std::vector<double> value = tmpiter->second;
			std::list<double> ll(value.begin(), value.end());

			jobject v = convertToJObject<std::list<double> >(env, ll);
			assure(v != NULL, env);

			env->CallObjectMethod(tmpobj, putMid, convertToJObject<int>(env,key), v);
		}
		tmp.clear();

		env->CallObjectMethod(hmObj, putMid, convertToJObject<int>(env,mkey), tmpobj);
	}
	//hashmap.clear();
	return hmObj;
}

/**
 * convert cpp::map<int,vector<double>> to java::HashMap<Integer, Iterable<Double>>
 */
template<>
jobject convertToJObject(JNIEnv* env, const std::map<int, std::vector<double> >& mm) {
	jclass hashmapcls = env->FindClass("java/util/HashMap");
	assure(hashmapcls != NULL, env);

	jmethodID cons = env->GetMethodID(hashmapcls, "<init>", "()V");
	assure(cons != NULL, env);

	jobject hashMap = env->NewObject(hashmapcls, cons);
	assure(hashMap != NULL, env);

	jmethodID putMid = env->GetMethodID(hashmapcls, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	assure(putMid != NULL, env);

	std::map<int, std::vector<double> >::const_iterator iter;
	for (iter = mm.begin(); iter != mm.end(); ++iter) {
		int index = iter->first;
		std::vector<double> v = iter->second;
		std::list<double> ll(v.begin(), v.end());
		jobject vec = convertToJObject<std::list<double> >(env, ll);
		assure(vec != NULL, env);

		jobject integer = convertToJObject<int>(env, index);

		jobject obj = env->CallObjectMethod(hashMap, putMid, integer, vec);
	}

	return hashMap;
}

template<>
int convertFromJObject(JNIEnv* env, jobject jobj) {
	jclass integercls = env->GetObjectClass(jobj);
	assure(integercls != NULL, env);
	jmethodID mid = env->GetMethodID(integercls, "intValue", "()I");
	assure(mid != NULL, env);
	int intValue = env->CallIntMethod(jobj, mid);
	assure(intValue != 0, env);
	return intValue;
}

/**
 * convert java Double to cpp double
 */
template<>
double convertFromJObject(JNIEnv* env, jobject jobj) {
	jclass doubleCls = env->GetObjectClass(jobj);
	assure(doubleCls != NULL, env);

	jmethodID doubleValue = env->GetMethodID(doubleCls, "doubleValue", "()D");
	assert(doubleValue != NULL);

	double x1 = env->CallDoubleMethod(jobj, doubleValue);

	return x1;
}

/**
 * convert java String to cpp string
 */
template<>
std::string convertFromJObject(JNIEnv* env, jobject str) {
	/*jclass strCls = env->GetObjectClass(str);
	assure(strCls != NULL, env);
	jmethodID mid = env->GetMethodID(strCls, "toString", "()Ljava/lang/String;");
	assure(mid != NULL, env);
 	jstring jstr = (jstring)env->CallObjectMethod(str, mid);*/
	jstring jstr = (jstring)str;
	const char* s = env->GetStringUTFChars(jstr, NULL);
	assure(s != NULL, env);
	string result(s);
	env->ReleaseStringUTFChars(jstr, s);
	return result;
}

/**
 * convert scala Tuple2 to cpp map
 */
template<>
std::map<string, int> convertFromJObject(JNIEnv*env, jobject tuple2){
	jclass tuplecls = env->GetObjectClass(tuple2);
	assure(tuplecls != NULL, env);

	jfieldID _1 = env->GetFieldID(tuplecls, "_1", "Ljava/lang/Object;");
	assure(_1 != NULL, env);

	jfieldID _2 = env->GetFieldID(tuplecls, "_2", "Ljava/lang/Object;");
	assure(_2 != NULL, env);

	jobject str = env->GetObjectField(tuple2, _1); // return tuple._1 => String
	assure(str != NULL, env);

	jobject integer = env->GetObjectField(tuple2, _2);// return tuple._2 =>Integer
	assure(integer != NULL, env);

	std::map<string, int> result;

	result.insert(
		std::pair<string, int>(convertFromJObject <string> (env, str),
				convertFromJObject<int>(env, integer)));

	return result;
}

/**
 * convert java iterable<double> to cpp vector<double>
 */
template<>
std::vector<double> convertFromJObject(JNIEnv* env, jobject iterable) {
	jclass iterablecls = env->GetObjectClass(iterable);
	assert(iterablecls != NULL);

	jmethodID iterator = env->GetMethodID(iterablecls, "iterator",
			"()Ljava/util/Iterator;");
	assert(iterator != NULL);

	jobject jiterator = env->CallObjectMethod(iterable, iterator);
	assert(jiterator != NULL);

	jclass iteratorcls = env->GetObjectClass(jiterator);
	assert(iteratorcls != NULL);

	jmethodID hasNext = env->GetMethodID(iteratorcls, "hasNext", "()Z");
	assert(hasNext != NULL);

	jmethodID next = env->GetMethodID(iteratorcls, "next",
			"()Ljava/lang/Object;");
	assert(next != NULL);
	std::vector<double> vtr;
	while (env->CallBooleanMethod(jiterator, hasNext)) {
		jobject x1Obj = env->CallObjectMethod(jiterator, next);
		assert(x1Obj != NULL);

		double x1 = convertFromJObject<double>(env, x1Obj); //env->CallDoubleMethod(x1Obj, doubleValue);
		vtr.push_back(x1);
	}
	return vtr;
}

/**
 * convert java iterable<String> to cpp vector<string>
 */
template<>
std::vector<std::string> convertFromJObject(JNIEnv* env, jobject iterable) {
	jclass iterablecls = env->GetObjectClass(iterable);
	assert(iterablecls != NULL);

	jmethodID iterator = env->GetMethodID(iterablecls, "iterator",
			"()Ljava/util/Iterator;");
	assert(iterator != NULL);

	jobject jiterator = env->CallObjectMethod(iterable, iterator);
	assert(jiterator != NULL);

	jclass iteratorcls = env->GetObjectClass(jiterator);
	assert(iteratorcls != NULL);

	jmethodID hasNext = env->GetMethodID(iteratorcls, "hasNext", "()Z");
	assert(hasNext != NULL);

	jmethodID next = env->GetMethodID(iteratorcls, "next",
			"()Ljava/lang/Object;");
	assert(next != NULL);
	std::vector<std::string> vtr;
	while (env->CallBooleanMethod(jiterator, hasNext)) {
		jobject x1Obj = env->CallObjectMethod(jiterator, next);
		assert(x1Obj != NULL);

		string x1 = convertFromJObject<string>(env, x1Obj); //env->CallDoubleMethod(x1Obj, doubleValue);
		vtr.push_back(x1);
	}
	return vtr;
}

/**
 * convert java::Iterable<double> to cpp::list<double>
 */
template<>
std::list<double> convertFromJObject(JNIEnv* env, jobject iterable) {
	std::list<double> result;

	jclass iterablecls = env->GetObjectClass(iterable);
	assert(iterablecls != NULL);

	jmethodID iterator = env->GetMethodID(iterablecls, "iterator",
			"()Ljava/util/Iterator;");
	assert(iterator != NULL);

	jobject jiterator = env->CallObjectMethod(iterable, iterator);
	assert(jiterator != NULL);

	jclass iteratorcls = env->GetObjectClass(jiterator);
	assert(iteratorcls != NULL);

	jmethodID hasNext = env->GetMethodID(iteratorcls, "hasNext", "()Z");
	assert(hasNext != NULL);

	jmethodID next = env->GetMethodID(iteratorcls, "next",
			"()Ljava/lang/Object;");
	assert(next != NULL);

	while (env->CallBooleanMethod(jiterator, hasNext)) {
		jobject x1Obj = env->CallObjectMethod(jiterator, next);
		assert(x1Obj != NULL);

		double x1 = convertFromJObject<double>(env, x1Obj); //env->CallDoubleMethod(x1Obj, doubleValue);
		result.push_back(x1);
	}
	return result;

	return result;
}

/**
 * convert Java::HashMap<Integer, Iterable<Double>> to cpp:: map<int, vector<double>>
 */
template<>
std::map<int, std::vector<double> > convertFromJObject(JNIEnv* env,
		jobject hashmap/*java:HashMap<Integer, Iterable<Double>>*/) {
	std::map<int, std::vector<double> > result;

	jclass clazz = env->GetObjectClass(hashmap);
	assert(clazz != NULL);
	// Set entrySet = map.entrySet();
	jmethodID entrySet = env->GetMethodID(clazz, "entrySet",
			"()Ljava/util/Set;");
	assert(entrySet != NULL);

	jobject jentrySet = env->CallObjectMethod(hashmap, entrySet);
	assert(jentrySet != NULL);

	clazz = env->GetObjectClass(jentrySet);
	assert(jentrySet != NULL);
	// Iterator iterator = entrySet.iterator();
	jmethodID iterator = env->GetMethodID(clazz, "iterator",
			"()Ljava/util/Iterator;");
	assert(iterator != NULL);

	jobject jiterator = env->CallObjectMethod(jentrySet, iterator);
	assert(jiterator != NULL);

	clazz = env->GetObjectClass(jiterator);
	assert(clazz != NULL);

	// while (iterator.hasNext()) {
	jmethodID hasNext = env->GetMethodID(clazz, "hasNext", "()Z");
	assert(hasNext != NULL);

	jmethodID next = env->GetMethodID(clazz, "next", "()Ljava/lang/Object;");
	assert(next != NULL);

	while (env->CallBooleanMethod(jiterator, hasNext)) {
		// Map.Entry entry = iterator.next();
		jobject jentry = env->CallObjectMethod(jiterator, next);
		assure(jentry != NULL, env);

		clazz = env->GetObjectClass(jentry);
		assure(clazz != NULL, env);
		// String key = entry.getKey();
		jmethodID getKey = env->GetMethodID(clazz, "getKey",
				"()Ljava/lang/Object;");
		assure(getKey != NULL, env);

		jobject jkey = env->CallObjectMethod(jentry, getKey);
		assure(jkey != NULL, env);

		// String value = entry.getValue();
		jmethodID getValue = env->GetMethodID(clazz, "getValue",
				"()Ljava/lang/Object;");
		assure(getValue != NULL, env);

		jobject jvalue = env->CallObjectMethod(jentry, getValue);
		assure(jvalue != NULL, env);

		/*jclass test = env->FindClass("java/lang/Iterable");
		if (env->IsInstanceOf(jvalue, test)) {
			std::cout << "in convert OK ,Iterable class" << std::endl;
		} else {
			std::cout << "in convert Bad ,not Iterable class" << std::endl;
		}
*/
		int key = convertFromJObject<int>(env, jkey);

		std::vector<double> value = convertFromJObject < std::vector<double> > (env, jvalue);

		result[key] = value;
	}

	return result;
}


/**
 * convert Java::HashMap<Integer, HashMap<Integer, Iterable<Double>>> to C++::map<int, map<int, vector<double>>>
 */
template<>
std::map<int, std::map<int, std::vector<double> > > convertFromJObject(
		JNIEnv* env,
		jobject hashmap/*HashMap<Integer, HashMap<Integer, Iterable<Double>>>*/) {
	std::map<int, std::map<int, std::vector<double> > > result;

	jclass clazz = env->GetObjectClass(hashmap);
	assert(clazz != NULL);
	// Set entrySet = map.entrySet();
	jmethodID entrySet = env->GetMethodID(clazz, "entrySet",
			"()Ljava/util/Set;");
	assert(entrySet != NULL);

	jobject jentrySet = env->CallObjectMethod(hashmap, entrySet);
	assert(jentrySet != NULL);

	clazz = env->GetObjectClass(jentrySet);
	assert(clazz != NULL);

	// Iterator iterator = entrySet.iterator();
	jmethodID iterator = env->GetMethodID(clazz, "iterator",
			"()Ljava/util/Iterator;");
	assert(iterator != NULL);

	jobject jiterator = env->CallObjectMethod(jentrySet, iterator);
	assert(jiterator != NULL);

	clazz = env->GetObjectClass(jiterator);
	assert(clazz != NULL);

	// while (iterator.hasNext()) {
	jmethodID hasNext = env->GetMethodID(clazz, "hasNext", "()Z");
	assert(hasNext != NULL);

	jmethodID next = env->GetMethodID(clazz, "next", "()Ljava/lang/Object;");
	assert(next != NULL);

	while (env->CallBooleanMethod(jiterator, hasNext)) {
		// Map.Entry entry = iterator.next();
		jobject jentry = env->CallObjectMethod(jiterator, next);
		assert(jentry != NULL);

		clazz = env->GetObjectClass(jentry);
		assert(clazz != NULL);

		// String key = entry.getKey();
		jmethodID getKey = env->GetMethodID(clazz, "getKey",
				"()Ljava/lang/Object;");
		assert(getKey != NULL);
		jobject jkey = env->CallObjectMethod(jentry, getKey);
		assert(jkey != NULL);

		// String value = entry.getValue();
		jmethodID getValue = env->GetMethodID(clazz, "getValue",
				"()Ljava/lang/Object;");
		assert(getValue != NULL);
		jobject jvalue = env->CallObjectMethod(jentry, getValue);
		assert(jvalue != NULL);

		std::map<int, std::vector<double> > tmp = convertFromJObject<std::map<int, std::vector<double> > >(env, jvalue);

		result[convertFromJObject<int>(env, jkey)] = tmp;
	}

	return result;
}

/**
 * convert scala::Tuple2<Integer, Iterable<Double>> to cpp::map<int, list<double>>
 */

template<>
std::map<int, std::list<double> > convertFromJObject(JNIEnv* env, jobject tuple2) {
	std::map<int, std::list<double> > result;

	jclass tuplecls = env->GetObjectClass(tuple2);
	assure(tuplecls != NULL, env);

	jfieldID _1 = env->GetFieldID(tuplecls, "_1", "Ljava/lang/Object;"); //Integer
	assure(_1 != NULL, env);

	jfieldID _2 = env->GetFieldID(tuplecls, "_2", "Ljava/lang/Object;"); // Vector<Double>
	assure(_2 != NULL, env);

	jobject integer = env->GetObjectField(tuple2, _1);
	assert(integer != NULL);

	jobject jvector = env->GetObjectField(tuple2, _2);
	assert(jvector != NULL);

	int mkey = convertFromJObject<int>(env, integer);

	// convert Java::Iterable<Double> to C++::list<double>
	std::list<double> mval = convertFromJObject<std::list<double> >(env, jvector);

	result[mkey] = mval;

	return result;
}


