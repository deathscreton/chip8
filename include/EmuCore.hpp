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

class EmuCore
{


public:
	//METHODS//

	//Default constructor.
	EmuCore();

	//Creates members then calls StartEmu. 
	void Init(const uint32_t argc, const char* rom);

	//Contains program loop.
	void StartEmu(const uint32_t argc, const char* rom);

private:

	//OBJECTS//

	Chip8 chip8; //Creates emulator object and initializes class state using constructor
	sf::Event event; //Creates event object to contain event types necessary for interactivity.
	sf::Event keyState; //Creates event object titled keyState to hold state of key presses.
	sf::SoundBuffer beepBuffer; //Creates sound buffer to hold beep.wav file.
	sf::Sound beepSound; //Creates sound object to control buffer playback.
	sf::RenderWindow mainWindow{sf::VideoMode(1280, 720), "Chip 8 Emulator"}; //Create and initalize a Window object for rendering.
	sf::RectangleShape chip8SpriteRect; //Create RectangleShape object with a size of scale factor.

	//METHODS//

	//Handles the events and cpu cycles.
	void EmulationLoop();

	//Handles drawing to the screen using gfx array and SFML drawing functions.
	void pushBuffer();

	//Detects key release.
	void keyStateReleased();

	//Detects key presses.
	void keyStatePressed();

	//Method responsible for setting chip8 opening variables from argc and argv. 
	bool setOpenParams(const uint32_t argc, const char* rom);

	//Function to set romName string.
	bool setRomName();

	//Function responsible for loading program into memory.
	bool loadROM();

	//MEMBERS//

	//Struct that contains the members responsible for holding the current resolution value. This defaults to 720p.
	struct currentRes
	{
		uint32_t x_res = 1280;
		uint32_t y_res = 720;
	}coreRes;

	//Member variables responsible for the correct scaling of internal resolution to screen resolution. 
	float x_scale = 0.0f;
	float y_scale = 0.0f;

	//String variable that carries the current ROM in memory.
	std::string romName;

};
#endif //EMUCORE_HPP

