#!/bin/bash
# Wrapper for .checkpatch.pl

if [ $# -eq 0 ]
  then
    for f in $(find . -name '*.c' -or -name '*.h'); do
    	perl .checkpatch.pl --no-tree --no-signoff --show-types --ignore SPDX_LICENSE_TAG --file $f
    done
else
	perl .checkpatch.pl --no-tree --no-signoff --show-types --ignore SPDX_LICENSE_TAG --file $1 
fi
