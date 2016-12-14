cd build
xvfb-run tests/tests
lcov -c -d .
