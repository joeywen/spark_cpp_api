#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cassert>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <pthread.h>
#include <errno.h>
#include <stdexcept>
#include <exception>
#include <typeinfo>
#include <list>

#include "spark/csparkcontext.hpp"
#include "spark/crdd.hpp"
#include "spark/cpairrdd.hpp"
#include "spark/Class.h"

using namespace std;

static map<int,vector<double> > kpoint;

static pthread_mutex_t hashMutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_HASH_MAP() pthread_mutex_lock(&hashMutex)
#define UNLOCK_HASH_MAP() pthread_mutex_unlock(&hashMutex)

class Mapper : public MapContext<string, vector<double> >  {
	DYNAMIC_DELARE();
public:

	virtual vector<double> call(const string& str) {
		vector<double> vec;
		string substring;
		string::size_type start = 0, index;

		do {
			index = str.find_first_of(" ", start);
			if (index != string::npos) {
				try {
					substring = str.substr(start, index - start);
					vec.push_back(atof(substring.c_str()));
					start = str.find_first_not_of(" ", index);
					if (start == string::npos) {
						break;
					}
				} catch(std::out_of_range exc) {
				}
			}
		}while(index != string::npos);

		try {
			substring = str.substr(start);
			vec.push_back(atof(substring.c_str()));
		} catch(std::out_of_range exc) {
			cout << "out_of_range : " << exc.what() << endl;
		}
		return vec;
	}
};

DYNAMIC_IMPLEMENT(Mapper, "MapContext<string, vector<double> >")

class PairMapper1 : public PairMapContext<vector<double>, int, map<int, vector<double> > > {
	DYNAMIC_DELARE();
public:

	virtual map<int, map<int, vector<double> > > call(const vector<double>& v) {
		LOCK_HASH_MAP();
		map<int, map<int, vector<double> > > result;

		int bestIndex = 0;
		double closest = 1000.0;

		map<int, vector<double> >::iterator kiter;
		for (kiter = kpoint.begin(); kiter != kpoint.end(); ++kiter) {
			double tempDist = 0.0;

			vector<double> kp = kiter->second;
			vector<double>::iterator kpiter;
			vector<double>::const_iterator viter;

			int i;
			for (i = 1, kpiter = kp.begin(), viter = v.begin(); kpiter != kp.end() && viter != v.end();
					++kpiter, ++viter, ++i) {
				double x1 = *kpiter;
				double x2 = *viter;

				tempDist += (x1 - x2) * (x1 - x2);
			}

			if (tempDist < closest) {
				closest = tempDist;
				bestIndex = i;
			}
		}
		map<int, vector<double> > tmp;
		tmp.insert(make_pair(1, v));
		result.insert(make_pair(bestIndex, tmp));

		tmp.clear();
		UNLOCK_HASH_MAP();
		return result;
	}
};

DYNAMIC_IMPLEMENT(PairMapper1,"PairMapContext<vector<double>, int, map<int, vector<double> > >")

class PairMapper2 : public PairMapContext<map<int, map<int, vector<double> > >, int, vector<double> > {
	DYNAMIC_DELARE();
public:
	virtual map<int, vector<double> > call(const map<int, map<int, vector<double> > >& mm) {
		LOCK_HASH_MAP();
		map<int, map<int, vector<double> > >::const_iterator miter;

		map<int, vector<double> > result;
		int index = 0;
		vector<double> v;

		for (miter = mm.begin(); miter != mm.end(); ++miter) {
			index = miter->first;
			map<int, vector<double> > mv = miter->second;
			map<int, vector<double> >::iterator iter;
			for (iter = mv.begin(); iter != mv.end(); ++iter) {
				int index = iter->first;
				vector<double> tmp = iter->second;
				vector<double>::iterator tmpiter;
				for (tmpiter = tmp.begin(); tmpiter != tmp.end(); ++tmpiter) {
					double d = *tmpiter / index;
					v.push_back(d);
				}
			}
		}

		result.insert(make_pair(index, v));

		UNLOCK_HASH_MAP();
		return result;
	}

};

DYNAMIC_IMPLEMENT(PairMapper2, "PairMapContext<map<int, map<int, vector<double> > >, int, vector<double> >")

class ReduceByKey : public ReduceContext<map<int, vector<double> >, map<int, vector<double> >, map<int, vector<double> > > {
	DYNAMIC_DELARE();
public:
	virtual map<int, vector<double> > call(const map<int, vector<double> >& m1,
			const map<int, vector<double> >& m2) {
		LOCK_HASH_MAP();
		map<int, vector<double> > result;
		map<int, vector<double> >::const_iterator m1iter;
		map<int, vector<double> >::const_iterator m2iter;

		for (m1iter = m1.begin(), m2iter = m2.begin();
				m1iter != m1.end() && m2iter != m2.end();
				++m1iter, ++m2iter) {
			int index = m1iter->first + m2iter->first;

			vector<double> v1 = m1iter->second;
			vector<double> v2 = m2iter->second;

			vector<double>::iterator v1iter;
			vector<double>::iterator v2iter;
			vector<double> tmp;

			for (v1iter = v1.begin(), v2iter = v2.begin();
					v1iter != v1.end() && v2iter != v2.end();
					++v1iter, ++v2iter) {
				tmp.push_back(*v1iter + *v2iter);
			}

			result.insert(make_pair(index, tmp));
			//tmp.clear();
		}
		UNLOCK_HASH_MAP();
		return result;
	}
};
DYNAMIC_IMPLEMENT(ReduceByKey, "ReduceContext<map<int, vector<double> >, map<int, vector<double> >, map<int, vector<double> > >")

int main(int argc, char** args) {
	if (argc < 5) {
		cout << "Usage: <master> <jobname> <file> <k> <converge>" << endl;
		return -1;
	}

	string master = args[1];
	string jobname = args[2];
	string file = args[3];
	int k = atoi(args[4]);
	double converge = atof(args[5]);

	double tmpDist = 1.0;

	CSparkContext scs(master, jobname);

	CRDD rdd = scs.textFile(file);
	cout << "==================> map <========================" << endl;

	// JavaRDD<Iterable<Double>> = rdd.map(Functions);
	Mapper mapper;
	PairMapper1 pair1;
	PairMapper2 pair2;
	ReduceByKey reduce;

	CRDD rdd2 = rdd.map<string, vector<double> >(mapper);
	rdd2.collect<vector<double> >();

	rdd2.cache();

	cout << "==================> takeSample <========================" << endl;
	list<vector<double> > v = rdd2.takeSample<double>(false, k, 42);
	int e = 1;
	//map<int, vector<double> > kpoint;
	list<vector<double> >::iterator iter;

	for (iter = v.begin(); iter != v.end(); ++iter){
		cout << e << " ==> " << endl;
		kpoint.insert(make_pair(e++, *iter));
		vector<double> vv = *iter;
		vector<double>::iterator viter;
		for (viter = vv.begin(); viter != vv.end(); ++viter) {
			cout << *viter << "\t";
		}
		cout << endl;
	}
	v.clear();

	cout << "==================> start to iterate <========================" << endl;
	while (tmpDist > converge) {
		// returned JavaPairRDD<> Tuple2<int, HashMap<int, vector<double>>>
		cout << "iterator step 1 ..." << endl;
		CPairRDD rdd3 = rdd2.pairMap<vector<double>, int, map<int, vector<double> > >(pair1);
		rdd3.collect<map<int, map<int, vector<double> > > >();

		cout << "iterator step 2 ..." << endl;
		CPairRDD rdd4 = rdd3.reduceByKey<map<int, vector<double> >, map<int, vector<double> >, map<int, vector<double> > >(reduce);

		rdd4.collect<map<int, map<int, vector<double> > > >();

		cout << "iterator step 3 ..." << endl;
		CPairRDD rdd5  = rdd4.pairMap<map<int, map<int, vector<double> > >, int, vector<double> >(pair2);

		vector<map<int, list<double> > > newkpoint = rdd5.collect<map<int, list<double> > >();
		cout << "iterator step 4 ..." << endl;
		vector<map<int, list<double> > >::iterator iter;

		cout << "iterator step 5 ..." << endl;

		tmpDist = 0.0;
		map<int,vector<double> > tmp = kpoint;
		kpoint.clear();

		for (iter = newkpoint.begin(); iter != newkpoint.end(); ++iter) {
			map<int, list<double> > mm = *iter;
			map<int, list<double> >::iterator newiter;
			map<int, vector<double> >::iterator olditer;
			for (newiter = mm.begin(), olditer = tmp.begin();
					newiter != mm.end() && olditer != tmp.end();
					++newiter, ++olditer) {
				vector<double> oldv = olditer->second;
				list<double> newv = newiter->second;

				vector<double>::iterator oviter;
				list<double>::iterator nviter;
				for (oviter = oldv.begin(), nviter = newv.begin();
						oviter != oldv.end() && nviter != newv.end();
						++oviter, ++nviter) {
					double x1 = *oviter;
					double x2 = *nviter;

					tmpDist += (x1 - x2) * (x1 - x2);
					cout << "==========================> tmpDist : " << tmpDist << endl;
				}

				vector<double> v(newv.begin(), newv.end());
				kpoint.insert(make_pair(newiter->first, v));
			}
		}

	}

	cout << "result :" << endl;
	map<int, vector<double> >::iterator mmiter;
	for (mmiter = kpoint.begin(); mmiter != kpoint.end(); ++mmiter) {
		cout << mmiter->first << ":\t";
		vector<double> v = mmiter->second;
		vector<double>::iterator viter;
		for (viter = v.begin(); viter != v.end(); ++viter) {
			cout << *viter << ",";
		}
		cout << endl;
	}

	return 0;
}


