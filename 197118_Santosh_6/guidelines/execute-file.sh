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
if [ $1 == "matmult" ]
then
    n=(128 256 512 1024)
else
    n=(1000 10000 100000 1000000)
fi

outputFile=$(echo "$1-output.csv")
echo -n "" > "${outputFile}"
echo "Size,Number of threads,Execution time1,Execution time2,Execution time3,Execution time4,Average" >> "${outputFile}"
for j in "${n[@]}"
do
    for i in "${p[@]}"
    do
        # echo "thread: $i size:$j" >> "${outputFile}"
        avg=0
        echo -n "$j,$i," >> "${outputFile}"
        for k in 1 2 3 4
        do 
            curr=$("./$1" $i $j)
            echo -n "$curr, " >>"${outputFile}"
            avg=$(echo "$avg" + "$curr" | bc -l)
        done
        avg=$(echo "$avg/4" | bc -l )
        echo "$avg"  >> "${outputFile}"
    done 
done