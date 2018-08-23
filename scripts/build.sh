#!/bin/bash

g++ -g -O2 -Wall -Wextra -Wpedantic -o nanoaod_dimuon_spectrum nanoaod_dimuon_spectrum.cxx $(root-config --cflags --libs)
