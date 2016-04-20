#!/bin/bash

export OMP_NUM_THREADS=4
export EXTRAE_CONFIG_FILE=extrae.xml
export EXTRAE_HOME=/home/mogeb/paraver/extrae
source ${EXTRAE_HOME}/etc/extrae.sh

./pi_instrumented
