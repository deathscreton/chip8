#include "KeyHandler.hpp"

void pushBuffer(Chip8& chip8, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect) //Fills the SFML window buffer with the gfx buffer from the chip8 then draws it to the screen.
{
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (chip8.gfx[x][y] == 1) chip8SpriteRect.setPosition(x * SCALE_FACTOR, y * SCALE_FACTOR); //Multiply the position value by the scale factor 12.5 so nothing overlaps
            
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

        chip8.emulateCycle();

        if (chip8.quitFlag)
            mainWindow.close();

        if (chip8.drawFlag)
        {
            mainWindow.clear(sf::Color::Black);

            pushBuffer(chip8, mainWindow, chip8SpriteRect);
            
            #ifdef _DEBUG
            chip8.debugRender();
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
    sf::RenderWindow mainWindow(sf::VideoMode(800, 600), "Chip 8 Emulator"); //Create and declare Window object for rendering.
    sf::RectangleShape chip8SpriteRect(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR)); //Create RectangleShape object with a size of 12.5f, which is also the scale factor.

    //OBJECT INITIALIZATION END//

    if (chip8.setOpenParams(argc, argv[1]))
    {
        if (chip8.loadROM())
        {
            if (!beepBuffer.loadFromFile("beep.wav"))
            {
                std::cerr << "Error loading sound file. Please check that the file name is beep.wav and is located next to the executable. Continuing execution..." << std::endl;
            }
            else
            {
                beepSound.setBuffer(beepBuffer);
            }
            emulationLoop(chip8, event, beepBuffer, beepSound, mainWindow, chip8SpriteRect);
        }
        else
        {
            std::cerr << "Error: Something failed with loading the ROM. Check provided errors and try again." << std::endl;
            std::cin.get();
            return 1;
        }
    }
    else
    {
        std::cerr << "Something failed with setting opening program parameters.";
        return 1;
    }
    return 0;
}