import os
from SCons.Script import Environment

# These should be standard install paths
env  = Environment(CPPATH=['/usr/include/SDL2'],LIBPATH=['/usr/lib'],LIBS=['SDL2', 'SDL2_ttf', 'SDL2_mixer'],CCFLAGS=['-std=c++20', '-g'])

source_files = ['main.cpp', 'tetris.cpp']

env.Program(target='tetris', source=source_files)
