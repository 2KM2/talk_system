#!/bin/bash

build(){
    echo "build"
    if [ ! -d lib ]
    then
        mkdir lib
    fi
    
    mkdir -p ./common/build
	cd  ./common/build
    cmake ..
    make 
    if [ $? -eq 0 ]; then
        echo "make base successed";
    else
        echo "make base failed";
        exit;
    fi

	make -p ./run/lib
	make -p ./run/etc
	make -p ./run/login_server
	make -p ./run/route_server
	make -p ./run/msg_server
}


clean(){
    echo "clean"
	cd common
	make clean
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
	version)
		if [ $# != 2 ]; then 
			echo $#
			print_help
			exit
		fi

		echo $2
		echo "build..."
		build $2
		;;
	*)
		print_help
		;;
esac