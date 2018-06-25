#!/bin/bash

PRJ_DIR=project
DEM_DIR=demo
LIB_DIR=lib
RMV_ALL=0
DEST_BASE=$(dirname "$0")/..

for var in "$@"; do
    if [ $var == "-a" ] || [ $var == "--all" ]; then
        RMV_ALL=1
        break
    fi
done

do_remove()
{
  find "$1" -name "$2" | xargs rm -rf
}

cd $DEST_BASE

do_remove $PRJ_DIR Debug
do_remove $PRJ_DIR obj
do_remove $DEM_DIR Debug
do_remove $DEM_DIR obj
do_remove $LIB_DIR Debug
do_remove $LIB_DIR obj

do_remove . *.sdf
do_remove . *.VC.db
do_remove . *.VC.db-shm
do_remove . *.VC.db-wal
do_remove . *.cki

if [ $RMV_ALL -eq 1 ]; then
    do_remove lib "*.so.*"
fi
