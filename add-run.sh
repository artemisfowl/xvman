#!/bin/bash

# shell script for automating the task of testing xvman
# author: okdgod
# date Wed, 25 Dec 2024 09:59:13 +0530

make clean && make
cd build
./xvman -d -a goneovim /home/oldgod/data/code/test_bed/skunkworks/nvim/c/xvman/resources/goneovim-v0.6.9-linux/goneovim
./xvman -d -a goneovim /home/oldgod/data/code/test_bed/skunkworks/nvim/c/xvman/resources/goneovim-v0.6.11-linux/goneovim
valgrind --leak-check=full ./xvman -d -a goneovim /home/oldgod/data/code/test_bed/skunkworks/nvim/c/xvman/resources/goneovim-v0.6.10-linux/goneovim
