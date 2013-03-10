
package spark.api.java.function;

import java.lang.Iterable;
import java.util.Arrays;

public class FlatMapFunctionImpl extends FlatMapFunction<String, String>{
	
	@Override
	public Iterable<String> call(String x) throws Exception{
		return Arrays.asList(x.split(" "));
	}
	
	/*public native Iterable<R> callNativeMethod(T x);
	
	static {
		System.loadLibrary("FlatMapFunction");
	}*/
}
