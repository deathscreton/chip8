#include <iostream>
#include <cstring>
#include <string>

#include "SFML\Graphics.hpp"
#include "SFML\Window\Keyboard.hpp"

#include "Chip8.hpp"

#define SCALE_FACTOR 12.5f

Chip8 chip8; //Creates emulator object and initializes class state using constructor
sf::Event event;
sf::RenderWindow mainWindow(sf::VideoMode(800, 600), "Chip 8 Emulator"); //Create and declare Window object for rendering.
sf::RectangleShape chip8SpriteRect(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR)); //Create RectangleShape object with a size of 12.5f, which is also the scale factor.

void keyStateReleased(sf::Event keyState)
{
    switch (keyState.key.code)
    {
    case sf::Keyboard::Num1:
        chip8.key[0x1] = 0;
        break;
    case sf::Keyboard::Num2:
        chip8.key[0x2] = 0;
        break;
    case sf::Keyboard::Num3:
        chip8.key[0x3] = 0;
        break;
    case sf::Keyboard::Num4:
        chip8.key[0xC] = 0;
        break;
    case sf::Keyboard::Q:
        chip8.key[0x4] = 0;
        break;
    case sf::Keyboard::W:
        chip8.key[0x5] = 0;
        break;
    case sf::Keyboard::E:
        chip8.key[0x6] = 0;
        break;
    case sf::Keyboard::R:
        chip8.key[0xD] = 0;
        break;
    case sf::Keyboard::A:
        chip8.key[0x7] = 0;
        break;
    case sf::Keyboard::S:
        chip8.key[0x8] = 0;
        break;
    case sf::Keyboard::D:
        chip8.key[0x9] = 0;
        break;
    case sf::Keyboard::F:
        chip8.key[0xE] = 0;
        break;
    case sf::Keyboard::Z:
        chip8.key[0xA] = 0;
        break;
    case sf::Keyboard::X:
        chip8.key[0x0] = 0;
        break;
    case sf::Keyboard::C:
        chip8.key[0xB] = 0;
        break;
    case sf::Keyboard::V:
        chip8.key[0xF] = 0;
        break;
    default:
        break;
    }
}

void keyStatePressed(sf::Event keyState)
{
    switch (keyState.key.code)
    {
    case sf::Keyboard::Num1:
        chip8.key[0x1] = 1;
        break;
    case sf::Keyboard::Num2:
        chip8.key[0x2] = 1;
        break;
    case sf::Keyboard::Num3:
        chip8.key[0x3] = 1;
        break;
    case sf::Keyboard::Num4:
        chip8.key[0xC] = 1;
        break;
    case sf::Keyboard::Q:
        chip8.key[0x4] = 1;
        break;
    case sf::Keyboard::W:
        chip8.key[0x5] = 1;
        break;
    case sf::Keyboard::E:
        chip8.key[0x6] = 1;
        break;
    case sf::Keyboard::R:
        chip8.key[0xD] = 1;
        break;
    case sf::Keyboard::A:
        chip8.key[0x7] = 1;
        break;
    case sf::Keyboard::S:
        chip8.key[0x8] = 1;
        break;
    case sf::Keyboard::D:
        chip8.key[0x9] = 1;
        break;
    case sf::Keyboard::F:
        chip8.key[0xE] = 1;
        break;
    case sf::Keyboard::Z:
        chip8.key[0xA] = 1;
        break;
    case sf::Keyboard::X:
        chip8.key[0x0] = 1;
        break;
    case sf::Keyboard::C:
        chip8.key[0xB] = 1;
        break;
    case sf::Keyboard::V:
        chip8.key[0xF] = 1;
        break;
    default:
        break;
    }
}

void pushBuffer() //Fills the SFML window buffer with the gfx buffer from the chip8 then draws it to the screen.
{
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (chip8.gfx[x][y] == 1)
                chip8SpriteRect.setPosition(x * SCALE_FACTOR, y * SCALE_FACTOR); //Multiply the position value by the scale factor 12.5 so nothing overlaps
            mainWindow.draw(chip8SpriteRect);
        }
    }
}

void emulationLoop()
{
    mainWindow.setFramerateLimit(60);

    while (mainWindow.isOpen())
    {
        while (mainWindow.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                mainWindow.close();
                break;
            case sf::Event::KeyPressed:
                keyStatePressed(event);
                break;
            case sf::Event::KeyReleased:
                keyStateReleased(event);
                break;
            default:
                break;
            }
        }

        chip8.emulateCycle();

        if (chip8.drawFlag)
        {
            mainWindow.clear(sf::Color::Black);

            pushBuffer();
            
            #ifdef _DEBUG
            chip8.debugRender();
            #endif // _DEBUG

            mainWindow.display();

            chip8.drawFlag = false;
        }
    }
}

int main(int argc, char* argv[])
{
    if (chip8.loadROM(argc, argv[1]))
        emulationLoop();
    else
    {
        std::cerr << "Error: Something failed with loading the ROM." << std::endl;
        std::cin.get();
        return 1;
    }
    return 0;
}
