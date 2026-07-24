#!/bin/bash

VENV_NAME=.artifact_venv
VENV_PATH=$PWD/$VENV_NAME
TEMP_PATH=$PWD/.tmp

function check_command() {
	if ! command -v $1 &>/dev/null; then
		echo "Command $1 does not exist; please install before running script"
		return 1
	fi
}

function check_boost() {
	# if ! dpkg -s libboost-dev | grep -q "Version"; then
	# 	echo "Please install libboost"
	# 	return 1
	# fi
	return 0
}

function check_os() {
	local current_os
	case "$OSTYPE" in
	linux*) current_os="linux" ;;
	darwin*) current_os="macos" ;;
	*) current_os="other" ;;
	esac

	if [ "$current_os" == "other" ]; then
		echo "This script only supports macos or linux; provided OS: $current_os"
		return 1
	fi

	if [[ "$current_os" == "linux" ]]; then
		MSP_LIB=libmsp430.so
		MSP_BOOST_DIR=/usr/lib/x86_64-linux-gnu/
	else
		MSP_LIB=libmsp430.dylib
		MSP_BOOST_DIR=$(brew --prefix boost@1.85)
	fi

	# Define a read-only global variable
	readonly DETECTED_OS="$current_os"
}

function install_libusb() {
	if [ -f "$VENV_PATH/include/usb.h" ]; then
		echo "libusb already installed!"
		return 0
	fi

	pushd $TEMP_PATH

	git clone https://github.com/libusb/libusb.git
	pushd libusb
	./autogen.sh
	./autogen.sh
	./configure --prefix=$VENV_PATH
	make
	make install
	popd

	export PKG_CONFIG_PATH=$VENV_PATH/lib/pkgconfig/
	git clone https://github.com/libusb/libusb-compat-0.1.git
	pushd libusb-compat-0.1
	./autogen.sh
	./autogen.sh
	./configure --prefix=$VENV_PATH
	make
	make install
	popd

	popd
}

function install_hidapi() {
	if [ -d "$VENV_PATH/include/hidapi" ]; then
		echo "hidapi already installed!"
		return 0
	fi

	pushd $TEMP_PATH

	git clone https://github.com/libusb/hidapi.git
	mkdir hidapi_build
	pushd hidapi_build
	cmake ../hidapi/ -DCMAKE_INSTALL_PREFIX=$VENV_PATH
	cmake --build .
	cmake --build . --target install
	popd

	popd
}

function install_mspgcc() {
	if [ -f "$VENV_PATH/bin/msp430-elf-gcc" ]; then
		echo "mspgcc already installed!"
		return 0
	fi

	if [[ "$current_os" == "linux" ]]; then
		GCC_URL=https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-9.3.1.11_linux64.tar.bz2
		GCC_DIRNAME=msp430-gcc-9.3.1.11_linux64
	else
		GCC_URL=https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-9.3.1.11_macos.tar.bz2
		GCC_DIRNAME=msp430-gcc-9.3.1.11_macos
	fi
	GCC_SUP_URL=https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-support-files-1.212.zip

	pushd $TEMP_PATH

	wget --quiet $GCC_URL
	wget --quiet $GCC_SUP_URL

	tar xjf "$GCC_DIRNAME.tar.bz2"
	unzip -qx msp430-gcc-support-files-1.212.zip
	mv msp430-gcc-support-files/include/* $GCC_DIRNAME/include

	mkdir -p $VENV_PATH/ti/mspgcc
	mv $GCC_DIRNAME/* $VENV_PATH/ti/mspgcc

	popd

	pushd $VENV_PATH/ti/mspgcc
	stow -t $VENV_PATH/bin bin

	popd
}

function install_mspds() {

	if [ -f $VENV_PATH/lib/$MSP_LIB ]; then
		echo "mspds already installed!"
		return 0
	fi

	pushd $TEMP_PATH

	wget --quiet "https://dr-download.ti.com/software-development/driver-or-library/MD-4vnqcP1Wk4/3.15.1.1/MSPDebugStack_OS_Package_3_15_1_1.zip"
	unzip -q "MSPDebugStack_OS_Package_3_15_1_1.zip" -d mspds

	pushd mspds

	cp ../../mspds_makefile ./Makefile

	HID_PATH=$VENV_PATH/include/hidapi/hidapi.h
	cp $HID_PATH ./ThirdParty/include/

	if [[ "$1" == "linux" ]]; then
		make --silent BOOST_DIR=$MSP_BOOST_DIR VENV_PATH=$VENV_PATH
	else
		make --silent BOOST_DIR=$MSP_BOOST_DIR VENV_PATH=$VENV_PATH
	fi
	make --silent PREFIX=$VENV_PATH VENV_PATH=$VENV_PATH install

	popd
	popd
}

function install_mspdebug_unlinked() {
	if [ -f $VENV_PATH/bin/mspdebug_unlinked ]; then
		echo "mspdebug unlinked installed!"
		return 0
	fi

	pushd $TEMP_PATH

	git clone https://github.com/dlbeer/mspdebug
	pushd mspdebug

	export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$VENV_PATH/lib/pkgconfig
	echo $PKG_CONFIG_PATH

	make PREFIX=$VENV_PATH install
	mv $VENV_PATH/bin/mspdebug $VENV_PATH/bin/mspdebug_unlinked

	popd
	popd
}

function install_mspdebug() {
	if [ -f $VENV_PATH/bin/mspdebug ]; then
		echo "mspdebug installed!"
		return 0
	fi

	mspdebug=$VENV_PATH/bin/mspdebug
	touch $mspdebug
	echo "#!/bin/bash" >$mspdebug
	if [[ "$1" == "linux" ]]; then
		echo "LD_LIBRARY_PATH=\"$VENV_PATH/lib\" mspdebug_unlinked \"\$@\"" >>$mspdebug
	else
		echo "DYLD_LIBRARY_PATH=$VENV_PATH/lib mspdebug_unlinked \"\$@\"" >>$mspdebug
	fi
	chmod +x $mspdebug

}

echo "---------Checking Requirements---------"
check_command uv || exit 1
check_boost || exit 1
check_os || exit 1
echo "Pass"
echo "----------------------"

echo "---------VENV Creation---------"
uv venv $VENV_NAME
source $VENV_NAME/bin/activate
echo "----------------------"

echo "---------Python requirements---------"
uv pip install -r requirements.txt
echo "----------------------"

mkdir $TEMP_PATH
echo "---------Installing Dependencies---------"
install_libusb || exit 1
install_hidapi || exit 1
echo "----------------------"

echo "---------Installing mspgcc---------"
install_mspgcc || exit 1
echo "----------------------"

echo "---------Installing msp debug stack---------"
install_mspds || exit 1
echo "----------------------"

echo "---------Installing mspdebug---------"
install_mspdebug_unlinked || exit 1
install_mspdebug
echo "----------------------"

rm -rf $TEMP_PATH
rm -rf ./ltmain.sh

