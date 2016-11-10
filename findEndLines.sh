#!/bin/bash

find . \( -name "*.h" -o -name "*.cpp" \) -exec cpplint.py --filter=-,+whitespace/end_of_line  --verbose=0 --counting=detailed {} \+ 2>&1 | grep -v Done | tee ./tofix

python correction_auto_2.py --cpplint_output_file ./tofix

rm ./tofix