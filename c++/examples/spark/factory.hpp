#ifndef __FACTORY_HPP__
#define __FACTORY_HPP__

#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;

/*class Mapper;
class FlatMapper;
class PairMapper;
class ReduceByKey;*/

class Factory{
public:
	Factory(){}
	virtual ~Factory(){
		
	}

	virtual std::vector<std::string> flatMap(const std::string& t) {
		cout << "in Factory class : flatMap" << endl;
	}

	virtual int reduceByKey (const int& t1, const int& t2) {
		cout << "in Factory class : reduceByKey" << endl;
	}

	virtual std::map<std::string, int> pairMap(const std::string& t) {
		cout << "in Factory class : pairMap" << endl;
	}
};

/*typedef Mapper* createMapper_t();
typedef FlatMapper* createFlatMapper_t();
typedef PairMapper* createPairMapper_t();
typedef ReduceByKey* createReduceByKey_t();*/
//typedef Factory* create_t();
//typedef void destroy_t(Factory* f);

#endif //
