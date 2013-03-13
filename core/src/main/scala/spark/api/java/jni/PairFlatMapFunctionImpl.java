package spark.api.java.jni;

import java.util.HashMap;
import java.util.Vector;
import java.util.Map;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.lang.Object;

import scala.Tuple2;

import spark.api.java.*;
import spark.api.java.function.*;

public class PairFlatMapFunctionImpl extends PairFlatMapFunction<Object, Object, Object> implements FunctionInterface {

	private String fname = null;

	public PairFlatMapFunctionImpl(String s) {
		this.fname = s;
	}

	public Iterable<Tuple2<Object, Object>> call(Object t) throws Exception {
		
		return null;
	}

}
