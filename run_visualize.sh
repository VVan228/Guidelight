#!/bin/bash
cmake .
make 
./guidelight -V -F $1 $2 | python3 visualize/visualizer.py
