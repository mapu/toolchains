#!/bin/bash

sleep 3
while [ 1 ]
do
  if [ -e m5out/apc.out ]
  then
    ${MAPU_HOME}/simulator/apc/utils/mapu-pipeview.py --color m5out/apc.out -o m5out/mapu-pipeview.out
    cat m5out/mapu-pipeview.out
    read -p "Press ENTER to refresh pipeline view('q' to quit)" -n 1 key
    if [ "$key" == 'q' ]
    then break
    fi
    reset
  fi
done

