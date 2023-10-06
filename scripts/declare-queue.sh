#!/bin/sh

for i in $(seq 1 1 $1)
do
  amqp-declare-queue -q "q$i" &
  sleep 1
done
