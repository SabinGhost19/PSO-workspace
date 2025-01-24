#!/bin/bash

SERVER="localhost"
PORT=8080
NUM_REQUESTS=40
MESSAGE="MAMA"

for i in $(seq 1 $NUM_REQUESTS)
do
  {
  
    echo -n "$MESSAGE" | nc $SERVER $PORT &
  }
done

wait
echo "Toate cererile au fost trimise."
