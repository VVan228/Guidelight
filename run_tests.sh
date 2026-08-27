#!/bin/bash

default_agents=5
default_states=5
default_props=5

agent_start=5
agent_step=5
agent_end=50

props_start=109
props_step=1
props_end=110

states_start=7
states_step=7
states_end=35

cmake . && make 
if [ $1 == "agents" ]; then
    folder=".results/agent_change"
    rm -rf $folder 2> /dev/null
    mkdir -p "$folder";
    for i in $(seq $agent_start $agent_step $agent_end); do
        file="${i}_${default_states}_${default_props}_${2}"
        ./guidelight V R $i $default_states $default_props $2 | grep time:: \
            > ${folder}/${file}
    done
fi

if [ $1 == "props" ]; then
    folder=".results/props_change"
    rm -rf $folder 2> /dev/null
    mkdir -p "$folder";
    for i in $(seq $props_start $props_step $props_end); do
        file="${default_agents}_${default_states}_${i}_${2}"
        ./guidelight V R $default_agents $default_states $i $2 | grep time:: \
            > ${folder}/${file}
    done
fi

if [ $1 == "states" ]; then
    folder=".results/size_change"
    rm -rf $folder 2> /dev/null
    mkdir -p "$folder";
    for i in $(seq $states_start $states_step $states_end); do
        file="${default_agents}_${i}_${default_props}_${2}"
        ./guidelight V R $default_agents $i $default_props $2 | grep time:: \
            > ${folder}/${file}
    done
fi
