#!/usr/bin/env python

import os
import sys
import SCons
import subprocess

env = Environment()

# exec env file to set some env variables
env_process = subprocess.Popen('. ~/.bash_profile', shell = True, executable = "/bin/bash")
env_process.wait()

if env_process.returncode != 0:
	print "execute .bash_profile error !\n"

# prefix 
prefix = "/usr/local"
#include dir
include_dir = prefix + '/include'
bin_dir = prefix + '/bin'
lib_dir = prefix + '/lib'
 
# get current dir
current = os.getcwd()

home = os.environ['HOME']

# check env
java_home = os.getenv('JAVA_HOME', None)
scala_home = os.getenv('SCALA_HOME', None)

if java_home == None:
	print 'JAVA_HOME is not set, please set JAVA_HOME and then build this project.\n'
	sys.exit()

if scala_home == None:
	print 'SCALA_HOME is not set, please set SCALA_HOME and then build this project.\n'
	sys.exit()
	
# starting to build spark project
spark_process = subprocess.Popen('sbt/sbt assembly', shell = True, executable = "/bin/bash")
spark_process.wait()

if spark_process.returncode != 0:
	print "Build spark error ! \n"
	sys.exit()

#decompress spark jar 
if os.path.exists(home + '/.classes'):
    command = 'cd ' + home + '/.classes && jar -xf ' + current + '/core/target/spark*.jar && cd ' + current
else:
    command = 'mkdir ' + home + '/.classes && cd ' + home + '/.classes && jar -xf ' + current + '/core/target/spark*.jar && cd ' + current
    
spark_process = subprocess.Popen(command , shell = True, executable = "/bin/bash")
spark_process.wait()

#set java.class.path
os.environ['SPARK_CLASS_PATH'] = home + '/.class'

env = env.Append(CPPPATH = [java_home + '/include', 
							java_home + '/include/linux',
							java_home + '/jre/lib/amd64/server',
							current + '/c++/'],
				LIBS = ['jvm', 'pthread'],
				LIBPATH = [java_home + '/jre/lib/amd64/server'])

print 'starting to compile c++ API code ....'

jni_sources = Glob('#c++/spark/jni/*.c*')

spark_jni_obj = Object(jni_sources, srcdir = '#c++/spark/jni')

spark_jni_so = SharedLibrary('#c++/spark/jni/spark_jni',spark_jni_obj)

api_sources = Glob('#c++/spark/*.c*')

spark_api_obj = Object(api_sources, srcdir = '#c++/spark')

spark_api_so = SharedLibrary('#c++/spark/spark_api',spark_api_obj)

header_files = Glob('#c++/spark/*.h*')

#Alias('install-lib', Install(os.path.join(prefix, "/lib"), spark_jni_so, spark_api_so))
#Alias('install-include', Install(os.path.join(prefix, "/include"), header_files))
#Alias('install', ['install-include', 'install-lib'])

command = 'cp c++/spark/*.h* /usr/local/include/spark'
subprocess.Popen(command, shell = True, executable = "/bin/bash")
command = 'cp c++/spark/jni/*.h* /usr/local/include/spark/jni'
subprocess.Popen(command, shell = True, executable = "/bin/bash")

print 'starting to compile examples code ....'
examples_dir = current + '/c++/examples'

Program('#c++/examples/wordcount',
		'wordcount.cpp',
		LIBS = ['spark_api', 'jvm'],
		srcdir = '#c++/examples/')

Program('#c++/examples/kmeans',
		'kmeans.cpp',
		LIBS = ['spark_api', 'jvm'],
		srcdir = '#c++/examples/')
