# A Tetris clone that only does 40L clears #

I made this to learn some basic game architecture and C++

Here is what it looks like

[output.webm](https://github.com/user-attachments/assets/96834d62-4069-4019-a2b4-3729f5b5ccd2)

Credits to perk for the awesome mashup music!

## Building ##

The build tool is `scons`. You can install it with `pip`

### Linux ###

You need SDL2, SDL2_ttf, SDL2_mixer installed in the default locations. I tested this on my Fedora 41 with Wayland machine.

### Windows 11 ###

I included the dependencies in the project directory. You should be able to play it by opening the `tetris.exe` in the project directory if you don't want to build it.
