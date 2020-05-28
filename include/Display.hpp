#ifndef DISPLAY_HPP
#define DISPLAY_HPP

//class Chip8;

#include "Chip8.hpp"

class Display
{
public:

	Display();
	Display(Chip8 &tempchip);
	~Display();

	void set_HiRes();
	void set_LoRes();

private:
	
	Chip8* chip = nullptr;
	
};

#endif // !DISPLAY_HPP