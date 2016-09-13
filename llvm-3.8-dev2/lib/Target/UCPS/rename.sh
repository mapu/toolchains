#!/bin/bash
#for file in ""
for file in $(find . -type f)
do
  mv $file $(echo $file|sed 's/MSPU/UCPS/g')
done