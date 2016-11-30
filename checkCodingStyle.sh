#!/bin/bash

source env.sh

CPPLINT_ARGS=""
CPPLINT_ARGS="${CPPLINT_ARGS} --verbose=0"
CPPLINT_ARGS="${CPPLINT_ARGS} --recursive"
CPPLINT_ARGS="${CPPLINT_ARGS} --counting=detailed"
CPPLINT_ARGS="${CPPLINT_ARGS} --extensions=h,cpp"
CPPLINT_ARGS="${CPPLINT_ARGS} --exclude=src/XAO/* --exclude=src/XAO/tests/*"
CPPLINT_ARGS="${CPPLINT_ARGS} --output=vs7"

! cpplint.py ${CPPLINT_ARGS} . 2>&1 | grep -v Done
