#ifndef __CLASS_H__
#define __CLASS_H__

#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typeinfo>
#include <search.h>
#include <pthread.h>
#include <errno.h>
#include <stdexcept>
#include <exception>

using namespace std;


class Class;
class Object {
public:
	static const Class *const theClass;
	Object(){}
	virtual ~Object(){}
};

template<class T>
class ObjectFactory{
public:
	static Object *create() {
		return new T();
	}
};

#define DYNAMIC_DELARE() \
public: \
	static const Class *const theClass;

#define DYNAMIC_IMPLEMENT(classname, superclass) \
		string name##classname = typeid(classname).name(); \
		const Class *const classname::theClass = Class::Register<classname>(name##classname, superclass);

class Class{
private:
	Object *(*factory)(void);
	string name;
	string superName;
	static map<string, Class*> classMap;
	Class(){}

	Class(const Class&);
	const Class& operator=(const Class&);

public:
	inline static string& replace_all(string& str, const string& old_val,
			const string& new_val) {
		while (true) {
			string::size_type pos(0);
			if ((pos = str.find(old_val)) != string::npos)
				str.replace(pos, old_val.length(), new_val);
			else
				break;
		}
		return str;
	}

	template<class T>
	static Class * Register(string className, string superName) {
		if (classMap.count(className) == 0) {
			Class *cls = new Class();
			cls->factory = &ObjectFactory<T>::create;
			cls->name = className;
			//supername = std::transform(supername.begin(), supername.end(), supername.begin(), ::tolower);

			cls->superName = Class::replace_all(Class::replace_all(superName, "std::", ""), " ", "");
			classMap[className] = cls;
		}

		return classMap[className];
	}

	static Class *forName(string className);
	Object *newInstance() const;
	string getName() const;
	string getSuperClassName() const;
	virtual ~Class();
};


template<class T>
class Partitioner : public Object {
public:
	Partitioner(int num) : numPartitions(num) {}
	virtual ~Partitioner(){}

	virtual int getPartitions(T t) = 0;
	virtual bool equals(T t) = 0;

public:
	int numPartitions;
};


template<class T, class R>
class MapContext : public Object {
	public:
		MapContext(){}
		virtual ~MapContext() {}
		virtual R call(const T& r) = 0;
};

template<class T, class R>
class FlatMapContext : public Object {
	public:
		FlatMapContext(){}
		virtual ~FlatMapContext() {}

		virtual std::vector<R> call(const T& t) = 0;
};

template<class T, class K, class V>
class PairFlatMapContext : public Object {
public:
	PairFlatMapContext(){}
	virtual ~PairFlatMapContext(){}

	virtual std::vector<map<K, V> > call(const T& t) = 0;
};

template<class T, class K, class V>
class PairMapContext : public Object {
	public:
		PairMapContext(){}
		virtual ~PairMapContext(){}

		virtual std::map<K, V> call(const T& t) = 0;
};

template<class T1, class T2, class R>
class ReduceContext : public Object {
	public:
		ReduceContext(){}
		virtual ~ReduceContext(){}

		virtual R call(const T1& t1, const T2& t2) = 0;
};

template<class T>
class Attachment {
	public:
		Attachment(){};
		virtual ~Attachment(){};
		inline void setAttachment(const T& t){
			this->_attach = t;
		}

	public:
		T _attach;
};

#endif // Class.h



