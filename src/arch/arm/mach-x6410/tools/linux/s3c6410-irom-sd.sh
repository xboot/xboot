#!/bin/sh
#
# s3c6410 irom sd boot fusing tool.
# Author: jianjun jiang <jerryjianjun@gmail.com>


echo "s3c6410 irom sd boot fusing tool."
echo "jianjun jiang <jerryjianjun@gmail.com>"
echo

# display usage message and exit
# any arguments are displayed as an error message
USAGE()
{
  echo
  [ -z "$1" ] || echo Error: $*
  echo 's3c6410 irom sd boot fusing tool.'
  echo
  echo Usage: $(basename "$0") '<device> <card> <bootloader>'
  echo '       device     = disk device name for SD card.'
  echo '       card       = sd | sdhc'
  echo '       bootloader = /path/to/xboot.bin'
  echo 'e.g. '$(basename "$0")' /dev/sdb sd xboot.bin'
  exit 1
}

[ `id -u` == 0 ] || { echo "Error: You must be root user."; exit 1; }
[ -z "$1" -o -z "$2" -o -z "$3" ] && USAGE 'Missing arguments'

dev="$1"
card="$2"
xboot="$3"

[ -f ${xboot} ] || { echo "Error: ${xboot} is not a bootloader file."; exit 1; }
XBOOT_SIZE=`ls -l ${xboot} | awk '{ print $5 }'`
XBOOT_SIZE=`echo "(($XBOOT_SIZE+256*1024)/(256*1024))*256"|bc`

XBOOT_ALLOCATION=$(( ${XBOOT_SIZE} * 2 ))
XBOOT_INITIAL=$(( 8 * 2 ))
SIG=1

case "${card}" in
  [sS][dD][hH][cC])
    PADDING=1025
    ;;
  [sS][dD])
    PADDING=1
    ;;
  *)
    USAGE "${card} is an unknown card type"
esac

# the amount of space that must remain unused at the end of the disk
REAR_SECTORS=$(( $XBOOT_ALLOCATION + $XBOOT_INITIAL + $SIG + $PADDING ))

# validate parameters
[ -b "${dev}" ] || dev="/dev/${dev}"
[ -b "${dev}" ] || USAGE "${dev} is not a valid block device"
[ X"${dev}" = X"${dev%%[0-9]}" ] || USAGE "${dev} is a partition, please use device: perhaps ${dev%%[0-9]}"

# checking bootloader file.
[ -e "${xboot}" ] || USAGE "bootloader file: ${xboot} does not exist"

# get size of device
bytes=$(echo p | fdisk "${dev}" 2>&1 | sed '/^Disk.*, \([0-9]*\) bytes/s//\1/p;d')
SECTORS=`echo "$bytes/512"|bc`

[ -z "$SECTORS" ]  && USAGE "could not find size for ${dev}"
[ "$SECTORS" -le 0 ]  && USAGE "invalid size: '${SECTORS}' for ${dev}"

echo "${dev} is $SECTORS 512-byte blocks"

# copy the full bootloader image to the right place after the partitioned area
echo installing bootloader from: ${xboot}

dd if="${xboot}" of="${dev}" bs=512 count=$XBOOT_ALLOCATION seek=$(( $SECTORS - $REAR_SECTORS ))
dd if="${xboot}" of="${dev}" bs=512 count=$XBOOT_INITIAL seek=$(( $SECTORS - $REAR_SECTORS + $XBOOT_ALLOCATION ))
dd if=/dev/zero of="${dev}" bs=512 count=$(( $SIG + $PADDING )) seek=$(( $SECTORS - $REAR_SECTORS + $XBOOT_ALLOCATION + $XBOOT_INITIAL ))

sync;
sync;

echo "^_^ completed."

