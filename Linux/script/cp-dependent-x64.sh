#!/bin/bash

SRC_BASE=/usr/local
DEST_BASE=$(cd "$(dirname "$0")"; pwd)/../dependent
TARGET=x64

do_copy()
{
  mkdir -p $TARGET/lib $TARGET/include

  cp -rf /usr/local/$1/include/* $TARGET/include
  cp -rf /usr/local/$1/lib/*.a $TARGET/lib
}

cd $DEST_BASE
rm -rf $TARGET

do_copy jemalloc
do_copy openssl
do_copy zlib

