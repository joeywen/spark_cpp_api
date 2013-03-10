#ifndef __MAPPING_HPP__
#define __MAPPING_HPP__ 

#include <jni.h>
#include <iostream>
#include "Class.h"

using namespace std;

jobject mapping(JNIEnv* env, Object*,string super, jstring str);

jobject mapping(JNIEnv* env, Object*,string super, jobject obj);

jobject mapping(JNIEnv* env, Object*,string super, jobject obj1, jobject obj);

// KMP string search algorithm
void get_nextval(char const* ptrn, int plen, int* nextval);

int kmp_search(const char * src, int slen, const char * patn, int plen, int pos);

// String Matching on Ordered Alphabets Algorithm
void nextMaximalSuffix(char* x,int m,int *i, int* j,int *k, int *p);

int SMOA(char *x, int m, char* y, int n);

#endif 
