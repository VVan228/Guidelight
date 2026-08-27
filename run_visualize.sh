#!/bin/bash
cmake . 
make 
./guidelight V $1 $2 | python3 visualize/visualizer.py
