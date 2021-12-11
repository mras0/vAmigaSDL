# vAmigaSDL

Minimal/test frontend for [vAmiga](https://github.com/dirkwhoffmann/vAmiga) using [SDL](https://www.libsdl.org/).

Uses "microknight" font (c) Niels Krogh "Nölb/Grafictive" Mortensen

## Building

Clone the repository including submodules: `git clone --recurse-submodules https://github.com/mras0/vAmigaSDL.git`

Create a build directory: `mkdir build`

Configure: `cd build && cmake ..`

Build: `cmake --build .`

## Using

Place "kick13.rom" in the same directory as the executable and start. Use F12 to access the "retro shell". Press F11 to take a snapshot.

Command line arguments are interpreted as disk images and inserted in order. "txt" files are executed as retro shell scripts, "snp" files as snapshots.
