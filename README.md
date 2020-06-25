# chip8
A Chip8/SCHIP8 emulator coded in C++ using SFML 2.5.1.

The functionality of the core emulator is there. It will run Chip8 and SCHIP games and control them, and make a beep noise when appropriate.
To load a ROM, either drag and drop a ROM file onto the exe, or open run the "Chip8 Emulator.exe" file and provide the program with
a absolute pathfile: "C:\Roms\INVADERS.c8". The extension of the file does not matter. Do not include any quotes in the path.

You control the program using the keys below:

'1234
 qwer
 asdf'
 
F1 soft restarts the game, allowing you to restart the game from the beginning. F2 hard resets the program, allowing you to choose a new ROM. ESC is used to quit during play. Pressing "Space" will pause emulation.

You can ignore the VS19 solution files if you use a different IDE.


Resources used to build the emulator:
https://github.com/trapexit/chip-8_documentation and his SCHIP Opcodes.
http://devernay.free.fr/hacks/chip8/schip.txt for the Chip8 and SCHIP opcodes and explanations. 
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/ for getting me into emulation in the first place. 
