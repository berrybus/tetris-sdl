import os
from SCons.Script import Environment

env  = Environment(CCFLAGS=['-std=c++17'])

env.ParseConfig('pkg-config --cflags --libs sdl2')

source_files = ['main.cpp', 'Tetris.cpp']

env.Program(target='tetris', source=source_files)
