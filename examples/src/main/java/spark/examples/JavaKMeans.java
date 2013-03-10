package spark.examples;

import spark.api.java.*;
import spark.api.java.function.*;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Random;
import java.util.Arrays;
import java.io.Serializable;
import spark.util.Vector;

import scala.Tuple2;


public class JavaKMeans {
	private static HashMap<Integer, Iterable<Double>> kPoint = new HashMap<Integer, Iterable<Double>>();

	static class Closest implements Serializable {
		int i;
		Iterable<Double> iter;
		public Closest(Iterable<Double> dd, int ii) {
			this.i = ii;
			this.iter = dd;
		}
		
		public Closest add(Closest clt) {
			int ii = this.i + clt.i;
			Iterator<Double> titer = this.iter.iterator();
			Iterator<Double> oiter = clt.iter.iterator();
			Iterator<Double> tmp = this.iter.iterator();
			int count = 0;
			while (tmp.hasNext()) {
				tmp.next();
				count ++;
			}
			Double[] dd = new Double[count];
			count = 0;
			while (titer.hasNext() && oiter.hasNext()) {
				Double me = titer.next();
				Double other = oiter.next();
				dd[count ++] = me + other;
			}

			Iterable<Double> newiter = Arrays.asList(dd);
			return new Closest(newiter, ii);
		}
		
		public Iterable<Double> division() {
			Iterator<Double> tmp = iter.iterator();
			Iterator<Double> iterator = iter.iterator();
			int count = 0;
			while(tmp.hasNext()) {
				tmp.next();
				count ++;
			}
			
			Double[] dd = new Double[count];
			count  = 0;
			while (iterator.hasNext()) {
				dd[count ++] = iterator.next() / this.i;
			}
			
			return Arrays.asList(dd);
		}
	}
	
	public static void main(String[] args) {
		if (args.length < 4) {
			System.err
					.println("Usage: SparkLocalKMeans <master> <file> <k> <convergeDist>");
			System.exit(1);
		}

		int k = Integer.parseInt(args[2]);
		double converge = Double.parseDouble(args[3]);

		double tmpDist = 1.0;

		JavaSparkContext sc = new JavaSparkContext(args[0], "SparkKmeans");
		JavaRDD<String> lines = sc.textFile(args[1]);
		
		class Split extends FlatMapFunction<String, Double> {
			public Iterable<Double> call(String t) throws Exception {
				Iterable<String> str = Arrays.asList(t.split(" "));
				Iterator<String> iter = str.iterator();
				int count = 0;
				while(iter.hasNext()) {
					iter.next();
					count ++;
				}
				Double[] dd = new Double[count];
				Iterator<String> iterator = str.iterator();
				count = 0;
				while (iterator.hasNext()) {
					dd[count ++] = Double.parseDouble(iterator.next());
				}
				return Arrays.asList(dd);
			}
		}
		
		
		JavaRDD<Iterable<Double>> digts = lines.map(new Split());
		
		/*List<Iterable<Double>> dd = digts.collect();
		for (Iterable<Double> iarr : dd) {
			Iterator<Double> iter = iarr.iterator();
			while(iter.hasNext()) {
				Double D = iter.next();
				System.out.print(D + "\t");
			}
			System.out.println();
		}*/
		
		digts.cache();
		
		List<Iterable<Double>> points = digts.takeSample(false, k, 42);
		
		int e = 1;
		for (Iterable<Double> iter : points) {
			System.out.print(e + "\t");
			kPoint.put(e++, iter);
			Iterator<Double> ator = iter.iterator();
			while (ator.hasNext()) {
				System.out.print(ator.next() + ", ");
			}
			System.out.println();
		}
		
		// above code is OK !!!!!!!!!!
		
		class tupleCls extends PairFunction<Iterable<Double>, Integer, Closest> {
			
			public Tuple2<Integer, Closest> call(Iterable<Double> t) throws Exception {
				Iterator<Double> tt = t.iterator();
				while (tt.hasNext()) {
					System.out.print(tt.next() + ", ");
				}
				System.out.println();
				int bestIndex = 0;
				double closest  = Double.MAX_VALUE;
				
				
				for (int i = 1; i <= kPoint.size(); ++i) {
					double tmpDist = 0.0;
					
					Iterator<Double> iterkp = kPoint.get(i).iterator();
					Iterator<Double> iterp = t.iterator();
					
					while(iterkp.hasNext() && iterp.hasNext()) {
						double x1 = iterkp.next();
						double x2 = iterp.next();
						tmpDist += (x1 - x2) * (x1 - x2);
					}
					
					if (tmpDist < closest) {
						System.out.println("dist is :" + tmpDist + " and index is " + i);
						closest = tmpDist;
						bestIndex = i;
					}
				}
				
				return new Tuple2<Integer, Closest>(bestIndex, new Closest(t, 1));
			}
			
			
		}
		 
		class reduceCls extends Function2<Closest, Closest, Closest> {
			
			public Closest call(Closest x1, Closest x2) throws Exception {
				return x1.add(x2);
				//return null;
			}
			
		}
		
		class newPoint extends PairFunction<Tuple2<Integer, Closest>, Integer, Iterable<Double>> {
			public Tuple2<Integer, Iterable<Double>> call(Tuple2<Integer, Closest> clt) throws Exception {
				Iterable<Double> iter = clt._2.division();
				return new Tuple2(clt._1, iter);
			}
		}
		
		int count = 0;
		while (tmpDist > converge) {
			JavaPairRDD<Integer, Closest> closest = digts.map(new tupleCls());
			System.out.println("=======================================> After first map ...");
			List<Tuple2<Integer, Closest>> dd = closest.collect();
			for (Tuple2 tuple : dd) {
				System.out.print(tuple._1 + "\t");
				Closest clt = (Closest) tuple._2;
				Iterator<Double> iter = clt.iter.iterator();
				while (iter.hasNext()) {
					System.out.print(iter.next() + ", ");
				}
				
				System.out.println(clt.i);
			}
			
			JavaPairRDD<Integer, Closest> pointStats = closest.reduceByKey(new reduceCls());
			System.out.println("=======================================> After reduceByKey ...");
			List<Tuple2<Integer, Closest>> ddd = pointStats.collect();
			for (Tuple2 tuple : ddd) {
				System.out.print(tuple._1 + "\t");
				Closest clt = (Closest) tuple._2;
				Iterator<Double> iter = clt.iter.iterator();
				while (iter.hasNext()) {
					System.out.print(iter.next() + ", ");
				}
				
				System.out.println(clt.i);
			}
			
			JavaPairRDD<Integer, Iterable<Double>> newPoints = pointStats.map(new newPoint());

			List<Tuple2<Integer, Iterable<Double>>> lnewPoints = newPoints.collect(); 
			System.out.println("=======================================> After second map ...");
			for (Tuple2 tuple : lnewPoints) {
				System.out.println(tuple._1 + "\t");
				Iterable<Double> iter = (Iterable<Double>)tuple._2;
				Iterator<Double> ator = iter.iterator();
				while (ator.hasNext()) {
					System.out.print(ator.next() + ",");
				}
				System.out.println();
			}
			
			tmpDist = 0.0;
			for (Tuple2 tuple : lnewPoints) {
				Iterator<Double> oldp = kPoint.get(tuple._1).iterator();
				Iterator<Double> newp = ((Iterable<Double>)tuple._2).iterator();
				while (oldp.hasNext() && newp.hasNext()) {
					double x1 = oldp.next();
					double x2 = newp.next();
					tmpDist += (x1 - x2) * (x1 - x2);
				}

				System.out.println("tmpDist is " + tmpDist);

				kPoint.put((Integer)tuple._1, (Iterable<Double>)tuple._2);
			}

			count ++;

		//	if (count > 2) break;
		}
	
		for (int i = 1; i <= kPoint.size(); ++i) {
			System.out.print(i + ": ");
			Iterator<Double> iter = kPoint.get(i).iterator();

			while (iter.hasNext()) {
				System.out.print(iter.next() + ",");
			}

			System.out.println();
		}
		System.exit(0);
	}
	
	
}
