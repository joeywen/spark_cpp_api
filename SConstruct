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
spark_process = subprocess.Popen('sbt/sbt package', shell = True, executable = "/bin/bash")
spark_process.wait()

if spark_process.returncode != 0:
	print "Buil spark error ! \n"
	sys.exit()

#decompress spark jar 
if os.path.exists(home + '/.class'):
    command = 'cd ' + home + '/.class && jar -xf ' + current + '/core/target/spark*.jar && cd ' + current
else:
    command = 'mkdir ' + home + '/.class && cd ' + home + '/.class && jar -xf ' + current + '/core/target/spark*.jar && cd ' + current
    
spark_process = subprocess.Popen(command , shell = True, executable = "/bin/bash")
spark_process.wait()

#set java.class.path
os.environ['SPARK_CLASS_PATH'] = home + '/.class'

env = env.Append(CPPPATH = [java_home + '/include', 
							java_home + '/include/linux',
							java_home + '/jre/lib/amd64/server'])

sources = Glob('*.c*')
spark_jni_so = env.SharedLibrary('#c++/spark/jni/spark_jni',
				sources,
				LIBS = 'jvm',
				srcdir = '#c++/spark/jni')

spark_api_so = env.SharedLibrary('#c++/spark/spark_api',
				sources,
				LIBS = 'jvm',
				srcdir = '#c++/spark/')

header_files = Glob('#c++/spark/*.h*')

env.Alias('install-lib', Install(os.path.join(prefix, "/lib"), spark_jni_so, spark_api_so))
env.Alias('install-include', Install(os.path.join(prefix, "/include"), header_files))
env.Alias('install', ['install-include', 'install-lib'])

from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.Chmod = SCons.ActionFactory(os.chmod, lambda dest, mode: 'Chmod("%s", "0%o")' % (dest, mode))

def InstallPerm(env, dest, files, perm):
	obj = env.Install(dest, files)
	for i in obj:
		env.AddPostAction(i, env.Chmod(str(i), perm))
	return dest

SConsEnvironment.InstallPerm = InstallPerm
SConsEnvironment.InstallProgram = lambda env, dest, files: InstallPerm(env, dest, files, 0755)
SConsEnvironment.InstallHeader = lambda env, dest, files: InstallPerm(env, dest, files, 0644)

header_files = Glob('#c++/spark/*.h*')
env.InstallHeader(include_dir, header_files)

try:
	umask = os.umask(022)
	print 'setting umask to 022 (was )%o)' % umask
except:
	pass

examples_dir = current + '/c++/examples'

env.Program('#c++/examples/wordcount',
		'#c++/examples/wordcount.cpp',
		LIBS = ['spark_api', 'jvm'],
		srcdir = '#c++/examples/')

env.Program('#c++/examples/kmeans',
		'#c++/examples/kmeans.cpp',
		LIBS = ['spark_api', 'jvm'],
		srcdir = '#c++/examples/')
