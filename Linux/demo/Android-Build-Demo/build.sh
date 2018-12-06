#/bin/sh
export PATH=/usr/aarch64-linux-android/bin/:$PATH
export PATH=/usr/arm-linux-android/bin/:$PATH

CXX=arm-linux-androideabi-g++ AR=arm-linux-androideabi-ar make
mv build build32
CXX=aarch64-linux-android-g++ AR=aarch64-linux-android-ar make
mv build build64

