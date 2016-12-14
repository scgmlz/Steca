cd build
xvfb-run tests/tests
lcov -c -d . -o coverage.info
lcov -r coverage.info 'tests/*' -o coverage.info
lcov -l coverage.info
coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
