#!/bin/bash

OUT_PATH="$1"
TARGET_NAME="$2"
LIB_SO_EXT=".so"
LIB_A_EXT=".a"

cd $OUT_PATH

rm -rf $TARGET_NAME$LIB_SO_EXT.*
rm -rf $TARGET_NAME$LIB_A_EXT
