#!/bin/bash

for (( i=0; i<255; i+=2 )); do
    seed=$(printf "%02x000000" "$i")
    ./gen-bitmaps $seed
    aws --profile=rogue s3 cp --recursive out s3://rogue-star-bitmaps
    rm out/*
done

    
