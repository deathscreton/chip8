#include "Display.hpp"

Display::Display() //default constructor
{

}

Display::Display(Chip8 &tempchip) //constructor overload that takes a chip8 object
{
	chip = &tempchip;
	chip->connectDisplay(this);
}

Display::~Display()
{

}

void Display::set_HiRes()
{

}

void Display::set_LoRes()
{

}