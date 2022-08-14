#!/bin/bash
CURRENT_DIR=$(dirname $(readlink -f "$0"))
build(){
	if [ ! -d build ]
	then
		mkdir build
	fi

	if [ ! -d release ]
	then
		mkdir -p   release/bin
		mkdir -p   release/lib
		mkdir  -p  release/etc
	fi
	export LD_LIBRAY_PATH=${CURRENT_DIR}/release/lib;$LD_LIBRAY_PATH
	cd build
	cmake ..
	make
}
# LD_LIBRAY_PATH

clean(){
    echo "clean"
	rm -rf build
	rm -rf release

}

print_help() {
	echo "Usage: "
	echo "  $0 clean --- clean all build"
	echo "  $0 version version_str --- build a version"
}
case $1 in
	clean)
		echo "clean all build..."
		clean
		;;
	build)
		#if [ $# != 2 ]; then 
		#	echo $#
		#	print_help
		#	exit
		#fi
		build

		#echo $2
		#echo "build..."
		#build $2
		;;
	*)
		print_help
		;;
esac