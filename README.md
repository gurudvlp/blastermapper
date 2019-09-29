# Blaster Mapper
Level editor for the Blaster Master NES ROM.

You will need a copy of the Blaster Master ROM, which is not included.

This is an early alpha release.  You can view all of the maps in the game, edit,
and save them.  There is some sort of bug still though resulting in corrupted
maps that seems to get progressively worse the more edits you make.

## Compilation

You will need to install a few packages to get this to compile.  Here are a few
for Debian:

libx11-dev<br />
mesa-common-dev<br />
libglu1-mesa-dev

## How To

In a terminal, type:

```bash
./blastmap romfilename.nes
```

From here, you use the arrow keys to navigate.

## Keys
Keyboard keys 1-8 change levels, and 0 switches between tank and overhead.<br />
Enter - Zoom in<br />
Escape - Zoom out/exit<br />
S - Save Rom to blasterout.nes<br />
C - Copy block<br />
V - Paste block<br />
Key pad 8 - Increase block id<br />
Key pad 2 - Decrease block id<br />
Key pad + - Set spawn point<br />
