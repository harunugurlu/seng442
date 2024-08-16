#!/bin/bash

# Set parameters
N=2048             # Number of vertices
FILENAME="input2048.txt"  # Output filename
MAX_WEIGHT=100     # Maximum edge weight
NO_CONNECTION=9999999  # Indicator of no direct connection

# Create and open the file
echo "Generating $N x $N adjacency matrix..."
> $FILENAME

# Generate the matrix
for ((i=0; i<N; i++))
do
    for ((j=0; j<N; j++))
    do
        if [ $i -eq $j ]; then
            # No self-loops, distance to self is 0
            echo -n "0 " >> $FILENAME
        else
            # Randomly decide to place an edge or not
            if [ $((RANDOM % 2)) -eq 1 ]; then
                # Random weight between 1 and MAX_WEIGHT
                echo -n "$((RANDOM % MAX_WEIGHT + 1)) " >> $FILENAME
            else
                # No edge between these vertices
                echo -n "$NO_CONNECTION " >> $FILENAME
            fi
        fi
    done
    echo "" >> $FILENAME # New line for the next row of the matrix
done

echo "Matrix generated and saved to $FILENAME"