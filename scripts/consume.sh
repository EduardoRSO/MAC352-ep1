#!/bin/bash
for i in {1..10}
do
  for j in {1..2}
  do
    gnome-terminal --tab --title="$i_$j" -- amqp-consume -q q$i cat
  done
done
