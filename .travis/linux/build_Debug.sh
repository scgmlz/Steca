export QTDIR=/opt/qt54
export PATH=$QTDIR/bin:$PATH

git submodule update --init --recursive
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D WITH_TESTS=1 -D WITH_COVERAGE=1 ..
make -j9
