#!/bin/bash

my_array=()

for i in {1..32768}; do
  my_array+=($RANDOM)
done

# Insert the integer 10240 at a random position in the array
random_index=$((RANDOM % 32768))
my_array=("${my_array[@]:0:$random_index}" 10240 "${my_array[@]:$random_index}")

parallel_execute array_search 10240 "${my_array[@]}"
echo "  "
echo "Sorting the array..."
echo "  "
parallel_execute sort 8 "${my_array[@]}"
