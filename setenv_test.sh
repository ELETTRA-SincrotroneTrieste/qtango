#!/bin/bash

echo -e "\e[1;32m* \e[0m setting library paths for QTango 3.1 testing installation...\n"

export LD_LIBRARY_PATH=/testingqtango3.1/runtime/lib:/testingqtango3.1/runtime/elettra/lib:$LD_LIBRARY_PATH
export QT_PLUGIN_PATH=/testingqtango3.1/runtime/lib/plugins:/testingqtango3.1/runtime/elettra/lib/plugins

echo -e "\e[1;32m* \e[0mdone: \e[4mLD_LIBRARY_PATH\e[0m is " $LD_LIBRARY_PATH
echo -e "\e[1;32m* \e[0m      \e[4mQT_PLUGIN_PATH\e[0m is " $QT_PLUGIN_PATH


