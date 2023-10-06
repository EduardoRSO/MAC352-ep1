#!/bin/bash
for i in $(seq 1 1 $1)
do
  for j in $(seq 1 1 $2)
  do
    gnome-terminal --tab --title="$i$j" -- amqp-consume -q q$i cat &
    sleep 1
  done
done
