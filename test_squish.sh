#!/bin/bash -x

set -e

if [ "$#" = 1 ]; then
  SQUISHSERVER_PORT=$1
elif [ -z ${SQUISHSERVER_PORT} ]; then
  SQUISHSERVER_PORT=4320
fi

source env_squish.sh

for aut in linux_run.sh salome_run.sh; do
  squishserver --config addAUT ${aut} $(pwd)
done
squishserver --config setAUTTimeout 60
squishserver --verbose --port=${SQUISHSERVER_PORT} --stop
squishserver --verbose --port=${SQUISHSERVER_PORT} --daemon

RETVAL=0
for suite in ./test.squish/suite_*
do
  set +e
  squishrunner --port=${SQUISHSERVER_PORT} --testsuite ${suite} --reportgen stdout --exitCodeOnFail 1
  EXIT_CODE=$?
  set -e
  if [ ${EXIT_CODE} = '1' ]; then RETVAL=1; fi
done

squishserver --verbose --port=${SQUISHSERVER_PORT} --stop
for aut in linux_run.sh salome_run.sh; do
  squishserver --config removeAUT ${aut} $(pwd)
done

exit ${RETVAL}
