#!/bin/bash
set -e 

export PICO_PATH=/home/pico
export PICO_SDK_PATH=${PICO_PATH}/pico-sdk
export PICO_EXTRAS_PATH=${PICO_PATH}/pico-extras
export GVGA_HOME=/Users/greg/git/GVga

clear
env | grep PICO
rm -rf build
mkdir -p build
cd build

# It's important to include PICO_COPY_TO_RAM=1 
# since it increases performance for some of the applications 
# because they use SSI in fast DMA streaming mode.

cmake -DPICO_COPY_TO_RAM=1  ..

#make VERBOSE=1
make -j4
