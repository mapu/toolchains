#!/bin/sh

#This script will generate the diff files 
# $1 : The dir1
# $2 : The dir2
# $3 : The output dir 

ListOnly=0
SingleFile=0

ex_pattern=".svn clang compiler-rt lldb UCPM MSPU MMPU MAPUTest MMPULite *.o *.so *.a"
ex_str=''
for ex in $ex_pattern; do
   ex_str="$ex_str -x $ex";
done

#####################################################################
# Generate the file list
if [ $ListOnly -eq 1 ]; then 
    diff -qNr $ex_str $1 $2 | awk '{print $2}'
    exit;
fi
#####################################################################
#Generate seperate diff files 
if ! [ $SingleFile -eq 1 ]; then 
    file_list=`diff -qNr $ex_str $1 $2 | awk '{print $2}' | cut -b 26-`
    for file in $file_list; do
    	file1=$1/$file
    	file2=$2/$file
    	output_file=$3/`echo $file | sed -r 's/\//--/g' | sed -r 's/\..*$/\.md/g'`
    	echo '```diff' > $output_file	
    	diff -N $file1 $file2 >>  $output_file
    	echo '```' >> $output_file
    done
else
#####################################################################
#Generate single diff files 
    echo '```diff' > $3 
    diff -Nru $ex_str $1 $2 >> $3
    echo '```'    >> $3
fi
