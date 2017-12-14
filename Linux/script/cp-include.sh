#!/bin/bash

INC_DIR=include/hpsocket
INC_PATH=$(dirname "$0")/../$INC_DIR
SRC_DIR=src
COMM_DIR=common

mkdir -p $INC_PATH/$COMM_DIR

cd $INC_PATH

cp -f ../../$SRC_DIR/HPSocket.h .
cp -f ../../$SRC_DIR/HPSocket4C.h .
cp -f ../../$SRC_DIR/HPTypeDef.h .
cp -f ../../$SRC_DIR/SocketInterface.h .

cp -f ../../$SRC_DIR/$COMM_DIR/GlobalDef.h $COMM_DIR
cp -f ../../$SRC_DIR/$COMM_DIR/GlobalErrno.h $COMM_DIR
