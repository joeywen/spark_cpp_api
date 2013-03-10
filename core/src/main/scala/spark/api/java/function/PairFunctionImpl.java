
package spark.api.java.function;

import java.util.Iterator;

import scala.Tuple2;
import java.util.HashMap;

public class PairFunctionImpl extends  PairFunction<String, String, Integer>{
	
	@Override
	public Tuple2<String, Integer> call(String t) throws Exception {
		/*HashMap<Object, Object> map = callNativeMethod(t);
		Iterator iter = map.keySet().iterator();
		while (iter.hasNext()) {
			Object key = iter.next();
			Object value = map.get(key);
			return new Tuple2(key, value);
		}*/
		return new Tuple2(t, 1);
	}
	
	/*
	public native HashMap<Object, Object> callNativeMethod(T t);
	
	static{
		System.loadLibrary("PairFunction");
	}
	*/
	
}
