#!/bin/bash -x

SQUISHSERVER_PORT=4320

source env_squish.sh
squishserver --verbose --port=${SQUISHSERVER_PORT} $@ 
