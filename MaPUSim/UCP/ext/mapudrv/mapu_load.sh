#!/bin/sh
module="mapu"
device="ape"
mode="666"

rm -f /dev/${device}

/sbin/insmod -f ./$module.ko $* || exit 1

major=`cat /proc/devices | awk "\\$2==\"$module\" {print \\$1}"`
major=`echo ${major} | awk "{print \\$1}"`

mknod /dev/${device} c ${major} 0

chmod $mode /dev/${device}

