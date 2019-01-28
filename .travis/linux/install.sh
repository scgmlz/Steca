docker build -t bornagain/xenial -f `dirname $0`/Dockerfile .
docker run -d --name dd -it bornagain/xenial
docker ps -a
