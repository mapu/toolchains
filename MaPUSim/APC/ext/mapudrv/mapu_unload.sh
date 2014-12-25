#!/bin/sh
module="mapu"
device="ape"

/sbin/rmmod -f $module $* || exit 1

rm -f /dev/${device}

exit 0

