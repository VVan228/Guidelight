#!/bin/bash
cmake . 
make 
./guidelight V $1 $2
