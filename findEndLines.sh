#!/bin/bash

FIX_LINE_ENDINGS=0
while [[ $# > 0 ]]; do
  key="$1"
  
  case $key in
    fix)
      FIX_LINE_ENDINGS=1
      shift
      ;;
    *)
      shift
      ;;
  esac
done

source env.sh

find . \( -name "*.h" -o -name "*.cpp" \) -exec cpplint.py --filter=-,+whitespace/end_of_line  --verbose=0 --counting=detailed {} \+ 2>&1 | grep -v Done | tee ./tofix

WRONG=0
if [ -s ./tofix ]; then
  if [[ ${FIX_LINE_ENDINGS} == 1 ]]; then
    python correction_auto_2.py --cpplint_output_file ./tofix
    echo "NOTE: Line endings are fixed"
  else
    WRONG=1
  fi
fi

rm ./tofix
exit $WRONG
