#!/bin/bash

SAMPLE_SIZE=100
MESSAGES=30
docker start ep1;
echo "Started docker container";
sleep 5;
./declare-queue.sh $1;
./publish.sh $1 $MESSAGES & 
./consume.sh $1 1 &
echo "Started get_cpu_network_from_docker"
./get_cpu_network_from_docker.sh $SAMPLE_SIZE $1
docker kill ep1;
echo "Killed docker container";
sleep 5;
pkill -TERM -g $$
