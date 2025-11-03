#!/bin/bash
cd $(dirname $0)
startat=""
if test $# -gt 0 ; then
  startat=$1
fi
BUNJALLOO=../_build_/sdl/bunjalloo/bunjalloo
while read line
do
  if ! test -z $startat ; then
    echo $line| grep $startat  > /dev/null
    if test $? -eq 0 ; then
      startat=""
    else
      continue
    fi
  fi
  cd ../.. >/dev/null
  echo $line...
  $BUNJALLOO $line 0 &> /dev/null
  if test  $? -ne 0; then
    echo $line caused a crash
    exit 1
  fi
  cd - >/dev/null
done < input.txt

echo "All done!"
