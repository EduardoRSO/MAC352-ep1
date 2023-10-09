#!/bin/bash

# get_cpu_network_from_docker SAMPLE_SIZE NUMBER_OF_CONSUMERS 
echo "Observing docker stats $1 times and saving in data/$2.txt"

for i in $(seq 1 1 $1)
do
  echo $(docker stats ep1 --format "{{.CPUPerc}} | {{.NetIO}}" --no-stream) >> ../output/$2.txt
done

echo "GET_CPU_FROM_DOCKER ENDED $1"
