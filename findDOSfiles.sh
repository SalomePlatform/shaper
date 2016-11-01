#!/bin/bash

function isDOSFile
{
  local FILENAME="$1" 
  file "$FILENAME" | grep -q "CRLF line terminators" 
}

if git-rev-parse --verify HEAD >/dev/null 2>&1 ; then
  against=HEAD
else
  # Initial commit: diff against an empty tree object
  against=4b825dc642cb6eb9a060e54bf8d69288fbee4904
fi

# Find files with DOS line endings
FOUND=0
for FILE in $(exec git diff-index --check --cached $against -- | sed '/^[+-]/d' | sed -r 's/:[0-9]+:.*//' | uniq) ; do
    isDOSFile "$FILE" 
    if (( $? == 0 ))
    then
      echo "\"$FILE\" has DOS line endings" >&2
      FOUND=1
    fi
done

exit $FOUND
