#include <iostream>
#include <cstring>
#include <string>

#include "SFML\Graphics.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Audio.hpp"

#include "Chip8.hpp"
#include "Init.hpp"
#include "KeyHandler.hpp"

void pushBuffer(Chip8& chip8, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect) //Fills the SFML window buffer with the gfx buffer from the chip8 then draws it to the screen.
{
    auto [x_range, y_range] = chip8.get_GfxRange();
    
    x_scale = x_res / x_range;
    y_scale = y_res / y_range;
    
    for (unsigned int y = 0; y < y_range; ++y)
    {
        for (unsigned int x = 0; x < x_range; ++x)
        {
            //this if statement is not properly containing the float for the quotient of y_res / y_range. This needs to be debugged. 
            if (chip8.gfx[x][y] == 1) chip8SpriteRect.setPosition(x * x_scale, y * y_scale ); //Multiply the position value by the scale factor so nothing overlaps and the array scales.
            
            mainWindow.draw(chip8SpriteRect);
        }
    }
}

void emulationLoop(Chip8& chip8, sf::Event& event, sf::SoundBuffer& beepBuffer, sf::Sound& beepSound, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect) //Main emulation loop responsible for running cycles, checking events, playing sounds, etc.
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
                keyStatePressed(chip8, event, beepBuffer, beepSound, mainWindow, chip8SpriteRect);
                break;
            case sf::Event::KeyReleased:
                keyStateReleased(chip8, event);
                break;
            default:
                break;
            }
        }
        for (int clockCycle = 0; clockCycle <= 9; clockCycle++)
        {
            if (!chip8.isPaused) chip8.emulateCycle();
        }
        if (chip8.quitFlag) mainWindow.close();
        if (chip8.drawFlag)
        {
            mainWindow.clear(sf::Color::Black);

            pushBuffer(chip8, mainWindow, chip8SpriteRect);
            
            #ifdef _DEBUG
            //chip8.debugRender();
            #endif // _DEBUG

            mainWindow.display();

            chip8.drawFlag = false;
        }
        if (chip8.playSound)
        {
            beepSound.play();
            chip8.playSound = false;
        }
    }
}

int main(int argc, char* argv[])
{
    //OBJECT INITIALIZATION//

    Chip8 chip8; //Creates emulator object and initializes class state using constructor
    sf::Event event; //Creates event object to contain event types necessary for interactivity.
    sf::SoundBuffer beepBuffer; //Creates sound buffer to hold beep.wav file.
    sf::Sound beepSound; //Creates sound object to control buffer playback.
    sf::RenderWindow mainWindow(sf::VideoMode(x_res, y_res), "Chip 8 Emulator"); //Create and declare Window object for rendering.
    sf::RectangleShape chip8SpriteRect(sf::Vector2f(10, 11.25)); //Create RectangleShape object with a size of scale factor.

    //OBJECT INITIALIZATION END//

    if (chip8.setOpenParams(argc, argv[1]))
    {
        if (chip8.loadROM())
        {
            if (!beepBuffer.loadFromFile("beep.wav"))
            {
                std::cerr << "Error loading sound file. Please check that the file name is beep.wav and is located next to the executable. Continuing execution... \n";
            }
            else
            {
                beepSound.setBuffer(beepBuffer);
            }
            emulationLoop(chip8, event, beepBuffer, beepSound, mainWindow, chip8SpriteRect);
        }
        else
        {
            std::cerr << "Error: Something failed with loading the ROM. Check provided errors and try again. \n";
            std::cin.get();
            return 1;
        }
    }
    else
    {
        std::cerr << "Something failed with setting opening program parameters. \n";
        return 1;
    }
    return 0;
}
