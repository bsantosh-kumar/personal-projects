#!/bin/bash
if(($# != 1))
then 
    echo "Not enough arguments"
    exit 1
fi
fileName=$(echo "$1.c" )
gcc ${fileName} -o $1 "-lm" "-pthread"
p=(1 2 4 8)
n=()
if(($1=="matmult"))
then
    n=(100)
else
    n=(1000 10000 100000 1000000)
fi
outputFile=$(echo "$1-output.txt")
echo -n "" > "${outputFile}"
for i in "${p[@]}"
do
    for j in "${n[@]}"
    do
        echo "thread: $i size:$j" >> "${outputFile}"
        avg=0
        for k in 1 2 3 4
        do 
            curr=$("./$1" $i $j)
            echo -n "$curr " >>"${outputFile}"
            avg=$(echo "$avg" + "$curr" | bc -l)
        done
        avg=$(echo "$avg/4" | bc -l )
        echo "" >> "${outputFile}"
        echo "avg:$avg"  >> "${outputFile}"
        echo "" >> "${outputFile}"
    done 
done