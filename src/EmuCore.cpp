#include "EmuCore.hpp"

EmuCore::EmuCore()
{
    this->Init();
}

void EmuCore::Init()
{

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
            this->EmulationLoop(chip8, event, beepBuffer, beepSound, mainWindow, chip8SpriteRect);
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

void EmuCore::EmulationLoop(Chip8& chip8, sf::Event& event, sf::SoundBuffer& beepBuffer, sf::Sound& beepSound, sf::RenderWindow mainWindow, sf::RectangleShape& chip8SpriteRect)
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
        for (uint32_t clockCycle = 0; clockCycle <= 9; clockCycle++)
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

void EmuCore::pushBuffer(Chip8& chip8, sf::RenderWindow& mainWindow, sf::RectangleShape& chip8SpriteRect)
{
    auto [x_range, y_range] = chip8.get_GfxRange();

    sf::Vector2f v_Scale((float(x_res) / x_range), (float(y_res) / y_range));
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
        std::cout << "Enter the absolute ROM file path(extension included): \n";
        std::getline(std::cin, romName);
        return true;
    }
    else if (argc == 2)
    {
        romName = rom;
        return true;
    }

    std::cerr << "Something went terribly wrong when collecting rom name. \n";
    return false;
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
    if ((4096 - 512) > fsize)
    {
        for (uint32_t pos = 0; pos < fsize; pos++)
        {
            //chip8.memory[0x200 + pos] = romBuffer[pos];
            chip8.writeMem();
        }
    }
    else
    {
        std::cerr << "ERROR: ROM file size is too large for memory! \n";
        return false;
    }
    return true;
}