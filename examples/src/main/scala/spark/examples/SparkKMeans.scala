package spark.examples

import java.util.Random
import spark.SparkContext
import spark.util.Vector
import spark.SparkContext._
import scala.collection.mutable.HashMap
import scala.collection.mutable.HashSet

object SparkKMeans {
  val R = 1000     // Scaling factor
  val rand = new Random(42)
    
  def parseVector(line: String): Vector = {
      var vec =  new Vector(line.split(' ').map(_.toDouble))
	  //println(vec.toString)
	  return vec
  }
  
  def closestPoint(p: Vector, centers: HashMap[Int, Vector]): Int = {
	println("the p is: \t" + p.toString)  
    var index = 0
    var bestIndex = 0
    var closest = Double.PositiveInfinity
  
    for (i <- 1 to centers.size) {
      val vCurr = centers.get(i).get
      val tempDist = p.squaredDist(vCurr)
      if (tempDist < closest) {
        closest = tempDist
        bestIndex = i
      }
    }
  
    return bestIndex
  }

  def main(args: Array[String]) {
    if (args.length < 4) {
        System.err.println("Usage: SparkLocalKMeans <master> <file> <k> <convergeDist>")
        System.exit(1)
    }
    val sc = new SparkContext(args(0), "SparkLocalKMeans")
    val lines = sc.textFile(args(1))

    val data = lines.map(parseVector _).cache()
    val K = args(2).toInt
    val convergeDist = args(3).toDouble
  
    var points = data.takeSample(false, K, 42)
    var kPoints = new HashMap[Int, Vector]
    var tempDist = 1.0
    
	println("takeSample ponits ...")
    for (i <- 1 to points.size) {
      kPoints.put(i, points(i-1))
	  println(i + "\t" + points(i-1))
    }

    while(tempDist > convergeDist) {
	  println("tmpDist is :\t" + tempDist)
      var closest = data.map (p => (closestPoint(p, kPoints), (p, 1)))
	  println("after first Map ...")
      closest.collect().foreach(println)

      var pointStats = closest.reduceByKey {case ((x1, y1), (x2, y2)) => (x1 + x2, y1 + y2)}
	  println("after reduceByKey ...")
      pointStats.collect().foreach(println)

      var newPoints = pointStats.map {pair => (pair._1, pair._2._1 / pair._2._2)}.collect()
	  println("after second Map ...")
      newPoints.foreach(println)

      tempDist = 0.0
      for (pair <- newPoints) {
        tempDist += kPoints.get(pair._1).get.squaredDist(pair._2)
      }
      
      for (newP <- newPoints) {
        kPoints.put(newP._1, newP._2)
      }
    }

    println("Final centers: " + kPoints)
    System.exit(0)
  }
}
