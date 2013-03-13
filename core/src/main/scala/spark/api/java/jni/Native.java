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
	public native Iterable<HashMap<Object, Object>> callPairFlatMap(Object t, String fname);

	///////////////////////////////////////////////////////////////////////////////////////////

	public native int getPartition(Object t, String fname);
	public native boolean equals(Object t, String fname);
	
}
