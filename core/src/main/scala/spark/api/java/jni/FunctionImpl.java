package spark.api.java.jni;

import spark.api.java.*;
import spark.api.java.function.*;

import java.util.HashMap;
import java.util.Vector;
import java.util.Iterator;

public class FunctionImpl extends Function<Object, Object> implements
		FunctionInterface {
	private String fname = null;

	public FunctionImpl(String s) {
		this.fname = s;
	}

	@Override
	public Object call(Object t) throws Exception {
		return nativeInstance.functionCall(t, fname);
	}
}