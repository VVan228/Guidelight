#!/bin/bash
cmake . 
make 
./guidelight -F $1 $2
