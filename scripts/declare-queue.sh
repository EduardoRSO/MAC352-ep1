#!/bin/bash
for i in {1..10}
do
  amqp-declare-queue -q "q$i" &
done
