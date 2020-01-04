#!/bin/bash

PACKAGE_PATH=${PACKAGE_PATH:-$(cd "$(dirname "$0")"; pwd)}  

LIB_DIR=lib
OBJ_DIR=obj
SRC_DIR=src
INC_DIR=include
DEPT_DIR=dependent

PROJECT_NAME=hpsocket
HP_DEF_FILE_NAME=HPTypeDef.h
HPSOCKET_LIB_NAME=hpsocket
HPSOCKET4C_LIB_NAME=hpsocket4c
HPSOCKET_LIB_DIR=$LIB_DIR/$HPSOCKET_LIB_NAME
HPSOCKET4C_LIB_DIR=$LIB_DIR/$HPSOCKET4C_LIB_NAME

HP_DEF_FILE_PATH=$INC_DIR/$PROJECT_NAME/$HP_DEF_FILE_NAME

VER_MAJOR_MICRO=HP_VERSION_MAJOR
VER_MINOR_MICRO=HP_VERSION_MINOR
VER_REVISE_MICRO=HP_VERSION_REVISE
VER_BUILD_MICRO=HP_VERSION_BUILD

PLATFORM=
VER_MAJOR=
VER_MINOR=
VER_REVISE=
VER_BUILD=

DEPT_INC_DIR=
DEPT_LIB_DIR=
HPSOCKET_LIB_TARGET_DIR=
HPSOCKET4C_LIB_TARGET_DIR=

read_confirm()
{
	while true; do
		read -p "Are you sure? [Y/n]: " ANSWER
		
		if [[ -z $ANSWER || $ANSWER == 'y' || $ANSWER == 'Y' ]]; then
			return 0
		elif [[ $ANSWER == 'n' || $ANSWER == 'N' ]]; then
			echo "bye ~ bye ~"
			exit 0
		fi
	done
}

print_version()
{
	printf "HP-Socket for Linux v%d.%d.%d [BN:%02d]\n" $VER_MAJOR $VER_MINOR $VER_REVISE $VER_BUILD
}

parse_version()
{
	echo $(cat $HP_DEF_FILE_PATH | grep --colour=never "#define" | grep --colour=never "$1" | awk '{print $3}')
}

check_platform()
{
	PLATFORM=
	
	if [ "$1" == "x86_64" ]; then
		PLATFORM="x64"
	elif [[ "$1" == "x86" || "$1" == "i686" || "$1" == "i586" || "$1" == "i386" ]]; then
		PLATFORM="x86"
	elif [[ "$1" =~ "arm" || "$1" =~ "aarch" ]]; then
		PLATFORM="ARM"
	fi
}

parse_envs()
{
	local OS_NAME=$(uname -s | tr "[A-Z]" "[a-z]")

	if [ "$OS_NAME" != "linux" ]; then
		printf "OS Platform not supported: %s\n" $OS_NAME
		exit 1
	fi
	
	check_platform "$(arch | tr "[A-Z]" "[a-z]")"
	
	if [ -z $PLATFORM ]; then
	
		check_platform "$(uname -p | tr "[A-Z]" "[a-z]")"
		
		if [ -z $PLATFORM ]; then
			printf "Can not determine Arch Platform\n"
			exit 1
		fi
	fi
	
	if [ ! -f $HP_DEF_FILE_PATH ]; then
		printf "HP-Socket version file not exists: '%s'\n" $HP_DEF_FILE_PATH
		exit 1
	fi

	VER_MAJOR=$(parse_version "$VER_MAJOR_MICRO")
	VER_MINOR=$(parse_version "$VER_MINOR_MICRO")
	VER_REVISE=$(parse_version "$VER_REVISE_MICRO")
	VER_BUILD=$(parse_version "$VER_BUILD_MICRO")
	
	reset_env_args
}

reset_env_args()
{
	HPSOCKET_LIB_TARGET_DIR=$HPSOCKET_LIB_DIR/$PLATFORM
	HPSOCKET4C_LIB_TARGET_DIR=$HPSOCKET4C_LIB_DIR/$PLATFORM

	DEPT_INC_DIR=$DEPT_DIR/$PLATFORM/$INC_DIR
	DEPT_LIB_DIR=$DEPT_DIR/$PLATFORM/$LIB_DIR
}

remove_file()
{
  find "$1" -name "$2" | xargs rm -rf
}

int_to_bool()
{
	[[ $1 -eq 0 ]] && echo 'false' || echo 'true'
}

bool_to_int()
{
	if [ "$1" == "true" ]; then
		 echo "1"
	elif [ "$1" == "false" ]; then
		echo "0"
	else
		echo ""
	fi
}

cd $PACKAGE_PATH

parse_envs
