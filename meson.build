project('tetris-sdl', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

sdl2_dep = dependency('sdl2')

executable('tetris-sdl',
           'Tetris.cpp',
           'main.cpp',
           install : true,
           dependencies: sdl2_dep)
