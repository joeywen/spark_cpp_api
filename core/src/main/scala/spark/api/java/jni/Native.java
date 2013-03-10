package spark.api.java.jni;

import java.lang.Iterable;
import java.util.Arrays;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Vector;
import scala.Tuple2;

public final class Native {

	static {
		System.load("/usr/local/lib/libsparkJNI.so");
	}

	///////////////////////////////////////////////////////////////////////////////////////////	
	public native Iterable<Object> callFlatMap(String s, String fname);
	public native Object function2call(Object t1, Object t2, String fname);
	public native Object functionCall(Object t1, String fname);
	public native HashMap<Object, Object> pairCall(Object s, String fname);
	///////////////////////////////////////////////////////////////////////////////////////////
	
	// primitive type for FlatMapFunction
	/*public static native Iterable<String> stringCall(String s, String name);

	public static native Iterable<Double> doubleCall(String s, String name);

	public static native Iterable<Integer> integerCall(String s, String name);

	public static native Iterable<Float> floatCall(String s, String name);

	public static native Iterable<Long> longCall(String s, String name);

	// for PairFlatMapFunction
	// public abstract Iterable<Tuple2<K, V>> call(T t) throws Exception;
	// public native Iterable<HashMap<String, String>>

	public static native HashMap<String, Integer> pairSISCall(String s, String name);

	public static native HashMap<String, String> pairSSSCall(String s, String name);

	public static native HashMap<Integer, HashMap<Integer, Iterable<Double>>> kmeansPairCall(
			Iterable<Double> t, String name);

	public static native HashMap<Integer, Iterable<Double>> kmeansReduceCall(
			HashMap<Integer, Iterable<Double>> m1,
			HashMap<Integer, Iterable<Double>> m2, String name);

	public static native HashMap<Integer, Iterable<Double>> kmeansAggCall(
			HashMap<Integer, HashMap<Integer, Iterable<Double>>> mm, String name);

	public static native Integer iiiCall(Integer t1, Integer t2, String name);
	/********** test method declaration ***************/
	//public static native Iterable<Object> call(String obj, String name);
	//public static native Object call(Object obj, String name);
	
	
}
