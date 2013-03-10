package spark.api.java.jni;

import java.lang.reflect.Constructor;
import java.security.NoSuchAlgorithmException;
import java.lang.reflect.InvocationTargetException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.ArrayList;
import java.math.BigInteger;
import java.security.MessageDigest;

import spark.api.java.*;
import spark.api.java.function.*;

public final class Mapping {

	/*private static HashMap<String, Class> factory = new HashMap<String, Class>();
	
	static {
		register("iterable<string>", StringFlatMapFunction.class, "string");
		register("integer", IIIFunction2.class, "integer", "integer");
		register("hashmap<string,integer>", PairSISFunction.class, "string");
		
		register("vector<double>", DoubleFlatMapFunction.class, "string");
		register("map<int,map<int,vector<double>>>", KMeansPairFunction.class, "vector<double>");
		register("map<int,vector<double>>", KMeansAggrFunction.class, "map<int,map<int,vector<double>>>");
		register("map<int,vector<double>>", KMeansFunction2.class, "map<int,vector<double>>","map<int,vector<double>>");
	}

	private static void register(String retValue, Class clazz,
			String... inValue) {
		String toEnc = array2String(inValue).toLowerCase() + retValue.toLowerCase();
		try {
			MessageDigest mdEnc = MessageDigest.getInstance("MD5");
			mdEnc.update(toEnc.getBytes(), 0, toEnc.length());
			String md5 = new BigInteger(1, mdEnc.digest()).toString(16);
			
			factory.put(md5, clazz);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.out.println("register class error," + e.getMessage());
		}
	}

	private static String array2String(String... a) {
		if (a == null)
			return "null";
		int iMax = a.length - 1;
		if (iMax == -1)
			return "";

		StringBuilder b = new StringBuilder();
		for (int i = 0;; i++) {
			b.append(String.valueOf(a[i]));
			if (i == iMax)
				return b.toString();
		}
	}

	public synchronized <T, R> T newInstance(String retValue, R params, String inValue) {
		
		String toEnc = inValue.toLowerCase() + retValue.toLowerCase();
		String md5 = null;
		try {
			MessageDigest mdEnc = MessageDigest.getInstance("MD5");
			mdEnc.update(toEnc.getBytes(), 0, toEnc.length());
			md5 = new BigInteger(1, mdEnc.digest()).toString(16);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.out.println("register class error," + e.getMessage());
		}
		if (!factory.containsKey(md5)) {
			
			System.out
					.println("Cannot find any function which input value(s) is "
							+ inValue + ", and return value(s) is " + retValue);
			return null;
		}
		Class clazz = factory.get(md5);
		try {
			if (params == null) {
				return (T) clazz.newInstance();
			} else if ((params instanceof String) && "".equalsIgnoreCase((String)params)){
				return (T) clazz.newInstance();				
			} else {
				Constructor cons = clazz.getConstructor(params.getClass());
				return (T) cons.newInstance(params);
			}
		} catch (InstantiationException e) {
			e.printStackTrace();
			return null;
		} catch (IllegalAccessException e) {
			e.printStackTrace();
			return null;
		} catch (SecurityException e) {
			e.printStackTrace();
			return null;
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
			return null;
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
			return null;
		} catch (InvocationTargetException e) {
			e.printStackTrace();
			return null;
		}
	}*/
}