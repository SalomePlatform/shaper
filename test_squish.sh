#!/bin/bash -x

a_dir=$(dirname $0)

set -e

if [ "$#" = 1 ]; then
  SQUISHSERVER_PORT=$1
elif [ -z ${SQUISHSERVER_PORT} ]; then
  SQUISHSERVER_PORT=4320
fi

export TEST_DATA_DIR=$(cd ${a_dir}; pwd)/test.squish/shared/testdata

source ${a_dir}/env_squish.sh

for aut in linux_run.sh salome_run.sh; do
  squishserver --config addAUT ${aut} $(pwd)
done
squishserver --config setAUTTimeout 120
squishserver --verbose --port=${SQUISHSERVER_PORT} --stop
squishserver --verbose --port=${SQUISHSERVER_PORT} 2>server.err &

squishrunner --port=${SQUISHSERVER_PORT} --config setCursorAnimation off

RETVAL=0

squishrunner_run() {
  local suite=$1
  local tests=$2

  set +x
  local tests_arg=
  for test in ${tests}; do
    tests_arg="${tests_arg} --testcase ${test}"
  done
  set -x

  set +e
  squishrunner --port=${SQUISHSERVER_PORT} --testsuite ${suite} ${tests_arg} --reportgen stdout --exitCodeOnFail 1
  EXIT_CODE=$?
  set -e
  if [ ${EXIT_CODE} = '1' ]; then RETVAL=1; fi
}

squishrunner_run ./test.squish/suite_ISSUES 'tst_BASE tst_DISTANCE tst_PARALLEL_1 tst_PARALLEL_2 tst_PERPENDICULAR_1 tst_RADIUS tst_c tst_common_1 tst_crash_1 tst_818 tst_532'
squishrunner_run ./test.squish/suite_ISSUES_SALOME 'tst_sketch_001 tst_sketch_002 tst_sketch_003 tst_sketch_004 tst_sketch_005 tst_sketch_006 tst_sketch_007 tst_sketch_008 tst_sketch_009 tst_sketch_010 tst_sketch_011 tst_474 tst_532 tst_576 tst_679'

squishserver --verbose --port=${SQUISHSERVER_PORT} --stop
for aut in linux_run.sh salome_run.sh; do
  squishserver --config removeAUT ${aut} $(pwd)
done

exit ${RETVAL}
