#!/bin/sh
#

# some base directory.
export CWD=$(cd `dirname $0` ; pwd)
export CDIR=${CWD}/../../src/framework
export LDIR=${CWD}/../../src/romdisk/framework
export OUTPUT_API=${CWD}/../../output/api
export OUTPUT_DOC=${CWD}/../../output/doc
export FRAMEWORKS=" \
	${LDIR}/org/xboot/core \
	${LDIR}/org/xboot/display \
	${LDIR}/org/xboot/lang \
	${LDIR}/org/xboot/event \
	${LDIR}/org/xboot/timer \
	${LDIR}/org/xboot/widget"

# gen framework doc and api
rm -fr ${OUTPUT_DOC} ${OUTPUT_API}
lua luadocumentor.lua -f doc -d ${OUTPUT_DOC} ${FRAMEWORKS}
lua luadocumentor.lua -f api -d ${OUTPUT_API} ${FRAMEWORKS}
