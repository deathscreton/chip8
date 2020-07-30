#include "EmuCore.hpp"

void EmuCore::Init(const uint32_t argc, const char* rom)
{
    this->StartEmu(argc, rom);
}


void EmuCore::StartEmu(const uint32_t argc, const char* rom)
{
    if (this->setOpenParams(argc, rom))
    {
        if (this->loadROM())
        {
            if (!beepBuffer.loadFromFile("beep.wav"))
            {
                std::cerr << "Error loading sound file. Please check that the file name is beep.wav and is located next to the executable. Continuing execution... \n";
            }
            else
            {
                beepSound.setBuffer(beepBuffer);
            }
            this->EmulationLoop();
        }
        else
        {
            std::cerr << "Error: Something failed with loading the ROM. Check provided errors and try again. \n";
            std::cin.get();
            //return 1; //setup proper exception handling
        }
    }
    else
    {
        std::cerr << "Something failed with setting opening program parameters. \n";
        //return 1;//setup proper exception handling
    }
}

void EmuCore::EmulationLoop()
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
                keyStatePressed();
                break;
            case sf::Event::KeyReleased:
                keyStateReleased();
                break;
            default:
                break;
            }
        }
        for (uint32_t clockCycle = 0; clockCycle <= 9; clockCycle++)
        {
            if (!chip8.isPaused) chip8.emulateCycle();
        }
        if (chip8.quitFlag) mainWindow.close();
        if (chip8.drawFlag)
        {
            mainWindow.clear(sf::Color::Black);

            pushBuffer();

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

void EmuCore::pushBuffer()
{
    auto [x_range, y_range] = chip8.get_GfxRange();

    sf::Vector2f v_Scale((float(coreRes.x_res) / x_range), (float(coreRes.y_res) / y_range));
    chip8SpriteRect.setSize(v_Scale);

    for (uint32_t y = 0; y < y_range; ++y)
    {
        for (uint32_t x = 0; x < x_range; ++x)
        {
            if (chip8.gfx[x][y] == 1) chip8SpriteRect.setPosition(x * v_Scale.x, y * v_Scale.y); //Multiply the position value by the scale factor so nothing overlaps and the array scales.

            mainWindow.draw(chip8SpriteRect);
        }
    }
}

//Method responsible for setting chip8 opening variables from argc and argv. 
bool EmuCore::setOpenParams(const uint32_t argc, const char* rom)
{
    //Check to see if program was opened via command line with an argument (or drag and drop) or via GUI/File Explorer.
    if (argc < 2)
    {
        if(this->setRomName()) return true;
    }
    else if (argc == 2)
    {
        romName = rom;
        return true;
    }

    std::cerr << "Something went terribly wrong when collecting rom name. \n";
    return false;
}

//Function to set romName string.
bool EmuCore::setRomName()
{
    std::cout << "Enter the absolute ROM file path(extension included): \n";
    std::getline(std::cin, romName);
    return true;
}

//Function responsible for loading program into memory.
bool EmuCore::loadROM()
{
    //open rom file using name set by setOpenParams or hardReset
    std::ifstream romfile(romName.data(), std::ios::binary);
    if (!romfile)
    {
        std::cerr << "Error opening " << romName << ". Please check the name of your file and try again. (You didn't include quotes, did you?) \n";
        return false;
    }

    //get rom file size
    romfile.seekg(0L, std::ios::end);
    uint32_t const fsize = romfile.tellg();
    romfile.seekg(0L, std::ios::beg);
    std::cout << romName << " is " << fsize << " bytes. \n";

    //create buffer using rom size
    std::vector<char> romBuffer(fsize);
    if (romBuffer.empty())
    {
        std::cerr << "Error creating memory buffer. \n";
        return false;
    }

    //write file to buffer
    romfile.read(romBuffer.data(), fsize);
    romfile.close();
    if (romBuffer.size() != fsize)
    {
        std::cerr << "Error writing file to buffer. \n";
        return false;
    }

    //write buffer to memory
    if (chip8.writeMem(fsize, romBuffer))
    {
        std::cerr << "Something went terribly wrong writing to buffer. \n";
        return false;
    }
}

void EmuCore::keyStateReleased()
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

void EmuCore::keyStatePressed()
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

#ifdef _DEBUG
        std::cout << "Current ROM loaded:" << romName;
#endif // _DEBUG

        this->EmulationLoop();
        break;
    case sf::Keyboard::F2:
        mainWindow.clear(sf::Color::Black);
        mainWindow.display();
        chip8.hardReset();
        this->setRomName(); //setRomName returns a bool, this needs to be tested for safety

#ifdef _DEBUG
        std::cout << "Current ROM being loaded:" << romName;
#endif // _DEBUG

        this->loadROM();
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
