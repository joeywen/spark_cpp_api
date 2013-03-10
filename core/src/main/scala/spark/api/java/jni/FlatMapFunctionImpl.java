package spark.api.java.jni;

import spark.api.java.*;
import spark.api.java.function.*;
import java.util.Arrays;
import java.util.HashMap;
import java.lang.Iterable;


public class FlatMapFunctionImpl extends FlatMapFunction<String, Object> implements FunctionInterface {
	
	private String fname = null;
	
	public FlatMapFunctionImpl(String s) {
		this.fname = s;
	}
	
	public Iterable<Object> call(String s) throws Exception {
		return nativeInstance.callFlatMap(s, this.fname);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/*class DoubleFlatMapFunction extends FlatMapFunction<String, Double> implements FunctionInterface {
	
	private String functionName = null;
	public DoubleFlatMapFunction(String n) {
		this.functionName = n;
	}
	
	@Override
	public Iterable<Double> call(String s) throws Exception {
		return nativeInstance.doubleCall(s, this.functionName);
	}
	
}

class FloatFlatMapFunction extends FlatMapFunction<String, Float> implements FunctionInterface {

	private String functionName = null;
	public FloatFlatMapFunction(String n) {
		this.functionName = n;
	}
	
	@Override
	public Iterable<Float> call(String s) throws Exception {
			
		return nativeInstance.floatCall(s, this.functionName);
	}

}

class IntegerFlatMapFunction extends FlatMapFunction<String, Integer> implements FunctionInterface {

	private String functionName = null;
	public IntegerFlatMapFunction(String m) {
		this.functionName = m;
	}
	
	@Override
	public Iterable<Integer> call(String s) throws Exception {
			
		return nativeInstance.integerCall(s, this.functionName);
	}

}

class StringFlatMapFunction extends FlatMapFunction<String, String> implements FunctionInterface {
	
	private String functionName = null;
	public StringFlatMapFunction(String n) {
		this.functionName = n;
	}
	
	@Override
	public Iterable<String> call(String s) throws Exception {
		return nativeInstance.stringCall(s, this.functionName);
	}
	
}

class LongFlatMapFunction extends FlatMapFunction<String, Long> implements FunctionInterface {

	private String functionName = null;
	public LongFlatMapFunction(String n) {
		this.functionName = n;
	}
	
	@Override
	public Iterable<Long> call(String s) throws Exception {
			
		return nativeInstance.longCall(s, this.functionName);
	}

}

class TestFlatMapFunction extends FlatMapFunction<String, Object> implements FunctionInterface {
	
	private String functionName = null;
	public TestFlatMapFunction(String n) {
		this.functionName = n;
	}
	
	public Iterable<Object> call(String s) throws Exception {
		
		return nativeInstance.call(s, this.functionName);
	}
}*/
