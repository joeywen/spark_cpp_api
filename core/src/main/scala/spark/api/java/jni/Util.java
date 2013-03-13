package spark.api.java.jni;

import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.lang.Iterable;
import scala.Tuple2;
import java.util.Arrays;
import java.lang.Object;


public final class Util {

	public static <E> List<E> iterable2List(Iterable<E> iterable) {
	
		if(iterable instanceof List) {
			return (List<E>) iterable;
		}
		ArrayList<E> list = new ArrayList<E>();
		if(iterable != null) {
			for(E e: iterable) {
				list.add(e);
			}
		}
		return list;
	}

	public static <E> Iterable<E> list2Iterable(ArrayList<E> list) {
		if (list instanceof Iterable) {
			return (Iterable<E>) list;
		}

		Object[] objs = new Object[list.size()];

		objs = list.toArray(objs);

		return (Iterable<E>)Arrays.asList(objs);
	}

	public static <K, V> Tuple2<K, V> HashMap2Tuple2(HashMap<K, V> hashmap) {
		Iterator iter = hashmap.entrySet().iterator();
		Object key = null;
		Object val = null;
		while (iter.hasNext()) {
			Map.Entry entry = (Map.Entry) iter.next();
			key = entry.getKey();
			val = entry.getValue();
		}

		if (key == null && val == null) {
			System.err.println("Error ! ");
			return null;
		}
		return new Tuple2(key, val);
	} 

}
