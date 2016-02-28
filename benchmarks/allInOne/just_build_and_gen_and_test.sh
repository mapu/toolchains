#!/bin/bash
./proj_manager.sh -b -g > results/out 2> results/err; grep -nr rror results
