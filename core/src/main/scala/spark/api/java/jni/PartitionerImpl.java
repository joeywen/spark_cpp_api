package spark.api.java.jni;

import spark.Partitioner;


public class PartitionerImpl extends Partitioner implements FunctionInterface {

	private int numPartitions;
	private String fname = null;

	public PartitionerImpl(int partitions, String s) {
		this.numPartitions = partitions;
		this.fname = s;
	}

	public int numPartitions() {
		return this.numPartitions;
	}

	public int getPartition(Object key) {
		return nativeInstance.getPartition(key, this.fname);
	}

	@Override
	public boolean equals(Object t) {
		return nativeInstance.equals(t, this.fname);
	}
}
