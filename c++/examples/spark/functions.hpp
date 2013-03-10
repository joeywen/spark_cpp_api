#ifndef __FUNCTIONS_HPP__
#define __FUNCTIONS_HPP__

#include <iostream>
#include <map>
#include <vector>

//#include "spark/Class.h"
#include "spark/factory.hpp"

class Mapper;
class FlatMapper;
class PairMapper;
class ReduceByKey;

template<class T, class R>
class MapContext : public Factory {
	public:
		MapContext(){}
		virtual ~MapContext() {}
		virtual R call(const T& r) = 0;
};

template<class T, class R>
class FlatMapContext : public Factory {
	public:
		FlatMapContext(){}
		virtual ~FlatMapContext() {}

		virtual std::vector<R> call(const T& t) = 0;
};

template<class T, class K, class V>
class PairMapContext : public Factory {
	public:
		PairMapContext(){}
		virtual ~PairMapContext(){}

		virtual std::map<K, V> call(const T& t) = 0;
};

template<class T1, class T2, class R>
class ReduceContext : public Factory {
	public:
		ReduceContext(){}
		virtual ~ReduceContext(){}

		virtual R call(const T1& t1, const T2& t2) = 0;
};

/*typedef Mapper* createMapper_t();
typedef FlatMapper* createFlatMapper_t();
typedef PairMapper* createPairMapper_t();
typedef ReduceByKey* createReduceByKey_t();
typedef void destroy_t(Factory* f);*/

#endif //
