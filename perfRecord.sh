#!/bin/bash

timestamp=$(date +"%Y%m%d%H%M%S")

# Start the perf stat
perf stat ./program --stdio >perfStats_$timestamp.txt

# Start the perf record
sudo perf record -g --call-graph dwarf ./program

# Generate the perf report
sudo perf report -G --stdio >perfReport_$timestamp.txt

# Generate the perf script
sudo perf script >perfScript_$timestamp.txt
