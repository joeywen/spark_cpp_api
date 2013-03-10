#ifndef _JNI_EXCEPTION_H_
#define _JNI_EXCEPTION_H_

#include <jni.h>
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

void jniException(JNIEnv* env, jthrowable exc){
	assert(env != NULL);
	 if (exc) {
	 	env->ExceptionDescribe();
		env->ExceptionClear();
	 } else {
	 	cout << "not a jni exception !" << endl;
	 }
}

#endif // end of declare jni_exception header file
