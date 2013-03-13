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
#include <algorithm>
#include <list>

#include <stdio.h>
#include <string.h>

#include "spark/mapping.hpp"
#include "spark/convert.hpp"
#include "spark/Class.h"

using namespace std;


void get_nextval(char const* ptrn, int plen, int* nextval) {
	int i = 0;
	nextval[i] = -1;
	int j = -1;

	while (i < plen - 1) {
		if (j == -1 || ptrn[i] == ptrn[j]) {
			++i;++j;

			if (ptrn[i] != ptrn[j])
				nextval[i] = j;
			else
				nextval[i] = nextval[j];
		} else
			j = nextval[j];
	}
}

int kmp_search(const char * src, int slen, const char * patn, int plen, int pos) {
	int* nextval = new int[plen];
	get_nextval(patn, plen, nextval);

	int i = pos;
	int j = 0;

	while (i < slen && j < plen) {
		if (j == -1 || src[i] == patn[j]) {
			++i;++j;
		} else {
			j = nextval[j];
		}
	}

	delete nextval;

	if (j >= plen) {
		return i - plen;
	} else {
		return -1;
	}
}

/* Compute the next maximal suffix. */
void nextMaximalSuffix(const char *x, int m, int *i, int *j, int *k, int *p) {
	char a, b;

	while (*j + *k < m) {
		a = x[*i + *k];
		b = x[*j + *k];
		if (a == b)
			if (*k == *p) {
				(*j) += *p;
				*k = 1;
			} else
				++(*k);
		else if (a > b) {
			(*j) += *k;
			*k = 1;
			*p = *j - *i;
		} else {
			*i = *j;
			++(*j);
			*k = *p = 1;
		}
	}
}
int SMOA(const char *x, int m, const char *y, int n) {
	int i, ip, j, jp, k, p;

	/* Searching */
	ip = -1;
	i = j = jp = 0;
	k = p = 1;
	while (j <= n - m) {
		while (i + j < n && i < m && x[i] == y[i + j])
			++i;
		if (i == 0) {
			++j;
			ip = -1;
			jp = 0;
			k = p = 1;
		} else {
			if (i >= m)
				return j;
			nextMaximalSuffix(y + j, i + 1, &ip, &jp, &k, &p);
			if (ip < 0 || (ip < p && memcmp(y + j, y + j + p, ip + 1) == 0)) {
				j += p;
				i -= p;
				if (i < 0)
					i = 0;
				if (jp - ip > p)
					jp -= p;
				else {
					ip = -1;
					jp = 0;
					k = p = 1;
				}
			} else {
				j += (max(ip + 1, min(i - ip - 1, jp + 1)) + 1);
				i = jp = 0;
				ip = -1;
				k = p = 1;
			}
		}
	}
}

jobject mapping(JNIEnv *env, Object* obj, string super, jstring str) {
	string pattern("FlatMapContext<string,string>");

	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		FlatMapContext<string,string>* ctx = dynamic_cast<FlatMapContext<string,string>* >(obj);
		assert(ctx);

		vector<string> v = ctx->call(jstrTocstr(env, str));
		return convertToJObject<vector<string> >(env, v);
	}

	pattern = "FlatMapContext<string,double>";
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		FlatMapContext<string, double>* ctx = dynamic_cast<FlatMapContext<string, double>*>(obj);
		assert(ctx);
		return convertToJObject < vector<double> > (env, ctx->call(jstrTocstr(env, str)));
	}

	cerr << "in mapping jstring, fatal error ==> return NULL, superclass is " << super <<  endl;
	return NULL;
}

jobject mapping(JNIEnv* env, Object* obj,string super, jobject jobj) {
	string pattern("PairMapContext<string,string,int>");
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		PairMapContext<string,string,int>* ctx = dynamic_cast<PairMapContext<string,string,int>* >(obj);
		assert(ctx != NULL);

		return convertToJObject<map<string, int> >(env, ctx->call(convertFromJObject<string>(env, jobj)));
	}

	pattern = "PairMapContext<vector<double>,int,map<int,vector<double>>>";
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		PairMapContext<vector<double>, int, map<int, vector<double> > >* ctx =
				dynamic_cast<PairMapContext<vector<double>, int,
						map<int, vector<double> > >*>(obj);
		assert(ctx != NULL);

		return convertToJObject<map<int, map<int,vector<double> > > >(env, ctx->call(convertFromJObject<vector<double> >(env, jobj)));
	}

	pattern = "PairMapContext<map<int,map<int,vector<double>>>,int,vector<double>>";
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		PairMapContext<map<int, map<int, vector<double> > >, int, vector<double> >* ctx =
				dynamic_cast<PairMapContext<map<int, map<int, vector<double> > >, int, vector<double> >*>(obj);
		assert(ctx != NULL);

		return convertToJObject <map<int, vector<double> > > (env, ctx->call(convertFromJObject < map<int, map<int, vector<double> > > > (env, jobj)));
	}

	pattern = "MapContext<string,vector<double>>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		MapContext<string, vector<double> >* ctx = dynamic_cast<MapContext<
				string, vector<double> >*>(obj);
		assert(ctx);
		return convertToJObject < vector<double>
				> (env, ctx->call(convertFromJObject<string>(env, jobj)));
	}

	cerr << "in mapping 1 jobject, fatal error ==> return NULL, superclass is " << super <<  endl;
	return NULL;
}

jobject mapping(JNIEnv* env, Object* obj,string super, jobject obj1, jobject obj2) {

	string pattern("ReduceContext<int,int,int>");
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		ReduceContext<int, int, int>* ctx = dynamic_cast<ReduceContext<int, int, int>* >(obj);
		assert(ctx != NULL);

		return convertToJObject<int>(env, ctx->call(convertFromJObject<int>(env, obj1),convertFromJObject<int>(env, obj2)));
	}
	
	pattern = "ReduceContext<map<int,vector<double>>,map<int,vector<double>>,map<int,vector<double>>>";
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		ReduceContext<map<int, vector<double> >, map<int, vector<double> >,
				map<int, vector<double> > >* ctx = dynamic_cast<ReduceContext<
				map<int, vector<double> >, map<int, vector<double> >,
				map<int, vector<double> > >*>(obj);
		assert(ctx != NULL);

		return convertToJObject<map<int,vector<double> > >(env, ctx->call(convertFromJObject<map<int,vector<double> > >(env, obj1),
				convertFromJObject<map<int,vector<double> > >(env, obj2)));
	}

	cerr << "in mapping 2 jobjects, fatal error ==> return NULL, superclass is " << super <<  endl;

	return NULL;
}

jint mappingPartitions(JNIEnv* env, Object* obj,string super, jobject jobj) {
	string pattern("Partitioner<string>");
	if (kmp_search(pattern.c_str(), pattern.size(),super.c_str(), super.size(), 0) >= 0) {
		Partitioner<string>* ptr = dynamic_cast<Partitioner<string>* >(obj);
		assert(ptr != NULL);

		return ptr->getPartitions(convertFromJObject<string>(env, jobj));
	}

	pattern = "Partitioner<double>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<double>* ptr = dynamic_cast<Partitioner<double>*>(obj);
		assert(ptr != NULL);

		return ptr->getPartitions(convertFromJObject<double>(env, jobj));
	}

	pattern = "Partitioner<vector<string>>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<vector<string> >* ptr = dynamic_cast<Partitioner<vector<string> >*>(obj);
		assert(ptr != NULL);

		return ptr->getPartitions(convertFromJObject< vector<string> >(env, jobj));
	}

	pattern = "Partitioner<vector<double>>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<vector<double> >* ptr = dynamic_cast<Partitioner<vector<double> >*>(obj);
		assert(ptr != NULL);

		return ptr->getPartitions(convertFromJObject<vector<double> >(env, jobj));
	}
}

jboolean mappingEquals(JNIEnv* env, Object* obj ,string super, jobject jobj) {
	string pattern("Partitioner<string>");
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<string>* ptr = dynamic_cast<Partitioner<string>*>(obj);
		assert(ptr != NULL);

		return ptr->equals(convertFromJObject<string>(env, jobj));
	}

	pattern = "Partitioner<double>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<double>* ptr = dynamic_cast<Partitioner<double>*>(obj);
		assert(ptr != NULL);

		return ptr->equals(convertFromJObject<double>(env, jobj));
	}

	pattern = "Partitioner<vector<string>>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<vector<string> >* ptr = dynamic_cast<Partitioner<
				vector<string> >*>(obj);
		assert(ptr != NULL);

		return ptr->equals(
				convertFromJObject<vector<string> >(env, jobj));
	}

	pattern = "Partitioner<vector<double>>";
	if (kmp_search(pattern.c_str(), pattern.size(), super.c_str(), super.size(),
			0) >= 0) {
		Partitioner<vector<double> >* ptr = dynamic_cast<Partitioner<
				vector<double> >* >(obj);
		assert(ptr != NULL);

		return ptr->equals(convertFromJObject<vector<double> >(env, jobj));
	}
}
