#!/bin/bash

function run_test {
	echo Test `basename $1` - $2 executions
	rm -fr tmp.$$
	let total=0
	for ex in `seq $2`
	do
		# Ignore stderr!
		timing[${ex}]=`$1 2>/dev/null | grep "^RESULT :" | cut -d " " -f 4`
		echo ${timing[${ex}]} >> tmp.$$
		let total=${total}+${timing[${ex}]} 
		rm -fr set-0 TRACE.mpits TRACE.sym
	done
	min=`sort -n tmp.$$ | head -1`
	let avg=${total}/$2
	max=`sort -n tmp.$$ | tail -1`
	echo min: ${min} ns
	echo avg: ${avg} ns
	echo max: ${max} ns
	echo  # Additional line
	rm -fr tmp.$$
}


export EXTRAE_CONFIG_FILE=extrae.xml
export LD_LIBRARY_PATH+=:/home/mogeb/paraver/extrae/lib:/usr/lib:/usr/lib/x86_64-linux-gnu/

EXECUTABLES="./posix_clock ./ia32_rdtsc_clock ./extrae_event ./extrae_nevent4"
EXECUTABLES+=" ./extrae_eventandcounters ./papi_read1 ./papi_read4"
EXECUTABLES+=" ./extrae_user_function ./extrae_get_caller1 ./extrae_get_caller6 ./extrae_trace_callers"

# Compile binaries first if they do not exist

echo Checking for existing binaries, and compiling if necessary ...

for e in ${EXECUTABLES}
do
	if test ! -x ${e} ; then
		make `basename ${e}`
	fi
done

echo
echo
echo ------ CUT HERE ------
echo 
echo

if test ! -x /home/mogeb/paraver/extrae/bin/extrae-header ; then
	echo Cannot locate extrae-header in /home/mogeb/paraver/extrae, installation corrupted!
	exit -1
else
	/home/mogeb/paraver/extrae/bin/extrae-header
fi

echo

for e in ${EXECUTABLES}
do
	run_test $e 10
done
