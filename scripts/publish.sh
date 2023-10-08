#!/bin/bash

echo "Publishing $2 messages in $1 queues"

for i in $(seq 1 1 $1);do
  for j in $(seq 1 1 $2);do  
    amqp-publish -r q$i -b "m$j" & 
    sleep 1
  done
done
