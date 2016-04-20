#!/bin/bash

export EXTRAE_HOME=/home/mogeb/paraver/extrae
source ${EXTRAE_HOME}/etc/extrae.sh

${EXTRAE_HOME}/bin/extrae -config extrae.xml ./pi_forked
