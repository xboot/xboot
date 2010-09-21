#!/bin/sh

if [ "$1" == "" -o "$2" == "" -o "$3" == "" -o "$4" == "" ]; then
	echo "Signature a file"
	echo "Usage: $0 public.x509.pem private.pk8 input output"
	echo "    Signature input file with the private key"
	echo "    "
	exit
fi

java -jar SignFile.jar $1 $2 $3 $4

