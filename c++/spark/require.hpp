#ifndef REQUIRE_H_
#define REQUIRE_H_

#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>
#include <jni.h>

inline void require(bool requirement, const std::string& msg = "Requirement failed") {
	using namespace std;
	if (!requirement) {
		fputs(msg.c_str(), stderr);
		fputs("\n", stderr);
		exit(1);
	}
}

inline void requireArgs(int argc, int args, const std::string& msg = "Must use %d arguments") {
	using namespace std;

	if (argc != args + 1) {
		fprintf(stderr, msg.c_str(), args);
		fputs("\n", stderr);
		exit(1);
	}
}

inline void requireMinArgs(int argc, int minArgs, const std::string& msg = "Must use %d arguments") {
	using namespace std;

	if (argc < minArgs + 1) {
		fprintf(stderr, msg.c_str(), minArgs);
		fputs("\n", stderr);
		exit(1);
	}
}

inline void assure(std::ifstream& in, const std::string& filename = "") {
	using namespace std;

	if (!in) {
		fprintf(stderr, "Could not open file %s\n", filename.c_str());
		exit(1);
	}
}

inline void assure(std::ofstream& out, const std::string& filename = "") {
	using namespace std;

	if (!out) {
		fprintf(stderr, "Could not open file %s\n", filename.c_str());
		exit(1);
	}
}

inline void assure(bool requirement, JNIEnv *env) {
	if (!requirement) {
		if (env->ExceptionOccurred()) {
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
	assert(requirement);
} 

inline void assure(bool requirement, JNIEnv *env, const std::string& msg) {
	if (!requirement) {
		// fprintf(stderr, msg.c_str(), 0);
		std::cout << msg.c_str() << "\n";
		if (env->ExceptionOccurred()) {
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
	assert(requirement);
}
#endif  // REQUIRE_H 
