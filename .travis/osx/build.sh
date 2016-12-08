if [ "${CXX}" = "g++" ]; then export MKSPEC=macx-g++; export CC=gcc-4.8; export CXX=g++-4.8; fi

export QTDIR=/usr/local/opt/qt5
export PATH=$QTDIR/bin:$PATH

git submodule update --init --recursive
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=$BUILD_TYPE ..
make -j9
