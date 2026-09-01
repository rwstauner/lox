FROM ubuntu:latest

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    build-essential \
    gdb \
    libreadline-dev \
  && apt-get clean
