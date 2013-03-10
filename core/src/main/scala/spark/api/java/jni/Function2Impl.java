package spark.api.java.jni;

import spark.api.java.*;
import spark.api.java.function.*;

import java.util.HashMap;
import java.util.Vector;
import java.util.Iterator;

public class Function2Impl extends Function2<Object, Object, Object>
		implements FunctionInterface {
	private String fname = null;
	
	public Function2Impl(String s) {
		this.fname = s;
	}
	
	@Override
	public Object call(Object t1, Object t2) throws Exception {
		return nativeInstance.function2call(t1, t2, fname);
	}
}

//////////////////////////////////////////////////////////////////////////////////
/*class IIIFunction2 extends Function2<Integer, Integer, Integer> implements
		FunctionInterface {

	private String functionName = null;
	public IIIFunction2(String nn) {
		this.functionName = nn;
	}
	
	@Override
	public Integer call(Integer t1, Integer t2) throws Exception {
		return nativeInstance.iiiCall(t1, t2, functionName);
	}
}

class KMeansFunction2
		extends
		Function2<HashMap<Integer, Iterable<Double>>, HashMap<Integer, Iterable<Double>>, HashMap<Integer, Iterable<Double>>>
		implements FunctionInterface {
	private String functionName = null;
	
	public KMeansFunction2(String nn) {
		this.functionName = nn;
	}
	
	@Override
	public HashMap<Integer, Iterable<Double>> call(
			HashMap<Integer, Iterable<Double>> t1,
			HashMap<Integer, Iterable<Double>> t2) throws Exception {
		return nativeInstance.kmeansReduceCall(t1, t2, this.functionName);
	}
}*/