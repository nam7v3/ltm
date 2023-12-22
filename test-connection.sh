#!/bin/sh

echo "Spawning $1 client..."
for ((i = 0 ; i < $1; i++)); do
  echo "Spawning client #$i"
  ./client localhost 8080 3 & 2>&1 >/dev/null ;
  sleep 1;
done
