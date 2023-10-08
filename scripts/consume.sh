#!/bin/bash
echo "Setting $2 consumers in $1 queues"
for i in $(seq 1 1 $1);do
  for j in $(seq 1 1 $2);  do
    amqp-consume -q q$i cat & 
    sleep 1
  done
done
