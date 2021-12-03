#!/bin/bash
if(($# != 1))
then 
    echo "Not enough arguments"
    exit 1
fi
fileName=$(echo "$1.c" )
gcc ${fileName} -o $1 "-lm" "-pthread"
no_of_threads=(1 2 4 8)
no_of_elements=()
if [ $1 == "matmult" ]
then
    no_of_elements=(128 256 512 1024)
else
    no_of_elements=(1000 10000 100000 1000000)
fi

outputFile=$(echo "$1-output.csv")
echo -n "" > "${outputFile}"
echo "Size,Number of threads,Execution time1,Execution time2,Execution time3,Execution time4,Average" >> "${outputFile}"
for i in "${no_of_threads[@]}"
do
    for j in "${no_of_element[@]}"
    do
        # echo "thread: $i size:$j" >> "${outputFile}"
        avg=0
        echo -n "$i,$j," >> "${outputFile}"
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