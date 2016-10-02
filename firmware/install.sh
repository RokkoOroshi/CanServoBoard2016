#!/usr/bin/env sh
cd `dirname $0`

function install(){
    src=$1
    dest=$2
    if [ ! -e $dest ]; then
	ln -s $src $dest
	echo "OK : symbolic link ${dest} created."
    else
	echo "Failed : ${dest} already exist!"
    fi
}

if [ "$(uname)" == 'Darwin' ]; then
    echo "OS is Mac"
    arduino="/Applications/Arduino.app/Contents/Java"

elif [ "$(expr substr $(uname -s) 1 5)" == 'Linux' ]; then
    echo "OS is Linux"
    echo "please write install script by your self."
    #arduinoを適切なパスに設定すればいけると思う
    exit 1
else
    echo "Your platform ($(uname -a)) is not supported."
    exit 1
fi

if [ -d $arduino ]; then
    # hardware
    src="`pwd`/RokkoOroshiCanServoBoard/"
    dest="${arduino}/hardware/RokkoOroshiCanServoBoard"
    install $src $dest
    
    #example
    src="`pwd`/examples/"
    dest="${arduino}/examples/RokkoOroshiCanServoBoard"
    install $src $dest
    
    #external library
    src="`pwd`/external_library/CAN_BUS_Shield/"
    dest="${arduino}/libraries/CAN_BUS_Shield"
    install $src $dest
    
    src="`pwd`/external_library/Arduino-SerialCommand/"
    dest="${arduino}/libraries/Arduino-SerialCommand"
    install $src $dest
    
    src="`pwd`/external_library/MsTimer2/"
    dest="${arduino}/libraries/MsTimer2"
    install $src $dest

    src="`pwd`/external_library/Adafruit-PWM-Servo-Driver-Library/"
    dest="${arduino}/libraries/Adafruit-PWM-Servo-Driver-Library"
    install $src $dest
    
else
    echo "Arduino IDE is not found under /Application"
fi
