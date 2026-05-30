#!/usr/bin/env bash

rm TAGS

etags ./src/**.cpp ./deps/glew-2.3.1/include/GL/*.h ./deps/glfw-3.4/include/GLFW/*.h
