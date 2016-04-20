#!/bin/bash

export EXTRAE_CONFIG_FILE=extrae.xml
export EXTRAE_HOME=/home/mogeb/paraver/extrae
source ${EXTRAE_HOME}/etc/extrae.sh
export EXECUTABLE=./pi_instrumentedf # This is taken by extrae.xml

${EXECUTABLE}
