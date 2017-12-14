#!/bin/bash

PRJ_DIR=project
DEM_DIR=demo/Release
DEM_FILE="hp-*.exe"
LIB_DIR=lib
HPSOCKET_LIB_NAME=hpsocket
HPSOCKET4C_LIB_NAME=hpsocket4c
HPSOCKET_LIB_DIR=$LIB_DIR/$HPSOCKET_LIB_NAME
HPSOCKET4C_LIB_DIR=$LIB_DIR/$HPSOCKET4C_LIB_NAME
LIB_FILE_PREFIX="lib${HPSOCKET_LIB_NAME}"
INC_DIR=include
PACKAGE_PATH=$(dirname "$0")
SH_NAME=$(basename "$0")

PLATFORM=
VER_MAJOR=
VER_MINOR=
VER_REVISE=
VER_BUILD=

PREFIX_PATH=
DEST_LIB_DIR=
DEST_BIN_DIR=bin
DEST_INC_DIR=include
INSTALL_DEMO=1
IS_UNINSTALL=0
ACTION_NAME=

print_usage()
{
	printf "Usage: %s [...O.P.T.I.O.N.S...]\n" "$SH_NAME"
	echo "-----------------+------------------------------------------------"
	printf "  %-14s : %s\n" "-p|--prefix"	 "install/uninstall path (default: /usr/local)"
	printf "  %-14s : %s\n" "-l|--libdir"	 "lib dir (default: x86 -> lib, x64 -> lib64)"
	printf "  %-14s : %s\n" "-d|--with-demo" "install demos or not (default: true)"
	printf "  %-14s : %s\n" "-u|--uninstall" "execute uninstall operation from install path"
	printf "  %-14s : %s\n" "-v|--version"	 "print installing hp-socket version"
	printf "  %-14s : %s\n" "-h|--help"		 "print this usage message"
	echo "-----------------+------------------------------------------------"
}

print_version()
{
	printf "HP-Socket for Linux v%d.%d.%d [BN:%02d]\n" $VER_MAJOR $VER_MINOR $VER_REVISE $VER_BUILD
}

print_config()
{
	printf "Action: %s\n" "$ACTION_NAME"
	echo "-----------------------------------"
	if [ $IS_UNINSTALL -eq 0 ]; then
		printf "%14s : v%d.%d.%d [BN:%02d]\n" "HP-Socket" $VER_MAJOR $VER_MINOR $VER_REVISE $VER_BUILD
	fi

	printf "%14s : %s\n" "$ACTION_NAME path" "$PREFIX_PATH"
	printf "%14s : %s\n" "lib path" "$PREFIX_PATH/$DEST_LIB_DIR"
	
	if [ $IS_UNINSTALL -eq 0 ]; then
		printf "%14s : %s\n" "arch Platform" "$PLATFORM"
		printf "%14s : %s\n" "install demos" "$([[ $INSTALL_DEMO -eq 1 ]] && echo 'true' || echo 'false')"
	fi
	
	echo "-----------------------------------"

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

parse_version()
{
	local VERSION=$(cat project/Version.props | grep --colour=never -E "<$1>\w+</$1>" | cut -d ">" -f 2 | cut -d "<" -f 1)
	
	if [ -z $VERSION ]; then
		printf "Can not fetch HP-Socket version: %s\n" $1
		exit 1
	fi

	echo $VERSION
}

check_platform()
{
	PLATFORM=
	
	if [ "$1" == "x86_64" ]; then
		PLATFORM="x64"
	elif [[ "$1" == "i686" || "$1" == "i386" ]]; then
		PLATFORM="x86"
	elif [[ "$1" =~ "arm" ]]; then
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

	VER_MAJOR=$(parse_version "VER_MAJOR")
	VER_MINOR=$(parse_version "VER_MINOR")
	VER_REVISE=$(parse_version "VER_REVISE")
	VER_BUILD=$(parse_version "VER_BUILD")
}

parse_args()
{
	ARGS=$(getopt -o hvup:l:d: -l help,version,uninstall,prefix:,libdir:,with-demo: -n "$SH_NAME" -- "$@")
	RS=$?
	
	if [ $RS -ne 0 ]; then
		print_usage
		exit 2
	fi
	
	eval set -- "${ARGS}"
	
	while true; do
		case "$1" in
			-h|--help) 
				print_usage
				exit 0
				;;
			-v|--version)
				print_version
				exit 0
				;;
			-u|--uninstall)
				IS_UNINSTALL=1
				shift
				;;
			-p|--prefix)
				PREFIX_PATH="$2"
				shift 2
				;;
			-l|--libdir)
				DEST_LIB_DIR="$2"
				shift 2
				;;
			-d|--with-demo)
				if [ "$2" == "true" ]; then
					INSTALL_DEMO=1
				elif [ "$2" == "false" ]; then
					INSTALL_DEMO=0
				else
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi
				
				shift 2
				;;
			--)
				shift
				break
				;;
			*)
				printf "Parse args fail\n"
				print_usage
				exit 2
				;;
		esac
	done
	
	if [ -z "$PREFIX_PATH" ]; then
		PREFIX_PATH="/usr/local"
	fi
	
	if [ -z "$DEST_LIB_DIR" ]; then
		if [ $PLATFORM == "x64" ]; then
			DEST_LIB_DIR="lib64"
		else
			DEST_LIB_DIR="lib"
		fi
	fi
	
	if [ $IS_UNINSTALL -eq 0 ]; then
		ACTION_NAME="install"
	else
		ACTION_NAME="uninstall"
	fi
}

_INSTALL_FILE_=
set_install_file()
{
	_INSTALL_FILE_="$1"
	printf "  > %s\n" "$_INSTALL_FILE_"
}

cp_lib_a()
{
	for FILE in $1/$PLATFORM/${LIB_FILE_PREFIX}*.a; do
		if [ -f "$FILE" ]; then
			set_install_file $PREFIX_PATH/$DEST_LIB_DIR/$(basename $FILE)
			cp -f $FILE $_INSTALL_FILE_
			chmod -f +x $_INSTALL_FILE_
		fi
	done
}

cp_lib_so()
{
	local DEST_LIB_PATH=$PREFIX_PATH/$DEST_LIB_DIR
	
	for FILE in $1/$PLATFORM/${LIB_FILE_PREFIX}*.so; do
		if [ -f "$FILE" ]; then
			FILE_NAME=$(basename $FILE)
			set_install_file $DEST_LIB_PATH/$FILE_NAME.$VER_MAJOR.$VER_MINOR.$VER_REVISE
			cp -f $FILE $_INSTALL_FILE_
			chmod -f +x $_INSTALL_FILE_
			set_install_file $DEST_LIB_PATH/$FILE_NAME.$VER_MAJOR
			ln -snf $FILE_NAME.$VER_MAJOR.$VER_MINOR.$VER_REVISE $_INSTALL_FILE_
			set_install_file $DEST_LIB_PATH/$FILE_NAME
			ln -snf $FILE_NAME.$VER_MAJOR $_INSTALL_FILE_
		fi
	done
}

cp_inc()
{
	local SRC=$1
	local DEST=$2
	
	for FILE in $SRC/*; do
		FILE_NAME=$(basename $FILE)
		if [ -d "$FILE" ]; then
			mkdir -p $DEST/$FILE_NAME
			cp_inc $SRC/$FILE_NAME $DEST/$FILE_NAME
		elif [ -f "$FILE" ]; then
			set_install_file $DEST/$FILE_NAME
			cp -f $FILE $_INSTALL_FILE_
		fi
	done
}

cp_bin_exe()
{
	for FILE in $DEM_DIR/$PLATFORM/$DEM_FILE; do
		if [ -f "$FILE" ]; then
			set_install_file $PREFIX_PATH/$DEST_BIN_DIR/$(basename $FILE)
			cp -f $FILE $_INSTALL_FILE_
			chmod -f +x $_INSTALL_FILE_
		fi
	done
}

rm_empty_dir()
{
	if [[ -d "$1" && -z $(ls -A "$1") ]]; then
		set_install_file "$1"
		rm -rf $_INSTALL_FILE_
	fi
}

do_install()
{
	mkdir -p $PREFIX_PATH/$DEST_LIB_DIR
	
	# copy *.a
	cp_lib_a $HPSOCKET_LIB_DIR
	cp_lib_a $HPSOCKET4C_LIB_DIR
	
	# copy *.so
	cp_lib_so $HPSOCKET_LIB_DIR
	cp_lib_so $HPSOCKET4C_LIB_DIR
	
	mkdir -p $PREFIX_PATH/$DEST_INC_DIR
	
	# copy include dir
	cp_inc $INC_DIR $PREFIX_PATH/$DEST_INC_DIR
	
	if [ $INSTALL_DEMO -eq 1 ]; then
		mkdir -p $PREFIX_PATH/$DEST_BIN_DIR
		
		# copy demo .exe files
		cp_bin_exe
	fi
}

do_uninstall()
{
	# rm *.a
	set_install_file "$PREFIX_PATH/$DEST_LIB_DIR/${LIB_FILE_PREFIX}*.a"
	rm -rf $_INSTALL_FILE_
	# rm *.so
	set_install_file "$PREFIX_PATH/$DEST_LIB_DIR/${LIB_FILE_PREFIX}*.so"
	rm -rf $_INSTALL_FILE_
	set_install_file "$PREFIX_PATH/$DEST_LIB_DIR/${LIB_FILE_PREFIX}*.so.*"
	rm -rf $_INSTALL_FILE_
	
	# rm include
	set_install_file "$PREFIX_PATH/$DEST_INC_DIR/$HPSOCKET_LIB_NAME"
	rm -rf $_INSTALL_FILE_
	
	# rm demo
	set_install_file "$PREFIX_PATH/$DEST_BIN_DIR/$DEM_FILE"
	rm -rf $_INSTALL_FILE_
	
	# rm empty dir
	rm_empty_dir $PREFIX_PATH/$DEST_LIB_DIR
	rm_empty_dir $PREFIX_PATH/$DEST_INC_DIR
	rm_empty_dir $PREFIX_PATH/$DEST_BIN_DIR
	rm_empty_dir $PREFIX_PATH
}

cd $PACKAGE_PATH

parse_envs
parse_args "$@"
print_config

if [ $IS_UNINSTALL -eq 0 ]; then
	do_install
else
	do_uninstall
fi

ldconfig > /dev/null 2>&1

echo "Done!"
exit 0