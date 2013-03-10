package spark.api.java.jni;

import java.util.HashMap;
import java.util.Vector;
import java.util.Iterator;
import java.util.Map;

import spark.api.java.*;
import spark.api.java.function.*;

import scala.Tuple2;

public class PairFunctionImpl extends PairFunction<Object, Object, Object>
		implements FunctionInterface {
	
	private String fname = null;
	public PairFunctionImpl(String s) {
		this.fname = s;
	}
	
	@Override
	public Tuple2<Object, Object> call(Object s) throws Exception {
		HashMap<Object, Object> hashmap = nativeInstance.pairCall(s, this.fname);

		Iterator iter = hashmap.entrySet().iterator();
		Object key = null;
		Object val = null;
		while (iter.hasNext()) {
			Map.Entry entry = (Map.Entry) iter.next();
			key = entry.getKey();
			val = entry.getValue();
		}

		if (key == null && val == null) {
			System.out.println("Error ! ");
			return null;
		}
		return new Tuple2(key, val);
	}
}
////////////////////////////////////////////////////////////////////////////////////
/*
class PairSISFunction extends PairFunction<String, String, Integer> implements
		FunctionInterface {
	
	private String functionName = null;
	
	public PairSISFunction(String nn) {
		this.functionName = nn;
	}

	@Override
	public Tuple2<String, Integer> call(String s) throws Exception {
		HashMap<String, Integer> hashmap = nativeInstance.pairSISCall(s, this.functionName);

		Iterator<String> iter = hashmap.keySet().iterator();
		String ss = null;
		Integer ii = 0;
		while (iter.hasNext()) {
			ss = iter.next();
			ii = hashmap.get(ss);
		}

		if (ss == null && ii == 0) {
			return null;
		}
		return new Tuple2(ss, ii);
	}
}

class KMeansPairFunction
		extends
		PairFunction<Iterable<Double>, Integer, HashMap<Integer, Iterable<Double>>>
		implements FunctionInterface {

	private String functionName = null;

	public KMeansPairFunction(String func) {
		functionName = func;
	}

	public Tuple2<Integer, HashMap<Integer, Iterable<Double>>> call(Iterable<Double> t) throws Exception {

		HashMap<Integer, HashMap<Integer, Iterable<Double>>> hashmap = nativeInstance
				.kmeansPairCall(t, this.functionName);
		
		Iterator<Integer> iter = hashmap.keySet().iterator();

		Integer ii = 0;
		HashMap<Integer, Iterable<Double>> vtr = new HashMap<Integer, Iterable<Double>>();
		while (iter.hasNext()) {
			ii = iter.next();
			vtr = hashmap.get(ii);
		}

		if (vtr == null && ii == 0) {
			return null;
		}
		return new Tuple2(ii, vtr);
	}
}

class KMeansAggrFunction
		extends
		PairFunction<Tuple2<Integer, HashMap<Integer, Iterable<Double>>>, Integer, Iterable<Double>>
		implements FunctionInterface {
	private String functionName = null;
	
	public KMeansAggrFunction(String nn) {
		functionName = nn;
	}
	
	public Tuple2<Integer, Iterable<Double>> call(Tuple2<Integer, HashMap<Integer, Iterable<Double>>> tuple) {
		
		HashMap<Integer, HashMap<Integer, Iterable<Double>>> mm = new HashMap<Integer, HashMap<Integer, Iterable<Double>>>();
		mm.put(tuple._1, tuple._2);
		
		HashMap<Integer, Iterable<Double>> hashmap = nativeInstance
				.kmeansAggCall(mm, this.functionName);

		Iterator<Integer> iter = hashmap.keySet().iterator();
		Integer ii = 0;
		//Iterable<Double> vtr = new Iterable<Double>();
		Tuple2 tuple2 = null;
		
		while (iter.hasNext()) {
			ii = iter.next();
			
			tuple2 = new Tuple2(ii, hashmap.get(ii));
		}

		if (tuple2 == null) {
			return null;
		}
		return tuple2;//new Tuple2(ii, vtr);
	}
}*/
