#!/bin/bash

PACKAGE_PATH=$(cd "$(dirname "$0")"; pwd)
SH_NAME=$(basename "$0")
SCRIPT_DIR=script

source $PACKAGE_PATH/$SCRIPT_DIR/env.sh

WITH_DGBUG_LIB=1
USE_JEMALLOC=
UDP_ENABLED=1
HTTP_ENABLED=1
SSL_ENABLED=1
ZLIB_ENABLED=1
ICONV_ENABLED=1
CC=g++

EXEC_FLAG=0
ACTION_NAME=

LIB_NAME_JEMALLOC=jemalloc_pic
LIB_NAME_SSL_1=ssl
LIB_NAME_SSL_2=crypto
LIB_NAME_ZLIB=z

LIB_FILE_JEMALLOC=lib$LIB_NAME_JEMALLOC.a
LIB_FILE_SSL_1=lib$LIB_NAME_SSL_1.a
LIB_FILE_SSL_2=lib$LIB_NAME_SSL_2.a
LIB_FILE_ZLIB=lib$LIB_NAME_ZLIB.a

CFG_RELEASE=Release
CFG_DEBUG=Debug

UDP_EXT_FILES=("ikcp.c")
HTTP_EXT_FILES=("http_parser.c")
HPSOCKET_LIB_EXCLUDE_FILES=(HPSocket4C.cpp HPSocket4C-SSL.cpp)
HPSOCKET4C_LIB_EXCLUDE_FILES=(HPSocket.cpp HPSocket-SSL.cpp)

print_usage()
{
	printf "Usage: %s [...O.P.T.I.O.N.S...]\n" "$SH_NAME"
	echo "----------------------+-------------------------------------------------"
	printf "  %-19s : %s\n" "-d|--with-debug-lib"	"compile debug libs (default: true)"
	printf "  %-19s : %s\n" "-j|--use-jemalloc"		"use jemalloc in release libs"
	printf "  %-19s : %s\n" ""						"(x86/x64 default: true, ARM default: false)"
	printf "  %-19s : %s\n" "-u|--udp-enabled"		"enable UDP components (default: true)"
	printf "  %-19s : %s\n" "-t|--http-enabled"		"enable HTTP components (default: true)"
	printf "  %-19s : %s\n" "-s|--ssl-enabled"		"enable SSL components (default: true)"
	printf "  %-19s : %s\n" "-z|--zlib-enabled"		"enable ZLIB related functions (default: true)"
	printf "  %-19s : %s\n" "-i|--iconv-enabled"	"enable ICONV related functions (default: true)"
	printf "  %-19s : %s\n" "-c|--compiler"			"compiler (default: g++)"
	printf "  %-19s : %s\n" "-p|--platform"			"platform: x86 / x64 / ARM"
	printf "  %-19s : %s\n" ""						"(default: current machine arch platform)"
	printf "  %-19s : %s\n" "-e|--clean"			"clean compilation intermediate temp files"
	printf "  %-19s : %s\n" "-r|--remove"			"remove all compilation target files"
	printf "  %-19s : %s\n" "-v|--version"			"print hp-socket version"
	printf "  %-19s : %s\n" "-h|--help"				"print this usage message"
	echo "----------------------+-------------------------------------------------"
}

print_config()
{
	printf "Action: %s\n" "$ACTION_NAME"
	echo "------------------+---------------------------------------"
	printf "%17s : v%d.%d.%d [BN:%02d]\n" "HP-Socket" $VER_MAJOR $VER_MINOR $VER_REVISE $VER_BUILD
	
	if [ $EXEC_FLAG -eq 0 ]; then
		printf "%17s : %s\n" "$ACTION_NAME path" "$PACKAGE_PATH"
		printf "%17s : %s\n" "--platform" 	"$PLATFORM"
		printf "%17s : %s\n" "--compiler"		"$CC"
		printf "%17s : %s\n" "--with-debug-lib"	"$(int_to_bool "$WITH_DGBUG_LIB")"
		printf "%17s : %s\n" "--use-jemalloc"	"$(int_to_bool "$USE_JEMALLOC")"
		printf "%17s : %s\n" "--udp-enabled"	"$(int_to_bool "$UDP_ENABLED")"
		printf "%17s : %s\n" "--http-enabled"	"$(int_to_bool "$HTTP_ENABLED")"
		printf "%17s : %s\n" "--ssl-enabled"	"$(int_to_bool "$SSL_ENABLED")"
		printf "%17s : %s\n" "--zlib-enabled"	"$(int_to_bool "$ZLIB_ENABLED")"
		printf "%17s : %s\n" "--iconv-enabled"	"$(int_to_bool "$ICONV_ENABLED")"
	else
		printf "%17s : %s\n" "$ACTION_NAME path" "$PACKAGE_PATH/$LIB_DIR"
	fi
	
	echo "------------------+---------------------------------------"

	read_confirm
}

parse_args()
{
	ARGS=$(getopt -o d:j:u:t:s:z:i:c:p:ervh -l with-debug-lib:,use-jemalloc:,udp-enabled:,http-enabled:,ssl-enabled:,zlib-enabled:,iconv-enabled:,compiler:platform:,clean,remove,version,help -n "$SH_NAME" -- "$@")
	RS=$?
	
	if [ $RS -ne 0 ]; then
		print_usage
		exit 2
	fi
	
	eval set -- "${ARGS}"
	
	local _SET_JEMALLOC="false"
	
	while true; do
		case "$1" in
			-d|--with-debug-lib)
				WITH_DGBUG_LIB=$(bool_to_int "$2")
				
				if [[ -z "$WITH_DGBUG_LIB" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-j|--use-jemalloc)
				USE_JEMALLOC=$(bool_to_int "$2")
				_SET_JEMALLOC="true"
				
				if [[ -z "$USE_JEMALLOC" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-u|--udp-enabled)
				UDP_ENABLED=$(bool_to_int "$2")
				
				if [[ -z "$UDP_ENABLED" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-t|--http-enabled)
				HTTP_ENABLED=$(bool_to_int "$2")
				
				if [[ -z "$HTTP_ENABLED" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-s|--ssl-enabled)
				SSL_ENABLED=$(bool_to_int "$2")
				
				if [[ -z "$SSL_ENABLED" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-z|--zlib-enabled)
				ZLIB_ENABLED=$(bool_to_int "$2")
				
				if [[ -z "$ZLIB_ENABLED" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-i|--iconv-enabled)
				ICONV_ENABLED=$(bool_to_int "$2")
				
				if [[ -z "$ICONV_ENABLED" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-c|--compiler)
				CC="$2"
				
				if [[ -z "$CC" ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi

				shift 2
				;;
			-p|--platform)
				PLATFORM="$2"
				
				if [[ -z "$PLATFORM" || ("$PLATFORM" != "x64" && "$PLATFORM" != "x86" && "$PLATFORM" != "ARM") ]]; then
					printf "Invalid arg value: %s %s\n" "$1" "$2"
					print_usage
					exit 2
				fi
				
				reset_env_args

				shift 2
				;;
			-e|--clean)
				EXEC_FLAG=1
				shift
				break
				;;
			-r|--remove)
				EXEC_FLAG=2
				shift
				break
				;;
			-v|--version)
				print_version
				exit 0
				;;
			-h|--help) 
				print_usage
				exit 0
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
	
	if [ $EXEC_FLAG -eq 1 ]; then
		ACTION_NAME="clean"
	elif [ $EXEC_FLAG -eq 2 ]; then
		ACTION_NAME="remove"
	else
		ACTION_NAME="compile"
	fi
	
	if [ $_SET_JEMALLOC != "true" ]; then
		USE_JEMALLOC=$([[ $PLATFORM == "ARM" ]] && echo 0 || echo 1)
	fi
}

do_build()
{
	C_LAN_OPTS="-c -x c -I $DEPT_INC_DIR -Wall -Wswitch -Wno-deprecated-declarations -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -fno-strict-aliasing -fpic -fvisibility=hidden -fexceptions -std=c11"
	CPP_LAN_OPTS="-c -x c++ -I $DEPT_INC_DIR -Wall -Wno-class-memaccess -Wno-reorder -Wswitch -Wno-deprecated-declarations -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -fno-strict-aliasing -fpic -fthreadsafe-statics -fvisibility=hidden -fexceptions -frtti -std=c++14"
	LINK_OPTS="-Wl,--no-undefined -Wl,-L$DEPT_LIB_DIR -L$DEPT_LIB_DIR -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -shared -Wl,-Bsymbolic"
	RELEASE_CFG_OPTS="-g0 -O3 -fomit-frame-pointer -DNDEBUG"
	DEBUG_CFG_OPTS="-g2 -gdwarf-2 -O0 -fno-omit-frame-pointer -DDEBUG -D_DEBUG"

	if [ -d $HPSOCKET_LIB_TARGET_DIR ]; then
		rm -rf $HPSOCKET_LIB_TARGET_DIR
	fi
	
	if [ -d $HPSOCKET4C_LIB_TARGET_DIR ]; then
		rm -rf $HPSOCKET4C_LIB_TARGET_DIR
	fi
	
	do_compile $HPSOCKET_LIB_NAME $CFG_RELEASE
	do_compile $HPSOCKET4C_LIB_NAME $CFG_RELEASE
	
	if [ $WITH_DGBUG_LIB -eq 1 ]; then
		do_compile $HPSOCKET_LIB_NAME $CFG_DEBUG
		do_compile $HPSOCKET4C_LIB_NAME $CFG_DEBUG
	fi

	update_hp_def
}

do_compile()
{
	_LIB_NAME=$1
	_CFG_NAME=$2
	
	parse_compile_args
	
	do_compile_step_1
	do_compile_step_2
	do_compile_step_3
}

parse_compile_args()
{
	_CL_OPTS=
	_LN_OPTS="-lrt -ldl -lpthread"
	_AR_FLAG=
	_CFG_OPTS=
	_EXCLUDE_FILES=
	_LIB_TARGET_DIR=
	_LIB_NAME_SUFFIX=
	
	if [ "$_LIB_NAME" == "$HPSOCKET_LIB_NAME" ]; then
		_EXCLUDE_FILES=(${HPSOCKET_LIB_EXCLUDE_FILES[@]})
		_LIB_TARGET_DIR=$HPSOCKET_LIB_TARGET_DIR
	else
		_EXCLUDE_FILES=(${HPSOCKET4C_LIB_EXCLUDE_FILES[@]})
		_LIB_TARGET_DIR=$HPSOCKET4C_LIB_TARGET_DIR
	fi
	
	if [ "$_CFG_NAME" == "$CFG_RELEASE" ]; then
		_CFG_OPTS=$RELEASE_CFG_OPTS
		_LIB_NAME_SUFFIX=
	else
		_CFG_OPTS=$DEBUG_CFG_OPTS
		_LIB_NAME_SUFFIX="_d"
	fi
	
	_AR_FLAG=0
	
	if [ $SSL_ENABLED -eq 1 ]; then
		_LN_OPTS="-l$LIB_NAME_SSL_1 -l$LIB_NAME_SSL_2 $_LN_OPTS"
		if [[ -f $DEPT_LIB_DIR/$LIB_FILE_SSL_1 && -f $DEPT_LIB_DIR/$LIB_FILE_SSL_2 ]]; then
			_AR_FLAG=$(($_AR_FLAG+1))
		fi
	else
		_CL_OPTS="-D_SSL_DISABLED $_CL_OPTS"
	fi
	
	if [ $ZLIB_ENABLED -eq 1 ]; then
		_LN_OPTS="-l$LIB_NAME_ZLIB $_LN_OPTS"
		if [ -f $DEPT_LIB_DIR/$LIB_FILE_ZLIB ]; then
			_AR_FLAG=$(($_AR_FLAG+2))
		fi
	else
		_CL_OPTS="-D_ZLIB_DISABLED $_CL_OPTS"
	fi
	
	if [[ $USE_JEMALLOC -eq 1 && $_CFG_NAME == $CFG_RELEASE ]]; then
		_LN_OPTS="-l$LIB_NAME_JEMALLOC $_LN_OPTS"
		if [ -f $DEPT_LIB_DIR/$LIB_FILE_JEMALLOC ]; then
			_AR_FLAG=$(($_AR_FLAG+4))
		fi
	fi
	
	if [ $UDP_ENABLED -eq 0 ]; then
		_CL_OPTS="-D_UDP_DISABLED $_CL_OPTS"
		_EXCLUDE_FILES=(${_EXCLUDE_FILES[@]} ${UDP_EXT_FILES[@]})
	fi
	
	if [ $HTTP_ENABLED -eq 0 ]; then
		_CL_OPTS="-D_HTTP_DISABLED $_CL_OPTS"
		_EXCLUDE_FILES=(${_EXCLUDE_FILES[@]} ${HTTP_EXT_FILES[@]})
	fi
	
	if [ $ICONV_ENABLED -eq 0 ]; then
		_CL_OPTS="-D_ICONV_DISABLED $_CL_OPTS"
	fi

	print_build_config
}

print_build_config()
{
	echo ">>>>>>>>>>>>>>>>>>>>> Starting Build <<<<<<<<<<<<<<<<<<<<<"
	
	echo "------------------+---------------------------------------"
	printf "%17s : %s\n" "_LIB_NAME"		"$_LIB_NAME"
	printf "%17s : %s\n" "_CFG_NAME"		"$_CFG_NAME"
	printf "%17s : %s\n" "PLATFORM"			"$PLATFORM"
	printf "%17s : %s\n" "_LIB_TARGET_DIR"	"$_LIB_TARGET_DIR"
	printf "%17s : %s\n" "_CL_OPTS"			"$_CL_OPTS"
	printf "%17s : %s\n" "_LN_OPTS"			"$_LN_OPTS"
	printf "%17s : %s\n" "_AR_FLAG"			"$_AR_FLAG"
	printf "%17s : %s\n" "_EXCLUDE_FILES"	"$(echo ${_EXCLUDE_FILES[@]})"
	echo "------------------+---------------------------------------"
}

do_compile_step_1()
{
	do_comepile_file $SRC_DIR
}

do_comepile_file()
{
	local _SRC_DIR=$1
	local _OBJ_TARGET_DIR=$_LIB_TARGET_DIR/$OBJ_DIR/$_CFG_NAME
	
	if [ ! -d $_OBJ_TARGET_DIR ]; then
		mkdir -p $_OBJ_TARGET_DIR
	fi
			
	for _FILE in $(ls $_SRC_DIR)
	do
		local _FULL_FILE_NAME=$_SRC_DIR/$_FILE
		
		if [ -d $_FULL_FILE_NAME ]; then
			do_comepile_file $_FULL_FILE_NAME
		elif [ -f $_FULL_FILE_NAME ]; then
			local _EXT=${_FILE##*.}
			
			if [[ "$_EXT" != "cpp" && "$_EXT" != "c" && "$_EXT" != "cc" ]]; then
				continue
			fi
			
			for _ITEM in ${_EXCLUDE_FILES[@]}
			do
				if [ "$_FILE" == "$_ITEM" ]; then
					#echo "- $_ITEM"
					continue 2
				fi
			done
			
			local _LAN_OPTS=
			local _OBJ_NAME=${_FILE%.*}.o
			
			if [ "$_EXT" == "cpp" ]; then
				_LAN_OPTS=$CPP_LAN_OPTS
			elif [[ "$_EXT" == "c" || "$_EXT" == "cc" ]]; then
				_LAN_OPTS=$C_LAN_OPTS 
			fi
			
			local _CMD="$CC $_LAN_OPTS $_FULL_FILE_NAME $_CFG_OPTS $_CL_OPTS -o $_OBJ_TARGET_DIR/$_OBJ_NAME"
			
			echo "> $_CMD"
			
			$_CMD
			
			if [ $? -ne 0 ]; then
				exit 3
			fi
		fi
	done
}

do_compile_step_2()
{
	local _LIB_FILE_NAME="lib$_LIB_NAME$_LIB_NAME_SUFFIX.so"
	local _SONAME_OPT="-Wl,-soname,$_LIB_FILE_NAME.$VER_MAJOR"
	local _OBJ_TARGET_DIR=$_LIB_TARGET_DIR/$OBJ_DIR/$_CFG_NAME
	local _OBJ_FILES=($(find $_OBJ_TARGET_DIR -name *.o | xargs ls))
	local _CMD="$CC -o $_LIB_TARGET_DIR/$_LIB_FILE_NAME $LINK_OPTS $_SONAME_OPT ${_OBJ_FILES[@]} $_LN_OPTS"
	
	echo "> $_CMD"
	
	$_CMD
	
	if [ $? -ne 0 ]; then
		exit 4
	fi
}

do_compile_step_3()
{
	local _LIB_FILE_NAME="lib$_LIB_NAME$_LIB_NAME_SUFFIX"
	local _LIB_PATH=$PACKAGE_PATH/$_LIB_TARGET_DIR
	local _CMD="$SCRIPT_DIR/post-link.sh $_AR_FLAG $_LIB_PATH $_LIB_FILE_NAME $PLATFORM $_CFG_NAME $VER_MAJOR $VER_MINOR $VER_REVISE"
	
	echo "> $_CMD"
	
	$_CMD
	
	if [ $? -ne 0 ]; then
		exit 5
	fi
}

update_hp_def()
{
	do_update_hp_def $UDP_ENABLED "_UDP_DISABLED"
	do_update_hp_def $SSL_ENABLED "_SSL_DISABLED"
	do_update_hp_def $HTTP_ENABLED "_HTTP_DISABLED"
	do_update_hp_def $ZLIB_ENABLED "_ZLIB_DISABLED"
	do_update_hp_def $ICONV_ENABLED "_ICONV_DISABLED"
}

do_update_hp_def()
{
	local _MACRO=$2
	
	if [ $1 -eq 1 ]; then
		sed -i "s/^#define $_MACRO/\/\/#define $_MACRO/" $HP_DEF_FILE_PATH 
	else
		sed -i "s/^\/\/#define $_MACRO/#define $_MACRO/" $HP_DEF_FILE_PATH 
	fi
}

do_clean()
{
	remove_file $LIB_DIR $OBJ_DIR
	remove_file $LIB_DIR "*.so.*"
}

do_remove()
{
	if [ -d $LIB_DIR ]; then
		rm -rf $LIB_DIR
	fi
}

parse_args "$@"
print_config

if [ $EXEC_FLAG -eq 1 ]; then
	do_clean
elif [ $EXEC_FLAG -eq 2 ]; then
	do_remove
else
	do_build
fi

echo "Done!"
exit 0
