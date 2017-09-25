#!/bin/bash

for i in {0..90}
	do
		echo -e  "Starting tango test\e[1;32m" $i "\e[0m"
		./bin/LightTest $i &
	done


