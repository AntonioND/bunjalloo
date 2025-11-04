#!/bin/bash

let i=0
let total=0
count=10
if [ $# -gt 0 ] ; then
  count=$1
fi

while [ $i -lt $count ]
do
  let i=$i+1
  pushd ../..
  timetaken=$(/usr/bin/time -f "%E" ./bunjalloo/bunjalloo_sdl.elf \
              tests/rendering/issue55.html 0 2>&1 | cut -d: -f2)
  total=$(python3 -c "print($total + $timetaken)")
  popd
done
python3 -c "print($total/$count)"
