
package spark.api.java.function;

import java.lang.Iterable;
import java.util.Arrays;
import java.util.ArrayList;

public class DoubleFlatMapFunctionImpl extends DoubleFlatMapFunction<String>{
	
	@Override
	public Iterable<Double> call(String x){
		ArrayList<String> strArr = (ArrayList) Arrays.asList(x.split(" ")); 
		ArrayList<Double> dArr = new ArrayList<Double>();
		int len = strArr.size();
		for (int i = 0; i < len; ++i) {
			dArr.add(Double.parseDouble(strArr.get(i)));
		}
		return dArr;
	}
	
}
