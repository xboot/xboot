#!/bin/sh
#
# s5pv210 irom sd boot fusing tool.
# Author: jianjun jiang <jerryjianjun@gmail.com>


# display usage message
USAGE()
{
  echo Usage: $(basename "$0") '<device> <bootloader>'
  echo '       device     = disk device name for SD card.'
  echo '       bootloader = /path/to/xboot.bin'
  echo 'e.g. '$(basename "$0")' /dev/sdb xboot.bin'
  exit 1
}

[ `id -u` == 0 ] || { echo "you must be root user"; exit 1; }
[ -z "$1" -o -z "$2" ] && { USAGE; exit 1; }

dev="$1"
xboot="$2"

# validate parameters
[ -b "${dev}" ] || { echo "${dev} is not a valid block device"; exit 1; }
[ X"${dev}" = X"${dev%%[0-9]}" ] || { echo "${dev} is a partition, please use device: perhaps ${dev%%[0-9]}"; exit 1; }
[ -f ${xboot} ] || { echo "${xboot} is not a bootloader file."; exit 1; }

# copy the full bootloader image to the right place after the partitioned area
dd iflag=dsync oflag=dsync if="${xboot}" of="${dev}" seek=1

sync;
sync;

echo "^_^ completed."

