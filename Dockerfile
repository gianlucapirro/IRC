# Start from the latest Ubuntu image
FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y build-essential valgrind irssi && \
    sudo apt-get install netcat

COPY . /app

RUN make re

CMD ["tail", "-f", "/dev/null"]
