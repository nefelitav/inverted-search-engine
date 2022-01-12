#!/bin/bash

input="result.txt"
declare -a time_results=()
make clean
make
sleep 1
for i in {1..100}
do
    make run
    count=0
    success=0
    while IFS= read -r line
    do
        let count+=1
        tokens=( $line );
        if [[ $success == 1 &&  ${tokens[0]} == "Time" ]]; then
            time_results+=(${tokens[2]})
            break
        fi
        if [[ $line == "Your program has successfully passed all tests." ]]; then 
            success=1
        elif [[ $success == 0 && $count != 1 ]]; then
            echo "Run failed"
            exit 1
        fi
    done < "$input"
    echo $i
done

average_time=0
for i in "${time_results[@]}"
do
        let average_time+=$i
done

average_time=$(( average_time / 100 ))
echo "Average time = $average_time milli-seconds"
