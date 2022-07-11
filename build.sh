#!/bin/bash
CURRENT_DIR=$(dirname $(readlink -f "$0"))
build(){
    echo "build"
    if [ ! -d lib ]
    then
        mkdir lib
    fi

	echo $CURRENT_DIR

    mkdir -p ./common/build
	cd  ./common/build
    cmake .. && make
    if [ $? -eq 0 ]; then
        echo "make common successed";
    else
        echo "make common failed";
        exit;
    fi
	cd $CURRENT_DIR #重新回到当前路径
	mkdir -p ./run/lib
	mkdir -p ./run/etc
	mkdir -p ./run/login_server
	mkdir -p ./run/route_server
	mkdir -p ./run/msg_server
}


clean(){
    echo "clean"
	cd $CURRENT_DIR/common/build
	make clean
	rm -rf $CURRENT_DIR/common/build
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