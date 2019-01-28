docker exec dd bash -c "cd build; make check" && exit 0
docker exec dd bash -c "cat build/Testing/Temporary/*.log" && exit 1
