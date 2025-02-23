import os
import platform
from SCons.Script import Environment

# These should be standard install paths
if platform.system() == "Linux":
    env  = Environment(CPPPATH=['/usr/include/SDL2'],LIBPATH=['/usr/lib'],LIBS=['SDL2', 'SDL2_ttf', 'SDL2_mixer'],CCFLAGS=['-std=c++20', '-g'])
elif platform.system() == "Windows":
    env  = Environment(CPPPATH=['windows/include'],LIBPATH=['windows/lib/x64'],LIBS=['SDL2', 'SDL2_ttf', 'SDL2_mixer', 'SDL2main', 'shell32'],CCFLAGS=['/std:c++latest'], LINKFLAGS="/SUBSYSTEM:WINDOWS")
else:
    # This will not work
    print("Unsupported environment")
    env = Environment()

source_files = ['main.cpp', 'tetris.cpp', 'font_manager.cpp']

env.Program(target='tetris', source=source_files)
