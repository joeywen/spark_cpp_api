#include "spark/csparkcontext.hpp"
#include "spark/crdd.hpp"
#include "spark/cpairrdd.hpp"
#include "spark/Class.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cassert>
#include <string.h>
#include <search.h>
#include <pthread.h>
#include <errno.h>
#include <stdexcept>
#include <exception>

using namespace std;

static pthread_mutex_t hashMutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_HASH_MAP() pthread_mutex_lock(&hashMutex)
#define UNLOCK_HASH_MAP() pthread_mutex_unlock(&hashMutex)

class FlatMapper : public FlatMapContext<string, string> {
	DYNAMIC_DELARE()
	public:
		FlatMapper(){};
		~FlatMapper(){}

		virtual vector<string> call(const string& str) {
			vector<string> vec;
			string substring;
			string::size_type start = 0, index;

			do {
				index = str.find_first_of(" ", start);
				if (index != string::npos) {
					try {
						substring = str.substr(start, index - start);
						vec.push_back(substring);
						start = str.find_first_not_of(" ", index);
						if (start == string::npos) {
							break;
						}
					} catch(std::out_of_range exc) {
					}
				}
			} while(index != string::npos);

			try {
				substring = str.substr(start);
				vec.push_back(substring);
			} catch(std::out_of_range exc) {

			}
			return vec;
		}
};

DYNAMIC_IMPLEMENT(FlatMapper, "FlatMapContext<std::string, string>")

class PairMapper : public PairMapContext<string, string, int>  {
	DYNAMIC_DELARE()

public:
	PairMapper(){}
	~PairMapper(){}

	virtual map<string, int> call(const string& key) {
		LOCK_HASH_MAP();
		map<string, int> maplive;
		maplive.insert(make_pair(key, 1));

		UNLOCK_HASH_MAP();
		return maplive;
	}
};

DYNAMIC_IMPLEMENT(PairMapper, "PairMapContext<string, string, int>")

class ReduceByKey : public ReduceContext<int, int, int>  {
	DYNAMIC_DELARE()

public:
	ReduceByKey(){}
	~ReduceByKey(){}

	virtual int call(const int& i1, const int& i2) {
		return (i1 + i2);
	}
};

DYNAMIC_IMPLEMENT(ReduceByKey, "ReduceContext<int, int, int>")

int main(int argc, char** args) {
	// std::string str(buf, std::find(buf, buf + l, '\0'));
	if (argc < 3) {
		cout << "Usage: <master> <jobname> <file>" << endl;
		return 0;
	}

	string master = args[1];
	string jobname = args[2];
	string path = args[3];
	
	CSparkContext csc(master, jobname);

	CRDD rdd = csc.textFile(path);
	CRDD cache = rdd.cache();

	cout << "1...." << endl;
	FlatMapper flatmap;
	CRDD flatmaprdd = cache.flatMap<string, string>(flatmap);
	flatmaprdd.collect<string>();

	cout << "2...." << endl;
	PairMapper pairmap;
	CPairRDD maprdd = flatmaprdd.pairMap<string ,string, int>(pairmap);
	vector<map<string, int> > vv = maprdd.collect<std::map<std::string, int> >();

	vector<map<string, int> >::iterator vviter;
	for (vviter = vv.begin(); vviter != vv.end(); ++vviter) {
		map<string, int> mmm = *vviter;
		map<string, int>::iterator mmiter;
		for (mmiter = mmm.begin(); mmiter != mmm.end(); ++mmiter) {
			cout << mmiter->first << " : " << mmiter->second << endl;
		}
	}

	cout << "3...." << endl;
	ReduceByKey reduce;
	CPairRDD reducerdd = maprdd.reduceByKey<int, int, int>(reduce);

	std::vector<std::map<std::string, int> > output = reducerdd.collect<std::map<std::string, int> >();

	vector<map<string, int> >::iterator viter;
	for (viter = output.begin(); viter != output.end(); ++viter) {
		map<string, int> mm = *viter;
		map<string, int>::iterator miter;
		for (miter = mm.begin(); miter != mm.end(); ++miter) {
			cout << miter->first << " : " << miter->second << endl;
		}
	}
	cout << "end !" << endl;
	return 0;
}
