package spark.rdd

import java.io.PrintWriter
import java.util.StringTokenizer

import scala.collection.Map
import scala.collection.JavaConversions._
import scala.collection.mutable.ArrayBuffer
import scala.io.Source

import spark.OneToOneDependency
import spark.RDD
import spark.SparkEnv
import spark.Split


/**
 * An RDD that pipes the contents of each parent partition through an external command
 * (printing them one per line) and returns the output as a collection of strings.
 */
class PipedRDD[T: ClassManifest](
  parent: RDD[T], command: Seq[String], envVars: Map[String, String])
  extends RDD[String](parent.context) {

  def this(parent: RDD[T], command: Seq[String]) = this(parent, command, Map())

  // Similar to Runtime.exec(), if we are given a single string, split it into words
  // using a standard StringTokenizer (i.e. by spaces)
  def this(parent: RDD[T], command: String) = this(parent, PipedRDD.tokenize(command))

  override def splits = parent.splits

  override val dependencies = List(new OneToOneDependency(parent))

  override def compute(split: Split): Iterator[String] = {
    val pb = new ProcessBuilder(command)
    // Add the environmental variables to the process.
    val currentEnvVars = pb.environment()
    envVars.foreach { case (variable, value) => currentEnvVars.put(variable, value) }
    
    val proc = pb.start()
    val env = SparkEnv.get

    // Start a thread to print the process's stderr to ours
    new Thread("stderr reader for " + command) {
      override def run() {
        for (line <- Source.fromInputStream(proc.getErrorStream).getLines) {
          System.err.println(line)
        }
      }
    }.start()

    // Start a thread to feed the process input from our parent's iterator
    new Thread("stdin writer for " + command) {
      override def run() {
        SparkEnv.set(env)
        val out = new PrintWriter(proc.getOutputStream)
        for (elem <- parent.iterator(split)) {
          out.println(elem)
        }
        out.close()
      }
    }.start()

    // Return an iterator that read lines from the process's stdout
    val lines = Source.fromInputStream(proc.getInputStream).getLines
    return new Iterator[String] {
      def next() = lines.next()
      def hasNext = {
        if (lines.hasNext) {
          true
        } else {
          val exitStatus = proc.waitFor()
          if (exitStatus != 0) {
            throw new Exception("Subprocess exited with status " + exitStatus)
          }
          false
        }
      }
    }
  }
}

object PipedRDD {
  // Split a string into words using a standard StringTokenizer
  def tokenize(command: String): Seq[String] = {
    val buf = new ArrayBuffer[String]
    val tok = new StringTokenizer(command)
    while(tok.hasMoreElements) {
      buf += tok.nextToken()
    }
    buf
  }
}
