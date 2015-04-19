#!/bin/bash
esc="\033["

echo -ne "${esc}40;34;1m"
echo -ne "slice"
echo -ne "${esc}40;32;1m"
echo -ne "host"
echo -ne "${esc}0m"
echo -ne "reset\n"
