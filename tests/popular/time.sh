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
  timetaken=$(/usr/bin/time -f "%E" ../_build_/sdl/bunjalloo/bunjalloo  tests/rendering/issue55.html 0 2>&1|cut -d: -f2)
  total=$(python -c "print $total + $timetaken")
done
python -c "print $total/$count"
