#ifndef __CLASS_LOADER_H__
#define __CLASS_LOADER_H__

#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <dlfcn.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK() pthread_mutex_lock(&mutex)
#define UNLOCK() pthread_mutex_unlock(&mutex)

using namespace std;

/**
 * must be modified to Singleton Pattern in future
 */
class ClassLoader {
public:
	void* dl_open() {
		if (!initialized) {
			LOCK();
			if (!initialized) {
				_handler = dlopen("libClassLoader.so", RTLD_NOW);
				initialized = true;
			}
			UNLOCK();
		}
		if (_handler == NULL) {
			std::cerr << "Cannot load library :" << dlerror() << "\n";
			return NULL;
		}
		return _handler;
	}

	ClassLoader() {
		initialized = false;
	}
	~ClassLoader() {
		dlclose(_handler);
	}

	ClassLoader(const ClassLoader&);
	const ClassLoader& operator=(const ClassLoader&);

private:
	bool initialized;
	void* _handler;
};

#endif // __CLASS_LOADER_H__ 
