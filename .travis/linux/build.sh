export QTDIR=/opt/qt54
export PATH=$QTDIR/bin:$PATH

mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=$BUILD_TYPE ..
make -j9
