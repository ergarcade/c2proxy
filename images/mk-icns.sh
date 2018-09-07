#!/bin/bash
#

PROG=ergarcade
ICONSET=${PROG}.iconset
PNG=${PROG}-logo.png

mkdir -p ${ICONSET}
for dim in 16 32 64 128 256; do
	sips -z ${dim} ${dim} ${PNG} --out ${ICONSET}/icon_${dim}x${dim}.png
done
for dim in 32 64 128 256 512; do
	d=$(($dim / 2))
	sips -z ${dim} ${dim} ${PNG} --out ${ICONSET}/icon_${d}x${d}@2x.png
done

iconutil -c icns ${ICONSET}
