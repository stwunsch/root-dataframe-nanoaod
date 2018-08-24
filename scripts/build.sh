#!/bin/bash

g++ -g -O3 -Wall -Wextra -Wpedantic -o nanoaod_dimuon_spectrum nanoaod_dimuon_spectrum.cxx $(root-config --cflags --libs)
