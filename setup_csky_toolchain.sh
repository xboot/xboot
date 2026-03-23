#!/bin/bash
# This script downloads and installs the C-SKY toolchain.

set -e

TOOLCHAIN_DIR="/opt/csky-elf-tools"
TOOLCHAIN_URL="https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/resource/1356021/1619529266702/csky-elf-tools-x86_64-minilibc-20210423.tar.gz"

echo "This script requires sudo to create directories in /opt."
echo "Creating toolchain directory at $TOOLCHAIN_DIR..."
sudo mkdir -p "$TOOLCHAIN_DIR"

echo "Downloading and extracting C-SKY toolchain..."
curl -L "$TOOLCHAIN_URL" | sudo tar -xz -C "$TOOLCHAIN_DIR"

echo "C-SKY toolchain installed successfully in $TOOLCHAIN_DIR"
echo "To use the toolchain, you can add $TOOLCHAIN_DIR/bin to your PATH:"
echo "export PATH=\$PATH:$TOOLCHAIN_DIR/bin"
