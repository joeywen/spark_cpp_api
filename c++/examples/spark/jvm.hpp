#ifndef __JVM_HPP__
#define __JVM_HPP__

#include <jni.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

namespace spark {

class Jvm {
public:

	enum JNIVersion {
		v_1_1 = JNI_VERSION_1_1,
		v_1_2 = JNI_VERSION_1_2,
		v_1_4 = JNI_VERSION_1_4,
		v_1_6 = JNI_VERSION_1_6
	};

	Jvm(const std::vector<std::string>& options, JNIVersion jniversion =
			Jvm::v_1_6);
	~Jvm();

	//forward declarations
	class ConstructorFinder;
	class MethodFinder;
	class JConstructor;
	class MethodSignature;
	class JMethod;

	class JClass {
	public:
		static const JClass forName(const std::string& nativeName);

		JClass(const JClass& other);
		//return the class of an array of the current class
		const JClass arrayOf() const;

		// Creates a builder that can be used to locate a constructor of this
		// class with Jvm::findConstructor.
		ConstructorFinder constructor() const;
		// creste a builder that can be used to locate a constructor of this class with jvm::findMethod
		MethodFinder method(const std::string& name) const;
	private:
		friend class Jvm;
		JClass(const std::string& nativeName, bool isNative = true);

		std::string signature() const;

		std::string nativeName;
		bool isNative;
	};

	/**
	 * a builder that is used to specify a constructor by specifying its parameter
	 * list with zero or more calss to ConstructorFinder::parameter.
	 */
	class ConstructorFinder {
	public:
		// add a parameter to the constructor parameter list
		ConstructorFinder& parameter(const JClass& type);

	private:
		friend class Jvm;
		friend class JClass;

		ConstructorFinder(const JClass& type);

		const JClass type;
		std::vector<JClass> parameters;
	};

	/**
	 * an opaque construtor descriptor that can be used to create new instances
	 * of a class using Jvm::invokeConstructor
	 */
	class JConstructor {
	public:
		JConstructor(const JConstructor& other);
	private:
		friend class Jvm;
		JConstructor(const JClass& clazz, const jmethodID id);

		const JClass clazz;
		const jmethodID id;
	};

	/**
	 * a builder thst is used to specify an instance method by specifying its
	 * parameter list with zero or more calls to MethodFinder::parameter and a
	 * final call to MethodFinder::returns to get an opaque specification of the
	 * method for use with Jvm::findMethod
	 */
	class MethodFinder {
	public:
		// add a parameter to the method parameter list
		MethodFinder& parameter(const JClass& type);

		//terminates description of a method by specifying its return type
		MethodSignature returns(const JClass& type) const;
	private:
		friend class JClass;
		MethodFinder(const JClass& clazz, const std::string& name);

		const JClass clazz;
		const std::string name;
		std::vector<JClass> parameters;
	};

	class MethodSignature {
	public:
		MethodSignature(const MethodSignature& other);
	private:
		friend class Jvm;
		friend class MethodFinder;

		MethodSignature(const JClass& clazz, const std::string& name,
				const JClass& returnType,
				const std::vector<JClass>& parameters);

		const JClass clazz;
		const std::string name;
		const JClass returnType;
		std::vector<JClass> parameters;
	};

	class JMethod {
	public:
		JMethod(const JMethod& other);
	private:
		friend class Jvm;
		friend class MethodSignature;

		JMethod(const JClass& clazz, const jmethodID id);

		const JClass clazz;
		const jmethodID id;
	};

	class JField {
	public:
		JField(const JField& other);
	private:
		friend class Jvm;
		//friend class 
		JField(const JClass& clazz, const jfieldID id);

		const JClass clazz;
		const jfieldID id;
	};

	class Attach {
	public:
		Attach(Jvm* jvm, bool daemon = true);
		~Attach();
	private:
		Jvm* _jvm;
	};

	friend class Attach;

	const JClass voidClass;
	const JClass booleanClass;
	const JClass byteClass;
	const JClass charClass;
	const JClass shortClass;
	const JClass intClass;
	const JClass longClass;
	const JClass floatClass;
	const JClass doubleClass;
	const JClass stringClass;

	jobject string(const std::string& str);

	JConstructor findConstructor(const ConstructorFinder& constructor);
	JMethod findMethod(const MethodSignature& signature);
	JMethod findStaticMethod(const MethodSignature& signature);
	JField findStaticField(const JClass& clazz, const std::string& name);

	jobject invoke(const JConstructor& ctor, ...);

	template<typename T>
	T invoke(const jobject receiver, const JMethod& method, ...);

	template<typename T>
	T invokeStatic(const JMethod& method, ...);

	template<typename T>
	T getStaticField(const JField& field);

	jobject newGlobalRef(const jobject object);
	void deleteGlobalRef(const jobject object);
	void deleteGlobalRefSafe(const jobject object);

private:
	jclass findClass(const JClass& clazz);

	jmethodID findMethod(const Jvm::JClass& clazz, const std::string& name,
			const Jvm::JClass& returnType,
			const std::vector<Jvm::JClass> argTypes, bool isStatic);

	template<typename T>
	T invokeV(const jobject receiver, const jmethodID id, va_list args);

	template<typename T>
	T invokeStaticV(const JClass& receiver, const jmethodID id, va_list args);

	void attachDaemon();
	void attach();
	void detach();

	JavaVM* jvm;
	JNIEnv* env;
};

template<>
void Jvm::invoke<void>(const jobject receiver, const JMethod& method, ...);

template<typename T>
T Jvm::invoke(const jobject receiver, const JMethod& method, ...) {
	va_list args;
	va_start(args, method);
	const T result = invokeV<T>(receiver, method.id, args);
	va_end(args);

	return result;
}

template<>
void Jvm::invokeStatic<void>(const JMethod& method, ...);

template<typename T>
T Jvm::invokeStatic(const JMethod& method, ...) {
	va_list args;
	va_start(args, method);
	const T result = invokeStaticV<T>(method.clazz, method.id, args);
	va_end(args);

	return result;
}

} // namespace spark

#endif // __JVM_HPP__
