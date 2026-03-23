#!/bin/bash
# This script builds the xboot project for the csky-gx6605s target.

set -e

# The toolchain must be installed in /opt/csky-elf-tools before running this script.
# You can use the setup_csky_toolchain.sh script to do this.

export PATH=$PATH:/opt/csky-elf-tools/bin
export CROSS_COMPILE=csky-elf-
export PLATFORM=csky-gx6605s

echo "Building xboot for platform: $PLATFORM with toolchain prefix: $CROSS_COMPILE"

cd src
make

echo "Build complete. The xboot binary can be found in the output/ directory."
