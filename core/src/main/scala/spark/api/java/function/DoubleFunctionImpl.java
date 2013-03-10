package spark.api.java.function;

public class DoubleFunctionImpl extends DoubleFunction<String> {

	public Double call(String x) throws Exception {
		return Double.parseDouble(x);
	}
}
