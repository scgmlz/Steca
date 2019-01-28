docker build -t bornagain/wily -f `dirname $0`/Dockerfile .
docker run -d --name dd -it bornagain/wily
docker ps -a
