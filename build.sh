#! /usr/bin/env bash

set -xe

GLFW_DIR="deps/glfw-3.4"
GLEW_DIR="deps/glew-2.3.1"

g++ -Wpedantic -Wall -Wextra -std=c++17 -ggdb -Og \
    -I${GLFW_DIR}/include -I${GLEW_DIR}\include \
    -o app \
    src/Application.cpp ${GLFW_DIR}/build/src/libglfw3.a ${GLEW_DIR}/lib/libGLEW.a \
    -lGL -lm
