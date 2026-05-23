#! /usr/bin/env bash

set -xe

if ! pkg-config --exists glfw3; then
    echo "Error: Glfw not found on the path"
    exit 1
fi

FLAGS="-Wpedantic -Wall -Wextra -std=c++17 -O2"
GLFW_FLAGS=$(pkg-config --cflags glfw3 gl)
GLFW_LIBS=$(pkg-config --libs glfw3 gl)

g++ ${FLAGS} ${GLFW_FLAGS} -o app src/Application.cpp ${GLFW_LIBS}
