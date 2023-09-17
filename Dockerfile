# Start from the latest Ubuntu image
FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y build-essential valgrind irssi netcat
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y expect

COPY . /app

# RUN make re

CMD ["tail", "-f", "/dev/null"]
