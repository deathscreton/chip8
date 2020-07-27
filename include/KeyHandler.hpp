#ifndef KEYHANDLER_HPP
#define KEYHANDLER_HPP

#include "Chip8.hpp"

class EmuCore {};

void EmuCore::keyStateReleased(Chip8& chip8, sf::Event& keyState)
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

void EmuCore::keyStatePressed(EmuCore& emucore, Chip8& chip8, sf::Event& event, sf::SoundBuffer& beepBuffer, sf::Sound& beepSound, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect)
{
    sf::Event keyState;
    keyState = event;

    switch (keyState.key.code)
    {
    case sf::Keyboard::Escape:
        mainWindow.close();
        break;
    case sf::Keyboard::F1:
        chip8.softReset();
        emucore.EmulationLoop(chip8, event, beepBuffer, beepSound, mainWindow, chip8SpriteRect);
        break;
    case sf::Keyboard::F2:
        mainWindow.clear(sf::Color::Black);
        mainWindow.display();
        chip8.hardReset();
        emucore.loadROM();
        break;
    case sf::Keyboard::Space: //Pauses emulation if isPaused = false. Otherwise, unpauses emulation.
        chip8.isPaused == true ? chip8.isPaused = false : chip8.isPaused = true;
        break;
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

#endif // KEYHANDLER_HPP