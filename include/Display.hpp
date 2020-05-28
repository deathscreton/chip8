#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "Chip8.hpp"

class Display
{
public:

	Display();
	~Display();

	Chip8 chip; 

public:
	
	void set_HiRes();
	void set_LoRes();
};

#endif // !DISPLAY_HPP