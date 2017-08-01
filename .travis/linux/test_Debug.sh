cd build
xvfb-run tests/tests
lcov -c -d . -o coverage.info
lcov -r coverage.info 'tests/*' '/opt/*' '/usr/*' -o coverage.info
lcov -l coverage.info
coveralls-lcov -t ${COVERALLS_TOKEN} coverage.info
