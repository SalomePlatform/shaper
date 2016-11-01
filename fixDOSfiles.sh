#!/bin/bash

find src -type f -name "*.cpp" | xargs dos2unix $1 >/dev/null
find src -type f -name "*.h" | xargs dos2unix $1 >/dev/null
find src -type f -name "*.py" | xargs dos2unix $1 >/dev/null