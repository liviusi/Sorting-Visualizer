#!/usr/bin/env bash

gcc main.c -lGL -lGLU -lglut
./a.out 400
rm -f a.out