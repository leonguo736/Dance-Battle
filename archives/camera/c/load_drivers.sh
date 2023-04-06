#!/bin/bash

# Store the current directory in a variable
MY_DIR=$(pwd)

# Change to the drivers directory and run the load_drivers script
cd ~/Linux_Libraries/drivers/
./load_drivers

# Return to the original directory
cd "$MY_DIR"