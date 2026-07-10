# Build phase (runs as amd64 via Rosetta emulation)
FROM --platform=linux/amd64 ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    git \
    ninja-build

# Set working directory
WORKDIR /Cobbler