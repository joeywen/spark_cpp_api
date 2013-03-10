#ifndef __CSPARKCONTEXT_HPP__
#define __CSPARKCONTEXT_HPP__

#include <jni.h>
#include <iostream>
#include <string>

#include "spark/crdd.hpp"
#include "spark/cpairrdd.hpp"

class CSparkContext{
public:
	CSparkContext(const std::string& _master, const std::string& _jobName);
	~CSparkContext();
	
	CRDD& textFile(const std::string& path);

private:
	JNIEnv* env;
	
	jobject sc;

	std::string master;
	std::string jobName;
};

JNIEnv* createJvm(const string classpath, const string jarpath);


#endif // __CSPARKCONTEXT_HPP__
