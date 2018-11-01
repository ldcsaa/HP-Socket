#!/bin/bash

AR_FLAG="$1"
OUT_PATH="$2"
OUT_PATH="${OUT_PATH/%\//}"
TARGET_NAME="$3"
LIB_SO_EXT=".so"
LIB_A_EXT=".a"
TARGET_SO_NAME=$TARGET_NAME$LIB_SO_EXT
TARGET_A_NAME=$TARGET_NAME$LIB_A_EXT
PLATFORM="$4"
CONFIGURATION="$5"
VER_MAJOR="$6"
VER_MINOR="$7"
VER_REVISE="$8"
INTERMEDIATE_DIR="obj/$CONFIGURATION"
OBJ_TEMP_DIR="obj/$CONFIGURATION/static-tmp"
DEPENDENT_PATH="$OUT_PATH/../../../dependent/$PLATFORM/lib"
SCRIPT_PATH=$(dirname $0)

add_dependent()
{
	local FILE_NAME="lib$1.a"
	
	rm -rf $OBJ_TEMP_DIR/*
	
	cd $OBJ_TEMP_DIR
	ar -x $DEPENDENT_PATH/$FILE_NAME
	cd $OUT_PATH
	
	ar -csr $TARGET_A_NAME $OBJ_TEMP_DIR/*.o
}

$SCRIPT_PATH/pre-link.sh $OUT_PATH $TARGET_NAME

cd $OUT_PATH

[ -f "$TARGET_SO_NAME" ] || exit 1
ln -snf $TARGET_SO_NAME $TARGET_SO_NAME.$VER_MAJOR
ln -snf $TARGET_SO_NAME $TARGET_SO_NAME.$VER_MAJOR.$VER_MINOR.$VER_REVISE

[[ -f "$TARGET_SO_NAME.$VER_MAJOR" &&  -d "$INTERMEDIATE_DIR" && $(ls -l $INTERMEDIATE_DIR/*.o | grep '^-' | wc -l) -gt 0 ]] || exit 2

mkdir -p $OBJ_TEMP_DIR

add_dependent jemalloc_pic

if [ $(($AR_FLAG & 1)) -gt 0 ]; then
	add_dependent crypto
	add_dependent ssl
fi

if [ $(($AR_FLAG & 2)) -gt 0 ]; then
	add_dependent z
	#add_dependent http_parser
fi

rm -rf $OBJ_TEMP_DIR

ar -csr $TARGET_A_NAME $INTERMEDIATE_DIR/*.o

exit 0