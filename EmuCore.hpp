#ifndef EMUCORE_HPP
#define	EMUCORE_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <cstdint>

#include "SFML\Graphics.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Audio.hpp"

#include "Chip8.hpp"
#include "Init.hpp"
#include "KeyHandler.hpp"

class EmuCore
{
public:
	//METHODS//

	//Constructor that calls method Init().
	EmuCore();

private:
	//METHODS//
	
	//Initalizes and creates necessary member objects. 
	void Init();

	//Contains program loop.
	void StartEmu();

	//Handles the events and cpu cycles.
	void EmulationLoop();

	//Handles drawing to the screen using gfx array and SFML drawing functions.
	void pushBuffer();

	//MEMBERS//
};
#endif //EMUCORE_HPP

