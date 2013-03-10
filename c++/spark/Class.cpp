#include "Class.h"
#include <iostream>

using namespace std;

map<string, Class*> Class::classMap = map<string, Class*>();

string Class::getName() const {
	return name;
}

string Class::getSuperClassName() const {
	return superName;
}

Class *Class::forName(string className) {
	if (classMap.count(className) != 0) {
		return classMap[className];
	}

	return NULL;
}

Object *Class::newInstance() const{
	return factory();
}

Class::~Class(){}

const Class *const Object::theClass = Class::Register<Object>("Object", "Object");
