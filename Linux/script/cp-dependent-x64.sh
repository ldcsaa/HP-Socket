#!/bin/bash

SRC_BASE=/usr/local
DEST_BASE=$(dirname "$0")/../dependent
TARGET=x64

do_copy()
{
  mkdir -p $TARGET/lib $TARGET/include

  cp -rf /usr/local/$1/include/* $TARGET/include
  cp -rf /usr/local/$1/lib/*.a $TARGET/lib
}

cd $DEST_BASE
rm -rf $TARGET

do_copy http-parser
do_copy jemalloc
do_copy openssl
do_copy zlib

