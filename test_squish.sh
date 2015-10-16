#!/bin/bash

# This sctipt uses:
# SOURCES_DIR - where test.squish is placed
# AUT_DIR - where linux_run.sh, salome_run.sh are placed
# SQUISH_PORT - the port for squishserver
# PATH - should contain squishserver, squishrunner

export TEST_DATA_DIR=${SOURCES_DIR}/test.squish/shared/testdata
export SQUISH_GRABWINDOW_CLASSES=AppElements_ViewPort,OCCViewer_ViewPort3d

SERVEROPTIONS_ARGS="${SERVEROPTIONS_ARGS} --port=${SQUISH_PORT}"

# config squishserver
for aut in linux_run.sh salome_run.sh; do
  squishserver --config addAUT ${aut} ${AUT_DIR}
done
squishserver --config setAUTTimeout 120
# stop previous version
squishserver ${SERVEROPTIONS_ARGS} --verbose --stop >/dev/null 2>&1
# start squishserver
squishserver ${SERVEROPTIONS_ARGS} --verbose 1>log_squishserver 2>err_squishserver &

# config squishrunner
squishrunner --port=${SQUISH_PORT} --config setCursorAnimation off
# start squishrunner

RETVAL=0

squishrunner-run() {
  local TESTSUITE=$1
  local TESTCASES="$2"

  local SQUISHRUNNER_ARGS=""
  SQUISHRUNNER_ARGS="${SQUISHRUNNER_ARGS} --testsuite ${SOURCES_DIR}/test.squish/${TESTSUITE}"
  SQUISHRUNNER_ARGS="${SQUISHRUNNER_ARGS} --resultdir ${SOURCES_DIR}/test.squish_results/${TESTSUITE}"
  SQUISHRUNNER_ARGS="${SQUISHRUNNER_ARGS} --reportgen xmljunit,${SOURCES_DIR}/test.squish_results/${TESTSUITE}_results.xml"
  SQUISHRUNNER_ARGS="${SQUISHRUNNER_ARGS} ${TESTCASES}"
  SQUISHRUNNER_ARGS="${SQUISHRUNNER_ARGS} --exitCodeOnFail 1"
  squishrunner ${SERVEROPTIONS_ARGS} ${SQUISHRUNNER_ARGS}
  EXIT_CODE=$?
  if [ ${EXIT_CODE} = '1' ]; then RETVAL=1; fi
}

TESTCASES=""
TESTCASES="${TESTCASES} --testcase tst_BASE"
TESTCASES="${TESTCASES} --testcase tst_PARALLEL_1"
TESTCASES="${TESTCASES} --testcase tst_PARALLEL_2"
TESTCASES="${TESTCASES} --testcase tst_PERPENDICULAR_1"
#TESTCASES="${TESTCASES} --testcase tst_532"
TESTCASES="${TESTCASES} --testcase tst_818"
TESTCASES="${TESTCASES} --testcase tst_c"
TESTCASES="${TESTCASES} --testcase tst_common_1"
TESTCASES="${TESTCASES} --testcase tst_crash_1"
TESTCASES="${TESTCASES} --testcase tst_DISTANCE"
TESTCASES="${TESTCASES} --testcase tst_RADIUS"
squishrunner-run suite_STANDALONE "${TESTCASES}"

TESTCASES=""
TESTCASES="${TESTCASES} --testcase tst_679"
squishrunner-run suite_ERROR_NOTIFICATION "${TESTCASES}"

TESTCASES=""
TESTCASES="${TESTCASES} --testcase tst_474"
TESTCASES="${TESTCASES} --testcase tst_576"
TESTCASES="${TESTCASES} --testcase tst_903"
squishrunner-run suite_FEATURE_PARAMETERS "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_532"
#squishrunner-run suite_ISSUES_SALOME "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_boolean_001"
#TESTCASES="${TESTCASES} --testcase tst_boolean_002"
#TESTCASES="${TESTCASES} --testcase tst_boolean_003"
#squishrunner-run suite_FEATURE_BOOLEAN "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_construction_001"
#squishrunner-run suite_FEATURE_CONSTRUCTION "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_extrusion_001"
#TESTCASES="${TESTCASES} --testcase tst_extrusion_002"
#TESTCASES="${TESTCASES} --testcase tst_extrusion_003"
#TESTCASES="${TESTCASES} --testcase tst_extrusion_004"
#TESTCASES="${TESTCASES} --testcase tst_extrusion_006"
#squishrunner-run suite_FEATURE_EXTRUSION "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_partition_001"
#squishrunner-run suite_FEATURE_PARTITION "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_revolution_001"
#TESTCASES="${TESTCASES} --testcase tst_revolution_003"
#TESTCASES="${TESTCASES} --testcase tst_revolution_004"
#TESTCASES="${TESTCASES} --testcase tst_revolution_005"
#TESTCASES="${TESTCASES} --testcase tst_revolution_006"
#squishrunner-run suite_FEATURE_REVOLUTION "${TESTCASES}"

TESTCASES=""
#TESTCASES="${TESTCASES} --testcase tst_sketch_001"
#TESTCASES="${TESTCASES} --testcase tst_sketch_002"
#TESTCASES="${TESTCASES} --testcase tst_sketch_003"
#TESTCASES="${TESTCASES} --testcase tst_sketch_004"
#TESTCASES="${TESTCASES} --testcase tst_sketch_005"
#TESTCASES="${TESTCASES} --testcase tst_sketch_006"
#TESTCASES="${TESTCASES} --testcase tst_sketch_007"
#TESTCASES="${TESTCASES} --testcase tst_sketch_008"
#TESTCASES="${TESTCASES} --testcase tst_sketch_009"
#TESTCASES="${TESTCASES} --testcase tst_sketch_010"
#TESTCASES="${TESTCASES} --testcase tst_sketch_011"
#TESTCASES="${TESTCASES} --testcase tst_sketch_012"
#TESTCASES="${TESTCASES} --testcase tst_sketch_013"
#TESTCASES="${TESTCASES} --testcase tst_sketch_014"
#TESTCASES="${TESTCASES} --testcase tst_sketch_015"
#TESTCASES="${TESTCASES} --testcase tst_sketch_016"
#TESTCASES="${TESTCASES} --testcase tst_sketch_017"
#TESTCASES="${TESTCASES} --testcase tst_sketch_018"
#squishrunner-run suite_FEATURE_SKETCH "${TESTCASES}"

# stop squishserver
squishserver ${SERVEROPTIONS_ARGS} --verbose --stop
for aut in linux_run.sh salome_run.sh; do
  squishserver --config removeAUT ${aut} ${AUT_DIR}
done

exit ${RETVAL}
