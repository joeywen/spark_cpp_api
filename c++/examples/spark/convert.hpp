#ifndef __CONVERT_HPP__
#define __CONVERT_HPP__

#include <jni.h>
#include <iostream>

template<typename T>
T convertFromJObject(JNIEnv* env, jobject jobj);

template<typename T>
jobject convertToJObject(JNIEnv* env, const T& t);

std::string jstrTocstr(JNIEnv *env, jstring jstr);

#endif // __CONVERT_HPP__
