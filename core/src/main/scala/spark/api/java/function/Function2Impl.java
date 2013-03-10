package spark.api.java.function;

public class Function2Impl extends Function2<Integer, Integer, Integer> {
	
	@Override
	public Integer call(Integer t1, Integer t2) throws Exception{
		return t1 + t2;
	}
	
	/*public native R callNativeMethod(T1 t1, T2 t2);
	
	static {
		System.loadLibrary("Function2");
	}*/
}
