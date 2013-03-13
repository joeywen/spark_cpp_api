#include <iostream>
#include "spark/storageLevel.h"

using namespace std;

char* getStorageLevel(StorageLevel level) {
	switch(level) {
	case NONE:
		return "NONE";
	case DISK_ONLY:
		return "DISK_ONLY";
	case DISK_ONLY_2:
		return "DISK_ONLY_2";
	case MEMORY_ONLY:
		return "MEMORY_ONLY";
	case MEMORY_ONLY_2:
		return "MEMORY_ONLY_2";
	case MEMORY_ONLY_SER:
		return "MEMORY_ONLY_SER";
	case MEMORY_ONLY_SER_2:
		return "MEMORY_ONLY_SER_2";
	case MEMORY_AND_DISK:
		return "MEMORY_AND_DISK";
	case MEMORY_AND_DISK_2:
		return "MEMORY_AND_DISK_2";
	case MEMORY_AND_DISK_SER:
		return "MEMORY_AND_DISK_SER";
	case MEMORY_AND_DISK_SER_2:
		return "MEMORY_AND_DISK_SER_2";
	default:
		return "MEMORY_ONLY";
	}
}