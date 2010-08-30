#!/bin/sh

AUTH='/C=US/ST=California/L=Mountain View/O=Android/OU=Android/CN=Android/emailAddress=android@android.com'

if [ "$1" == "" ]; then
	echo "Create a certificate key."
	echo "Usage: $0 NAME"
	echo "    Will generate NAME.pk8 and NAME.x509.pem"
	echo "    "
	exit
fi

openssl genrsa -3 -out $1.pem 2048
openssl req -new -x509 -sha1 -key $1.pem -out $1.x509.pem -days 10000 -subj "$AUTH"
openssl pkcs8 -in $1.pem -topk8 -outform DER -out $1.pk8 -nocrypt
rm -fr $1.pem

java -jar DumpPublicKey.jar $1.x509.pem > $1.c

