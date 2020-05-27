#ifndef INIT_HPP
#define INIT_HPP

#include <iostream>
#include <cstring>
#include <string>

#include "SFML\Graphics.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Audio.hpp"

#include "Chip8.hpp"

//Chunk of code responsible for resolving "unresolved external symbol __iob_func linker" errors. 
//Had to include legacy_stdio_definitions.lib as well. Will debug this later. 
#define stdin  (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
//

//MAIN VARIABLE// These variables are intended to be used in main only, and thus are wrapped in an anonymous namespace to avoid global scope.

namespace
{
    constexpr float SCALE_FACTOR = 12.5f; //ratio between resolutions 64x32 and 800x600
}

//MAIN VARIABLE END//

//FUNCTION DECLARATION//

void keyStateReleased(Chip8& chip8, sf::Event& keyState);
void keyStatePressed(Chip8& chip8, sf::Event& event, sf::SoundBuffer& beepBuffer, sf::Sound& beepSound, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect);
void pushBuffer(Chip8& chip8, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect);
void emulationLoop(Chip8& chip8, sf::Event& event, sf::SoundBuffer& beepBuffer, sf::Sound& beepSound, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect);

//FUNCTION DECLARATION END//

#endif // INIT_HPP