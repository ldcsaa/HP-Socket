#!/bin/bash

PACKAGE_PATH=$(cd "$(dirname "$0")"; pwd)
SH_NAME=$(basename "$0")
SCRIPT_DIR=script

DEM_DIR=demo/Release
CER_DIR=demo/ssl-cert
DEM_FILE="hp-*.exe"
LIB_FILE_PREFIX="lib${HPSOCKET_LIB_NAME}"

DEFAULT_PREFIX="/usr/local"
PREFIX_PATH=
DEST_LIB_DIR=
DEST_BIN_DIR=bin
DEST_CER_DIR=hp-ssl-cert
DEST_INC_DIR=include
INSTALL_DEMO=0
IS_UNINSTALL=0
ACTION_NAME=

source $PACKAGE_PATH/$SCRIPT_DIR/env.sh

print_usage()
{
	printf "Usage: %s [...O.P.T.I.O.N.S...]\n" "$SH_NAME"
	echo "-----------------+-------------------------------------------------------"
	printf "  %-14s : %s\n" "-p|--prefix"	 "install/uninstall path (default: /usr/local)"
	printf "  %-14s : %s\n" "-l|--libdir"	 "lib dir (x86/arm default: 'lib', x64/arm64 default: 'lib64')"
	printf "  %-14s : %s\n" "-d|--with-demo" "install demos or not (default: false)"
	printf "  %-14s : %s\n" "-u|--uninstall" "execute uninstall operation from install path"
	printf "  %-14s : %s\n" "-v|--version"	 "print hp-socket version"
	printf "  %-14s : %s\n" "-h|--help"		 "print this usage message"
	echo "-----------------+-------------------------------------------------------"
}

print_config()
{
	printf "Action: %s\n" "$ACTION_NAME"
	echo "---------------+-------------------"
	printf "%14s : v%d.%d.%d [BN:%02d]\n" "HP-Socket" $VER_MAJOR $VER_MINOR $VER_REVISE $VER_BUILD
	printf "%14s : %s\n" "$ACTION_NAME path" "$PREFIX_PATH"
	printf "%14s : %s\n" "lib path" "$PREFIX_PATH/$DEST_LIB_DIR"
	printf "%14s : %s\n" "arch platform" "$PLATFORM"
	
	if [ $IS_UNINSTALL -eq 0 ]; then
		printf "%14s : %s\n" "install demos" $(int_to_bool "$INSTALL_DEMO")
	fi
	
	echo "---------------+-------------------"

	read_confirm
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
				INSTALL_DEMO=$(bool_to_int $2)
				
				if [ -z $INSTALL_DEMO ]; then
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
	
	if [[ ${PREFIX_PATH:0-1:1} == '/' ]]; then
		PREFIX_PATH=${PREFIX_PATH%?}
	fi
	
	if [ -z "$PREFIX_PATH" ]; then
		PREFIX_PATH=$DEFAULT_PREFIX
	fi
	
	if [ -z "$DEST_LIB_DIR" ]; then
		if [[ $PLATFORM == "x64" || $PLATFORM == "arm64" ]]; then
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
	
	if [ $IS_UNINSTALL -eq 0 ]; then
		ACT="create"
	else
		ACT="remove"
	fi
	
	printf "  > %s %s\n" "$ACT" "$_INSTALL_FILE_"
}

cp_lib_a()
{
	for FILE in $1/${LIB_FILE_PREFIX}*.a; do
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
	
	for FILE in $1/${LIB_FILE_PREFIX}*.so; do
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

cp_bin_cert()
{
	for FILE in $CER_DIR/*; do
		if [ -f "$FILE" ]; then
			set_install_file $PREFIX_PATH/$DEST_BIN_DIR/$DEST_CER_DIR/$(basename $FILE)
			cp -f $FILE $_INSTALL_FILE_
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
	cp_lib_a $HPSOCKET_LIB_TARGET_DIR
	cp_lib_a $HPSOCKET4C_LIB_TARGET_DIR
	
	# copy *.so
	cp_lib_so $HPSOCKET_LIB_TARGET_DIR
	cp_lib_so $HPSOCKET4C_LIB_TARGET_DIR
	
	mkdir -p $PREFIX_PATH/$DEST_INC_DIR
	
	# copy include dir
	cp_inc $INC_DIR $PREFIX_PATH/$DEST_INC_DIR
	
	if [[ $INSTALL_DEMO -eq 1 && -d $DEM_DIR/$PLATFORM && "$(ls -A $DEM_DIR/$PLATFORM)" != "" ]]; then
		# copy demo .exe files
		mkdir -p $PREFIX_PATH/$DEST_BIN_DIR
		cp_bin_exe
		
		# copy demo ssl cert files
		mkdir -p $PREFIX_PATH/$DEST_BIN_DIR/$DEST_CER_DIR
		cp_bin_cert
	fi
}

do_uninstall()
{
	local FILE_PATH=$PREFIX_PATH/$DEST_LIB_DIR
	
	if [ -d "$FILE_PATH" ]; then
		# rm *.a
		set_install_file "$FILE_PATH/${LIB_FILE_PREFIX}*.a"
		rm -rf $_INSTALL_FILE_
		# rm *.so
		set_install_file "$FILE_PATH/${LIB_FILE_PREFIX}*.so"
		rm -rf $_INSTALL_FILE_
		set_install_file "$FILE_PATH/${LIB_FILE_PREFIX}*.so.*"
		rm -rf $_INSTALL_FILE_
		
		rm_empty_dir $FILE_PATH
	fi
	
	FILE_PATH=$PREFIX_PATH/$DEST_INC_DIR
	
	if [ -d "$FILE_PATH" ]; then
		# rm include
		set_install_file "$FILE_PATH/$HPSOCKET_LIB_NAME"
		rm -rf $_INSTALL_FILE_
		
		rm_empty_dir $FILE_PATH
	fi
	
	FILE_PATH=$PREFIX_PATH/$DEST_BIN_DIR
	
	if [ -d "$FILE_PATH" ]; then
		# rm demo cert
		set_install_file "$FILE_PATH/$DEST_CER_DIR"
		rm -rf $_INSTALL_FILE_
		
		# rm demo
		set_install_file "$FILE_PATH/$DEM_FILE"
		rm -rf $_INSTALL_FILE_
		set_install_file "$FILE_PATH/$DEM_FILE.*"
		rm -rf $_INSTALL_FILE_
		
		rm_empty_dir $PREFIX_PATH/$DEST_BIN_DIR
	fi
	
	rm_empty_dir $PREFIX_PATH
}

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
