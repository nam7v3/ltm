#!/bin/sh

echo "Spawning $1 client..."
for ((i = 0 ; i < $1; i++)); do
  echo "Spawning client #$i"
  ./build/ltm_client localhost 8080 3 & 2>&1 >/dev/null ;
  sleep 0.1;
done
