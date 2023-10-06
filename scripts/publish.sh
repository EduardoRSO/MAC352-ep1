#!/bin/bash
for i in {1..10}
do
  for j in {1..3}
  do  
    amqp-publish -r q$i -b "m$j" &
  done
 done
