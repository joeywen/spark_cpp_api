#!/bin/bash

source ~/.bash_profile

FWDIR=~/workspace/spark-0.6.0

CLASSPATH="$FWDIR/target/scala-2.9.2/classes"
#CLASSPATH+=":/home/software/scala-2.9.2/lib"

if [ -z $JAVA_HOME ];then
	echo "JAVA_HOME is not set"
	exit 1
fi

exec javah -classpath $CLASSPATH "$@" 
