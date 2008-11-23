#!/bin/sh

if type i586-pc-meinos-gcc > /dev/null 2> /dev/null ; then exit 0;
fi

echo "No meinos-XGCC found. Please install it. Patches are gcc-4.2.2-meinos.patch and binutils-2.18-meinos.patch"
exit 1