#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typeinfo>
#include <search.h>
#include <pthread.h>
#include <errno.h>
#include <stdexcept>
#include <exception>

using namespace std;

static pthread_mutex_t deleteMutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_DELETE() pthread_mutex_lock(&deleteMutex)
#define UNLOCK_DELETE() pthread_mutex_unlock(&deleteMutex)

template<class T, class R>
class MapContext {
	public:
		MapContext(){}
		virtual ~MapContext() {}
		virtual R callMap(const T& r) = 0;
};

template<class T, class R>
class FlatMapContext {
	public:
		FlatMapContext(){}
		virtual ~FlatMapContext() {}

		virtual std::vector<R> callFlatMap(const T& t) = 0;
};

template<class T, class K, class V>
class PairMapContext {
	public:
		PairMapContext(){}
		virtual ~PairMapContext(){}

		virtual std::map<K, V> callPairMap(const T& t) = 0;
};

template<class T1, class T2, class R>
class ReduceContext {
	public:
		ReduceContext(){}
		virtual ~ReduceContext(){}

		virtual R callReduce(const T1& t1, const T2& t2) = 0;
};

template<class T>
class Attachment {
	public:
		Attachment(){};
		virtual ~Attachment(){};
		inline void setAttachment(const T& t){
			this->_attach = t;
		}

	public:
		T _attach;
};


class Object {
private:
	Object* _handler;
public:
	Object(){}
	virtual ~Object(){}
	
	inline void setHandler(Object* obj) {
		_handler = obj;
	}

	template<class T>
	void setAttach(const T& t) {
		cout << "in Object classs" << endl;
		assert(_handler != NULL);
		Attachment<T>* attach = dynamic_cast<Attachment<T>* >(_handler);
		cout << "in Object classs after dynamic" << endl;
		attach->setAttachment(t);
	}

	template<class T, class R>
	R callMap(const T& t) {
		MapContext<T,R>* ctx = dynamic_cast<MapContext<T,R>* >(_handler);
		assert(ctx != NULL);
		
		return ctx->callMap(t);
	}
	
	template<class T, class R>
	vector<R> callFlatMap(const T& t) {
		FlatMapContext<T,R>* ctx = dynamic_cast<FlatMapContext<T,R>* >(_handler);
		assert(ctx != NULL);
				
		return ctx->callFlatMap(t);
	}
	
	template<class T, class K, class V>
	map<K, V> callPairMap(const T& t) {
		assert(_handler != NULL);
		PairMapContext<T,K,V>* ctx = dynamic_cast<PairMapContext<T,K,V>* >(_handler);
		assert(ctx != NULL);
						
		return ctx->callPairMap(t);		
	}
	
	template<class T1, class T2, class R>
	R callReduce(const T1& t1, const T2& t2) {
		ReduceContext<T1, T2, R>* ctx = dynamic_cast<ReduceContext<T1,T2,R>* >(_handler);
		assert(ctx != NULL);
		
		return ctx->callReduce(t1, t2);
	}

};


typedef Object* createMap_t();
typedef Object* createFlatMap_t();
typedef Object* createPairMap_t();
typedef Object* createPairMap1_t();
typedef Object* createPairMap2_t();
typedef Object* createReduce_t();
typedef void destroy_t(Object*);

#endif // __OBJECT_HPP__
