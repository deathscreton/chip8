#ifndef BUS_HPP
#define BUS_HPP

#include "Chip8.hpp"
#include "Display.hpp"

class Bus
{
public:

	Bus();
	~Bus();

private:

	Chip8 chip;
	Display display;

};

#endif // !BUS_HPP
