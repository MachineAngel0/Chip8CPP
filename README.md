# Chip8 USING THE VULKAN GRAPHICS API

- - - 

A rewrote of my old chip8 emulator now using my own vulkan renderer instead of the sdl2 as it was previously.

I hope that someone can use this as an example, as there wasn't a single GitHub repository I could find also using vulkan for their Chip8 emulator

![alt text](https://github.com/MachineAngel0/Chip8CPP/blob/master/Pong.png?raw=true "Pong Chip8 Screenshot") 

### BUILD:

    -cmake --build build --config Debug

### RUN (Command Line):

    -Chip8CPP <ROM>

### NOTE:

-The emulator should be showing up in black and white but due to the way vulkan works and writes to textures, it uses an
8-bit color channel that defaults to red or black.
I couldn't find anything that would make the image become black and white even after looking through the vulkan
documentation and messing with the color params. 



