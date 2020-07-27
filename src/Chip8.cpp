#include "Chip8.hpp"

uint8_t const fontset[80] =             //Fontset, 0-F hex, each sprite is five bytes.
{
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

uint8_t const fontset10[160] =        //Fontset for SCHIP8, 0-F hex, each sprite is ten bytes. Two different sets of fontstyles. Either should work.
{
    0xF0, 0xF0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, // 0
    0x20, 0x20, 0x60, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x70, // 1
    0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, // 2
    0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 3
    0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, 0x10, 0x10, 0x10, 0x10, // 4
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 5
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, // 6
    0xF0, 0xF0, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, // 7
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, // 8
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 9
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x90, 0x90, 0x90, 0x90, // A
    0xE0, 0xE0, 0x90, 0x90, 0xE0, 0xE0, 0x90, 0x90, 0xE0, 0xE0, // B
    0xF0, 0xF0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0xF0, // C
    0xE0, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xE0, 0xE0, // D
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, // E
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    //Using to cutdown on an already lengthy list.
    using a = Chip8;
    /*An initializer list that uses a vector of type opFunc to store another initializer lists for struct opFunc initializers. 
    The hiNibble and loNibble variables are used to locate the required function pointer that will populate the struct.*/
    parentFuncTable =
    {            //0//               //1//                              //2//
        {"zeroOp", &a::zeroOp}, {"JMP", &a::JMP},               {"CALL", &a::CALL},        
        {"SEXB", &a::SEXB},     {"SNEXB", &a::SNEXB},           {"SEXY", &a::SEXY},     
        {"LDXB", &a::LDXB},     {"ADXB", &a::ADXB},             {"eightOp", &a::eightOp},         
        {"SNEXY", &a::SNEXY},   {"LDI", &a::LDI},               {"JP0B", &a::JP0B},     
        {"RNDXB", &a::RNDXB},   {"DRWXY", &a::DRWXY},           {"hexEOp", &a::hexEOp}, 
        {"fOp", &a::fOp},       {"Unknown Opcode", &a::XXXX}
    };

    /*Initializer list that contains opcodes that can differ based on the parent opcode.*/
    childFuncTable =
    {       
        {"SCUN", &a::SCUN},     {"SCDN", &a::SCDN},    {"CLS", &a::CLS},                ////
        {"RET", &a::RET},       {"SCR", &a::SCR},      {"SCL", &a::SCL},                //Offset 0: 00XX Opcodes
        {"EXIT", &a::EXIT},     {"LOW", &a::LOW},      {"HIGH", &a::HIGH},              ////

        {"LDXY", &a::LDXY},     {"ORXY", &a::ORXY},    {"ANDXY", &a::ANDXY},            /////
        {"XORXY", &a::XORXY},   {"ADDXY", &a::ADDXY},  {"SUBXY",&a::SUBXY},             //Offset 9: 8XXX Opcodes
        {"SHRX", &a::SHRX},     {"SUBNXY", &a::SUBNXY},{"SHLXY", &a::SHLXY},            /////

        {"SKPX", &a::SKPX},     {"SKNPX", &a::SKNPX},                                   //Offset 18: EXXX Opcodes

        {"LDXDT", &a::LDXDT},   {"LDXK", &a::LDXK},     {"LDDTX", &a::LDDTX},           /////
        {"LDSTX", &a::LDSTX},   {"ADDIX", &a::ADDIX},   {"LDFX", &a::LDFX},             //Offset 20: FXXX Opcodes
        {"LDBX", &a::LDBX},     {"LDIX", &a::LDIX},     {"LDXI", &a::LDXI},             /////
        {"LDISC", &a::LDISC},   {"LDRVX", &a::LDRVX},   {"LDVXR", &a::LDVXR}
    };

    //std::cout << "Constructor initializing... \n";

    Chip8::LOW(); //set resolution to lowres.

    pc = 0x200;//0x200 == 512
    I = 0;
    opcode = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;

    //clears screen buffer
    for (uint32_t x = 0; x < 128; x++)
        for (uint32_t y = 0; y < 64; y++)
            gfx[x][y] = 0;

    //nulls memory for initialization
    for (uint32_t x = 0; x < 4096; x++)
        memory[x] = 0;

    //fill memory with chip8 fontset
    for (uint32_t x = 0; x < 80; x++)
        memory[x] = fontset[x];

    //fill memory with schip8 fontset
    for (uint32_t x = 0; x < 160; x++)
        memory[x + 80] = fontset10[x];

    //nulls the stack
    for (uint32_t x = 0; x < 16; x++)
        emuStack[x] = 0;

    //nulls all registers
    for (uint32_t x = 0; x < 16; x++)
        V[x] = 0;

    //clears all user HP flags
    for (uint32_t x = 0; x < 7; x++)
        RPL_FLAGS[x] = 0;

    playSound = false;
    drawFlag = true;
    quitFlag = false;
    superFlag = false;
    isPaused = false;

#ifdef _DEBUG
    std::cout << "gfx buffer is " << sizeof(gfx) << " bytes big. \n";
#endif //!_DEBUG
}

Chip8::~Chip8()
{

}

void Chip8::softReset()
{
    Chip8::LOW();

    pc = 0x200;//0x200 == 512
    I = 0;
    opcode = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;

    //clears screen buffer
    for (uint32_t x = 0; x < 128; x++)
        for (uint32_t y = 0; y < 64; y++)
            gfx[x][y] = 0;

    //nulls the stack
    for (uint32_t x = 0; x < 16; x++)
        emuStack[x] = 0;

    //nulls all registers
    for (uint32_t x = 0; x < 16; x++)
        V[x] = 0;

    //clears all key registers
    for (uint32_t x = 0; x < 16; x++)
        key[x] = 0;

    //clears all user HP flags
    for (uint32_t x = 0; x < 7; x++)
        RPL_FLAGS[x] = 0;

    playSound = false;
    drawFlag = true;
    quitFlag = false;
    superFlag = false;
    isPaused = false;

#ifdef _DEBUG
    std::cout << "Current ROM loaded:" << romName;
#endif // _DEBUG
}

void Chip8::hardReset()
{

    Chip8::LOW();

    pc = 0x200;//0x200 == 512
    I = 0;
    opcode = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;

    //clears screen buffer
    for (uint32_t x = 0; x < 128; x++)
        for (uint32_t y = 0; y < 64; y++)
            gfx[x][y] = 0;

    //nulls memory for initialization
    for (uint32_t x = 0; x < 4096; x++)
        memory[x] = 0;

    //fill memory with fontset
    for (uint32_t x = 0; x < 80; x++)
        memory[x] = fontset[x];

    //fill memory with schip8 fontset
    for (uint32_t x = 0; x < 160; x++)
        memory[x + 80] = fontset10[x];

    //nulls the stack
    for (uint32_t x = 0; x < 16; x++)
        emuStack[x] = 0;

    //nulls all registers
    for (uint32_t x = 0; x < 16; x++)
        V[x] = 0;

    //clears all key registers
    for (uint32_t x = 0; x < 16; x++)
        key[x] = 0;

    //clears all user HP flags
    for (uint32_t x = 0; x < 7; x++)
        RPL_FLAGS[x] = 0;

    playSound = false;
    drawFlag = true;
    quitFlag = false;
    superFlag = false;
    isPaused = false;

    std::cout << "Enter the absolute ROM file path(extension included): \n";
    getline(std::cin, romName);

#ifdef _DEBUG
    std::cout << "Current ROM being loaded:" << romName;
#endif // _DEBUG
}

//Function responsible for a single emulated CPU cycle.
void Chip8::emulateCycle()
{
    fetch();

    findFunc();

    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
    {
        if (isSoundReady())
        {
            playSound = true;
        }
        --soundTimer;
    }
}

//Fetches and stores the Opcode in class variable 'opcode'; sets hi and lo nibble for usage in findFunc.
void Chip8::fetch()
{
    opcode = memory[pc] << 8 | memory[pc + 1];

    hiNibble = (memory[pc] & 0xF0) >> 4;
    loNibble = (memory[pc + 1]);
}

//Find the initial opcode function using the hiNibble. If isValidOP returns true, the nibble is passed to parentFuncTable to initiate the function. If false, it hands off the program to the catch-all function XXXX.
void Chip8::findFunc()
{
    if (isValidOp(hiNibble)) 
        (this->*parentFuncTable[hiNibble].opCo)();
    else
        (this->*parentFuncTable[16].opCo)();
}

//Returns true if the nibble provided is apart of a valid opcode.
bool Chip8::isValidOp(const uint8_t &nibble)
{
    return (0x00 <= nibble && nibble <= 0xF);
}

//Method responsible for setting chip8 opening variables from argc and argv. 
bool Chip8::setOpenParams(const uint32_t argc, const char* rom)
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
bool Chip8::loadROM()
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
            memory[0x200 + pos] = romBuffer[pos];
        }
    }
    else
    {
        std::cerr << "ERROR: ROM file size is too large for memory! \n";
        return false;
    }
    return true;
}


//Supplies an offset for child opcodes that are derived from a parent opcode like 8XXX. This offset is used to determine which opcode function is ran from within vector childFuncTable. 
enum class Chip8::childFuncOffset Chip8::getOffset()
{
    switch (hiNibble)
    {
    case 0x8:
        return Chip8::childFuncOffset::opCode8;
        break;
    case 0xE:
        return Chip8::childFuncOffset::opCodeE;
        break;
    case 0xF:
        return Chip8::childFuncOffset::opCodeF;
        break;
    default:
        return Chip8::childFuncOffset::noOffset;
        break;
    }
}

#ifdef _DEBUG
//Draws graphics buffer 'gfx[x][y]' to the console.
void Chip8::debugRender()
{
    auto [x_range, y_range] = Chip8::get_GfxRange();

    for (uint32_t y = 0; y < y_range; ++y)
    {
        for (uint32_t x = 0; x < x_range; ++x)
        {
            if (gfx[x][y] == 0)
                printf(" ");
            else
                printf("O");
        }
        printf("\n");
    }
    printf("\n");

}
#endif // _DEBUG

////////////
///CHIP8 OPCODE FUNCTION IMPLEMENTATION
////////////

//Uses the loNibble to determine between 00E0 and 00EE.
void Chip8::zeroOp()
{
    switch (loNibble)
    {
    case 0xE0:
        (this->*childFuncTable[2].opCo)();
        break;
    case 0xEE:
        (this->*childFuncTable[3].opCo)();
        break;
    case 0xFB:
        (this->*childFuncTable[4].opCo)();
        break;
    case 0xFC:
        (this->*childFuncTable[5].opCo)();
        break;
    case 0xFD:
        (this->*childFuncTable[6].opCo)();
        break;
    case 0xFE:
        (this->*childFuncTable[7].opCo)();
        break;
    case 0xFF:
        (this->*childFuncTable[8].opCo)();
        break;
    default:
        switch ((loNibble & 0xF0) >> 4)
        {
        case 0xB:
            (this->*childFuncTable[0].opCo)();
            break;
        case 0xC:
            (this->*childFuncTable[1].opCo)();
            break;
        default:
            (this->*parentFuncTable[16].opCo)();
            break;
        }
        break;
    }
}

//Clears the display buffer.
void Chip8::CLS()
{
    for (uint32_t x = 0; x < range.x; x++)
    {
        for (uint32_t y = 0; y < range.y; y++)
        {
            gfx[x][y] = 0;
        }
    }
    drawFlag = true;
    pc += 2;
}

//Returns from a subroutine.
void Chip8::RET()
{
    --sp;
    pc = emuStack[sp];
    pc += 2;
}

//1nnn: Jump to location nnn.
void Chip8::JMP()
{
    pc = (opcode & 0x0FFF);
}

//2nnn: Call subroutine at nnn.
void Chip8::CALL()
{
    emuStack[sp] = pc;
    ++sp;
    pc = (opcode & 0x0FFF);
}

//3xkk: Skip next instruction if Vx = kk.
void Chip8::SEXB()
{
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 4;
    else
        pc += 2;
}

//4xkk: Skips next instruction if Vx != kk.
void Chip8::SNEXB()
{
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 4;
    else
        pc += 2;
}

//5xy0: Skip next instruction if Vx = Vy.
void Chip8::SEXY()
{
    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        pc += 4;
    else
        pc += 2;
}

//6xkk: Set Vx = kk.
void Chip8::LDXB()
{
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
}

//7xkk: Set Vx = Vx + kk.
void Chip8::ADXB()
{
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2;
}

//Uses the loNibble to determine which 8XXX opcode to run
void Chip8::eightOp()
{
    uint32_t offset = static_cast<uint32_t>(getOffset());//possibly move to inside parent opcodes that call a child opcode

    switch (loNibble & 0x0F)
    {
    case 0x00:
        (this->*childFuncTable[0 + offset].opCo)();
        break;
    case 0x01:
        (this->*childFuncTable[1 + offset].opCo)();
        break;
    case 0x02:
        (this->*childFuncTable[2 + offset].opCo)();
        break;
    case 0x03:
        (this->*childFuncTable[3 + offset].opCo)();
        break;
    case 0x04:
        (this->*childFuncTable[4 + offset].opCo)();
        break;
    case 0x05:
        (this->*childFuncTable[5 + offset].opCo)();
        break;
    case 0x06:
        (this->*childFuncTable[6 + offset].opCo)();
        break;
    case 0x07:
        (this->*childFuncTable[7 + offset].opCo)();
        break;
    case 0x0E:
        (this->*childFuncTable[8 + offset].opCo)();
        break;
    default :
        (this->*parentFuncTable[16].opCo)();
        break;
    }
}

//8xy0: Set Vx = Xy.
void Chip8::LDXY()
{
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

//8xy1: Set Vx = Vx OR(bitwise) Vy.
void Chip8::ORXY()
{
    V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]);
    pc += 2;
}

//8xy2: Set Vx = Vx AND(bitwise) Vy.
void Chip8::ANDXY()
{
    V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4]);
    pc += 2;
}

//8xy3: Set Vx = Vx XOR Vy.
void Chip8::XORXY()
{
    V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]);
    pc += 2;
}

//8xy4: Adds VX to VY. VF is set to 1 when there's a carry and 0 when there isn't. Then stores the result in VX.
void Chip8::ADDXY()
{
    if ((V[(opcode & 0x0F00) >> 8]) + (V[(opcode & 0x00F0) >> 4]) > 0xFF)
        V[0xF] = 1;
    else
        V[0xF] = 0;
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

//8xy5: Compares VX to VY. If larger, set VF flag to 1, else set to 0, then subtract VY from VX, store the results in VX.
void Chip8::SUBXY()
{
    if (V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4])
        V[0xF] = 1;
    else
        V[0xF] = 0;
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

//8xy6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
void Chip8::SHRX()
{
    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
    V[(opcode & 0x0F00) >> 8] >>= 1;
    pc += 2;
}

//8xy7: Compares VY to VX. If larger, set VF flag to 1, else set to 0, then subtract VX from VY, store the results in VX.
void Chip8::SUBNXY()
{
    if (V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8])
        V[0xF] = 1;
    else
        V[0xF] = 0;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

//8xyE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
void Chip8::SHLXY()
{
    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0xF0) >> 7;
    V[(opcode & 0x0F00) >> 8] <<= 1;
    pc += 2;
}

//9xy0: Skip next instrucion if VX != Vy
void Chip8::SNEXY()
{
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        pc += 4;
    else
        pc += 2;
}

//Annn: Set I = nnn.
void Chip8::LDI()
{
    I = opcode & 0x0FFF;
    pc += 2;
}

//Bnnn: Jump to location nnn + V0.
void Chip8::JP0B()
{
    pc = V[0x0] + (opcode & 0x0FFF);
}

//Cxkk: Set Vx = random byte AND(bitwise) kk.
void Chip8::RNDXB()
{
    V[(opcode & 0x0F00) >> 8] = std::rand() & (opcode & 0x00FF);
    pc += 2;
}

//Dxyn: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. If n=0 and superflag = 1, draw a 16x16 sprite.
void Chip8::DRWXY()
{
    short unsigned x = V[(opcode & 0x0F00) >> 8];
    short unsigned y = V[(opcode & 0x00F0) >> 4];
    short unsigned height = opcode & 0x000F;
    short unsigned bits = 8;
    short unsigned mask = 0x80;

    std::vector<short unsigned> pixel;

    V[0xF] = 0;//sets flag register to zero. necessary for correct collision detection.
    
    if (height == 0 and superFlag) //checks if schip8 draw function is necessary and sets required values and loads two byte sprites into memory. 
    {   
        height = 16; 
        bits = 16; 
        mask = 0x8000;

        for (uint32_t index = 0; index < 32; index += 2)
        {
            pixel.push_back(memory[I + index] << 8 | memory[I + index + 1]); //twobyte sprite load is similar to opcode load.
        }
    }
    else //otherwise, loads regular byte sprite into memory.
    {
        for(uint32_t index = 0; index < height; index++)
        {
            pixel.push_back(memory[I + index]);
        }
    }
    for (uint32_t yline = 0; yline < height; yline++)//loop for however many rows there are which is determined by 'height'
    {
        for (uint32_t xline = 0; xline < bits; xline++)//loop for all eight bits, sixteen if using schip8 draw function
        {
            if ((pixel[yline] & (mask >> xline)) != 0)//scans the bits one at a time. if the current bit is 1, then it needs to be checked for collision. shifts bits to the right based on loop iteration.
            {
               if (gfx[(x + xline) % range.x][(y + yline) % range.y] == 1)//checks if the bit on the screen is set. if it is, set VF to 1. used for collision. % used to make sure selection is inbounds.
               {
                  V[0xF] = 1;//set flag register to one for collision
               }
               gfx[(x + xline) % range.x][(y + yline) % range.y] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
}

//Exnn: Determines what 'Exnn' opcode to branch to based on nn.
void Chip8::hexEOp()
{
    uint32_t offset = static_cast<uint32_t>(getOffset());//possibly move to inside parent opcodes that call a child opcode

    if (loNibble == 0x9E)
        (this->*childFuncTable[0 + offset].opCo)();
    else
        (this->*childFuncTable[1 + offset].opCo)();
}

//Ex9E: Skip next instruction if key with the value of Vx is pressed.
void Chip8::SKPX()
{
    if ((key[V[((opcode & 0x0F00) >> 8)]]) != 0)
        pc += 4;
    else
        pc += 2;
}

//ExA1: Skip next instruction if key with the value of Vx is not pressed. 
void Chip8::SKNPX()
{
    if ((key[V[((opcode & 0x0F00) >> 8)]]) == 0)
        pc += 4;
    else
        pc += 2;
}

//Uses the entire loNibble to determine what FXXX opcode function to run. 
void Chip8::fOp()
{
    uint32_t offset = static_cast<uint32_t>(getOffset());//possibly move to inside parent opcodes that call a child opcode

    switch (loNibble)
    {    
    case 0x07:
        (this->*childFuncTable[0 + offset].opCo)();
        break;
    case 0x0A:
        (this->*childFuncTable[1 + offset].opCo)();
        break;
    case 0x15:
        (this->*childFuncTable[2 + offset].opCo)();
        break;
    case 0x18:
        (this->*childFuncTable[3 + offset].opCo)();
        break;
    case 0x1E:
        (this->*childFuncTable[4 + offset].opCo)();
        break;
    case 0x29:
        (this->*childFuncTable[5 + offset].opCo)();
        break;
    case 0x33:
        (this->*childFuncTable[6 + offset].opCo)();
        break;
    case 0x55:
        (this->*childFuncTable[7 + offset].opCo)();
        break;
    case 0x65:
        (this->*childFuncTable[8 + offset].opCo)();
        break;
    case 0x30:
        (this->*childFuncTable[9 + offset].opCo)();
        break;
    case 0x75:
        (this->*childFuncTable[10 + offset].opCo)();
        break;
    case 0x85:
        (this->*childFuncTable[11 + offset].opCo)();
        break;
    default:
        (this->*parentFuncTable[16].opCo)();
        break;
    }
}

//Fx07: Set Vx = delay timer value.
void Chip8::LDXDT()
{
    V[(opcode & 0x0F00) >> 8] = delayTimer;
    pc += 2;
}

//Fx0A: Wait for a key press, store the value of the key in Vx.
void Chip8::LDXK()
{
    bool keyPressed = false;

    for (uint32_t pos = 0; pos < 16; pos++)
    {
        if (key[pos] != 0)
        {
            keyPressed = true;
            V[(opcode & 0x0F00) >> 8] = pos;
        }
    }
    if (!keyPressed)
        return;
    pc += 2;
}

//Fx15: Set delay timer = Vx.
void Chip8::LDDTX()
{
    delayTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

//Fx18: Set sound timer = Vx.
void Chip8::LDSTX()
{
    soundTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

//Fx1E: Set I = I + Vx.
void Chip8::ADDIX()
{
    if ((I + V[(opcode & 0x0F00) >> 8]) > 0xFFF)
        V[0xF] = 1;
    else
        V[0xF] = 0;
    I += V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

//Fx29: Set I= location of sprite for digit Vx.
void Chip8::LDFX()
{
    I = V[(opcode & 0x0F00) >> 8] * 0x5; //each character is five bytes wide; multiply the result by five so the memory for the font isn't overwritten
    pc += 2;
}

//Fx33: Store BCD representation of Vx in memory locations I, I+1 and I+2. 
void Chip8::LDBX()
{
    uint32_t BCD = V[(opcode & 0x0F00) >> 8];
    memory[I] = (BCD % 1000) / 100;
    memory[I + 1] = (BCD % 100) / 10;
    memory[I + 2] = BCD % 10;
    pc += 2;
}

//Fx55: Store registers V- through Vx in memory starting a location I. 
void Chip8::LDIX()
{
    for (uint32_t mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        memory[I + mempos] = V[mempos];
    }
    //I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

//Fx65: Read register V0 through Vx from memory starting at location I.
void Chip8::LDXI()
{
    for (uint32_t mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        V[mempos] = memory[I + mempos];
    }
    //I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

//Catch all for unknown opcodes.
void Chip8::XXXX()
{
    std::cout << "Unknown Opcode: " << std::hex << opcode << "\n";
}

////////////
///SCHIP8 OPCODE FUNCTION IMPLEMENTATION
////////////

//00BN: Scroll display N lines up. 
void Chip8::SCUN()
{
    //This instruction is for XO-Chip extension compatibility. May implement at a later date. 
}

//00CN: Scroll display N lines down.
void Chip8::SCDN()
{
    uint32_t d_Scroll = opcode & 0x000F;                                //How many lines need to be scrolled. 

    for (uint32_t x = 0; x < range.x; x++)
    {
        auto x_gfxOffset = &gfx[x][0];                                      //Sets x_gfxOffset to the address the first [x] element. 
        memmove(x_gfxOffset + d_Scroll, x_gfxOffset, range.y - d_Scroll);   //This copies the first d_Scroll elements in contingous memory starting at x_gfxOffset, and pastes it using the same pointer, but with an offset of d_Scroll.
        memset(x_gfxOffset, 0, d_Scroll);                                   //Clears now garbage at [x] + d_Scroll, setting values to 0. 
    }
    drawFlag = true;
    pc += 2;
}

//00FB: Scroll display 4 pixels to the right.
void Chip8::SCR()
{
    std::array<uint8_t, 128> temparr = { 0 };                         //temporary array to hold a single contiguous line of [y] elements. 

    for (uint32_t y = 0; y < range.y; y++)                                       //iterates over every column to provide the proper [y] element. Effectively changes the [y] element.
    {
        for (uint32_t xcolumn = 0; xcolumn < range.x; xcolumn++)                 //changes row based on loop iteration. Effectively changes the [x] element.
        {   
            auto y_gfxOffset = &gfx[xcolumn % range.x][y % range.y];        //sets the address of the current [x] and [y] element in memory based on above loops. again, another pointer so we can work on the data.
            memcpy(temparr.data() + xcolumn, y_gfxOffset, 1);               //stores bytes from the array, one byte at a time for each loop. 
        }
        for (uint32_t xcolumn = 0; xcolumn < range.x; xcolumn++)
        {
            auto y_gfxOffset = &gfx[(xcolumn + 4) % range.x][y % range.y];
            memcpy(y_gfxOffset, temparr.data() + xcolumn, 1);
        }
        for (uint32_t xcolumn = 0; xcolumn < 4; xcolumn++)
        {
            auto y_gfxOffset = &gfx[xcolumn % range.x][y % range.y];
            memset(y_gfxOffset, 0, 1);
        }
    }
    drawFlag = true;
    pc += 2;
}

//00FC: Scroll display 4 pixels to the left. 
void Chip8::SCL()
{
    std::array<uint8_t, 128> temparr = { 0 };                         //temporary array to hold a single contiguous line of [y] elements. 

    for (uint32_t y = 0; y < range.y; y++)                                       //iterates over every column to provide the proper [y] element. Effectively changes the [y] element.
    {
        for (uint32_t xcolumn = 0; xcolumn < range.x; xcolumn++)        //changes row based on loop iteration. Effectively changes the [x] element.
        {
            auto y_gfxOffset = &gfx[xcolumn % range.x][y % range.y];        //sets the address of the current [x] and [y] element in memory based on above loops. 
            memcpy(temparr.data() + xcolumn, y_gfxOffset, 1);               //stores bytes from the array, one byte at a time for each loop. 
        }
        for (uint32_t xcolumn = 0; xcolumn < range.x; xcolumn++)
        {
            auto y_gfxOffset = &gfx[(xcolumn - 4) % range.x][y % range.y];
            memcpy(y_gfxOffset, temparr.data() + xcolumn, 1);
        }
        for (uint32_t xcolumn = 127; xcolumn >= 124; xcolumn--)
        {
            auto y_gfxOffset = &gfx[xcolumn % range.x][y % range.y];
            memset(y_gfxOffset, 0, 1);
        }
    }
    drawFlag = true;
    pc += 2;
}

//00FD: Exit the interpreter.
void Chip8::EXIT()
{
    quitFlag = true;
    pc += 2;
}

//00FE: Enable low res (64x32) mode.
void Chip8::LOW()
{
    superFlag = false;
    Chip8::range = {64, 32};
    pc += 2;
}

//00FF: Enable high res (128x64) mode.
void Chip8::HIGH()
{
    superFlag = true;
    Chip8::range = {128, 64};
    pc += 2;
}

//FX30: Set I to the address of the SCHIP-8 16x10 font sprite representing the value in VX. 
void Chip8::LDISC()
{
    I = (V[(opcode & 0x0F00) >> 8] * 0xA) + FONT_OFFSET;                    //Each character is ten bytes wide; multiply the result by ten so the memory for the font isn't overwritten
    pc += 2;
}

//FX75: Store V0 through VX to HP-48 RPL user flags (X <= 7).
void Chip8::LDRVX()
{
    for (uint32_t mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        RPL_FLAGS[mempos] = V[mempos];
    }
    //I += ((opcode & 0x0F00) >> 8) + 1;                                    //This is an old portion of this instruction from SCHIP 1.0 that was removed in 1.1. It's not needed in normal cases. 
    pc += 2;
}

//FX85: Read V0 through VX from HP-48 RPL user flags (X <= 7).
void Chip8::LDVXR()
{
    for (uint32_t mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        V[mempos] = RPL_FLAGS[mempos];
    }
    //I += ((opcode & 0x0F00) >> 8) + 1;                                    //This is an old portion of this instruction from SCHIP 1.0 that was removed in 1.1. It's not needed in normal cases. 
    pc += 2;
}