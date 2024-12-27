#!/bin/bash

# Clear the custom binary directory.
echo "Clearing the custom binary directory..."
rm -rfv ~/.cbin

# Clear the xvman directory.
echo "Clearing the custom binary directory..."
rm -rfv ~/.config/xvman

# edit the bash configuration file.
head -n -2 ~/.bashrc > ~/bashrc.tmp && mv ~/bashrc.tmp ~/.bashrc
