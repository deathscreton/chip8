#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "Chip8.hpp"


char unsigned fontset[80] =             //Fontset, 0-F hex, each sprite is five bytes.
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


Chip8::Chip8()
{
    //Using to cutdown on an already lenghty list.
    using a = Chip8;
    /*An initializer list that uses a vector of type opFunc to store another initializer lists for struct opFunc initializers. 
    The hiNibble and loNibble variables are used to locate the required function pointer that will populate the */
    jmpTable =
    {               //0//               //1//           //2//           //3//
        {"zeroOp", &a::zeroOp}, {"CLS", &a::CLS},       {"RET", &a::RET},
        {"JMP", &a::JMP},       {"CALL", &a::CALL},     {"SEXB", &a::SEXB},
        {"SNEXB", &a::SNEXB},   {"SEXY", &a::SEXY},     {"LDXB", &a::LDXB},
        {"ADXB", &a::ADXB},     {"eightOp", &a::ADXB},  {"LDXY", &a::LDXY},
        {"ORXY", &a::ORXY},     {"ANDXY", &a::ANDXY},   {"XORXY", &a::XORXY},
        {"fOp", &a::fOp},       {"ADDXY", &a::ADDXY},   {"SUBXY",& a::SUBXY},   
        {"SHRX", &a::SHRX},     {"SUBNXY", &a::SUBNXY}, {"SHLXY", &a::SHLXY},   
        {"SNEXY", &a::SNEXY},   {"LDI", &a::LDI},       {"JP0B", &a::JP0B},     
        {"RNDXB", &a::RNDXB},   {"DRWXY", &a::DRWXY},   {"hexEOp", &a::hexEOp}, 
        {"SKPX", &a::SKPX},     {"SKNPX", &a::SKNPX},   {"LDXDT", &a::LDXDT},
        {"LDXK", &a::LDXK},     {"LDDTX", &a::LDDTX},   {"LDSTX", &a::LDSTX},
        {"ADDIX", &a::ADDIX},   {"LDFX", &a::LDFX},     {"LDBX", &a::LDBX},
        {"LDIX", &a::LDIX},     {"LDIX", &a::LDIX},     {"0x0000", &a::XXXX},
    };

    //std::cout << "Constructor initializing..." << std::endl;

    pc = 0x200;//0x200 == 512
    I = 0;
    opcode = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;

    //clears screen buffer
    for (int x = 0; x < 64; x++)
        for (int y = 0; y < 32; y++)
            gfx[x][y] = 0;

    //nulls memory for initialization
    for (int x = 0; x < 4096; x++)
        memory[x] = 0;

    //fill memory with fontset
    for (int x = 0; x < 80; x++)
        memory[x] = fontset[x];

    //nulls the stack
    for (int x = 0; x < 16; x++)
        emuStack[x] = 0;

    //nulls all registers
    for (int x = 0; x < 16; x++)
        V[x] = 0;

    drawFlag = true;
}

Chip8::~Chip8()
{
    std::cout << "Destroying object..." << std::endl;
}

//Function responsible for a single emulated CPU cycle.
void Chip8::emulateCycle()
{
    fetch();
    findFunc();

    /*this switch test isolates the first 4 bits (nibble) of an opcode using the '&' bitwise operator
    along with 0xF000 in order to compare the nibble with case statements in order to find the correct
    x86 instruction to follow
    switch (opcode & 0xF000)
    {
    case 0x0://special case testing for 00E0 or 00EE
        switch (opcode & 0x000F)
        {
        case 0x0000://clears the screen
            for (int x = 0; x < 64; x++)
            {
                for (int y = 0; y < 32; y++)
                {
                    gfx[x][y] = 0;
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        case 0x000E://returns from a subroutine
            --sp;
            pc = emuStack[sp];
            pc += 2;
            break;
        default://when something breaks through the nth dimension
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
        }
        break;
    case 0x1000://jumps to address NNN
        pc = (opcode & 0x0FFF);
        break;
    case 0x2000://calls subroutine at NNN
        emuStack[sp] = pc;
        ++sp;
        pc = (opcode & 0x0FFF);
        break;
    case 0x3000://skips the next instruction if VX equals NN
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;
    case 0x4000://skips the next instruction if VX doesn't equal NN
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;
    case 0x5000://skips the next instruction if VX equals VY
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;
    case 0x6000://sets VX to NN
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        pc += 2;
        break;
    case 0x7000://adds NN to VX (carry flag is never changed)
        V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        pc += 2;
        break;
    case 0x8000://special case to test for 8XYN opcodes, where 'N' is a number corresponding to the different opcodes
        switch (opcode & 0x000F)
        {
        case 0x0000://sets VX to the value of VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0001://sets VX to VX or VY (using bitwise OR operation)
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0002://sets VX to VX and VY (using bitwise AND operation)
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0003://sets VX to VX xor VY
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0004://adds VY to VX. VF is set to 1 when there's a carry and 0 when there isn't. Then stores the result in VX.
            if ((V[(opcode & 0x0F00) >> 8]) + (V[(opcode & 0x00F0) >> 4]) >= 0xFF)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0005://compares VX to VY. If larger, set VF flag to 1, else set to 0, then subtract VY from VX, store the results in VX.
            if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0006://stores the least significant bit of VX in VF and then shifts VX to the right by 1
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
            break;
        case 0x0007://compares VY to VX. If larger, set VF flag to 1, else set to 0, then subtract VX from VY, store the results in VX.
            if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x000E://stores the most significant bit of VX in VF and then shifts VX to the left by 1
            V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0xF0) >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
            break;
        default://in case something goes tragically wrong
            printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
        }
        break;
    case 0x9000://skips the next instruction if VX doesn't equal VY (typically followed by a jmp)
        if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;
    case 0xA000://set address register I to NNN.
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    case 0xB000://jumps to the address NNN + V0
        pc = V[0x0] + (opcode & 0x0FFF);
        break;
    case 0xC000://sets VX to the result of a bitwise and operation on a rand num and NN
        V[(opcode & 0x0F00) >> 8] = std::rand() & (opcode & 0x00FF);
        pc += 2;
        break;
    case 0xD000://draws a sprite at coordinate (VX,VY) that has a width of 8 pixels and height of N pixels (Max 8x15) (DXYN)
    {
        short unsigned x = V[(opcode & 0x0F00) >> 8];
        short unsigned y = V[(opcode & 0x00F0) >> 4];
        short unsigned height = opcode & 0x000F;//maximum row amount, aka, pixel height
        short unsigned pixel;

        V[0xF] = 0;//sets flag register to zero. necessary for correct collision detection.

        for (int yline = 0; yline < height; yline++)//loop for however many rows there are which is determined by 'height'
        {
            pixel = memory[I + yline];//grab the byte from memory and store it in the variable 'pixel'. each bit in this byte is a set value for a pixel to be drawn.
            for (int xline = 0; xline < 8; xline++)//loop for all eight bits
            {
                if ((pixel & (0x80 >> xline)) != 0)//scans the bits one at a time. if the current bit is 1, then it needs to be checked for collision. shifts bits to the right based on loop iteration.
                {
                    if (gfx[x + xline][y + yline] == 1)//checks if the bit on the screen is set. if it is, set VF to 1. used for collision.
                    {
                        V[0xF] = 1;//set flag register to one for collision
                    }
                    gfx[x + xline][y + yline] ^= 1;
                }
            }
        }
        drawFlag = true;
        pc += 2;
        break;
    }
    case 0xE000://special case to test for EX9E or EXA1 opcodes.
        switch (opcode & 0x00FF)
        {
        case 0x009E://skips the next instruction if the key stored in VX is pressed
            if ((key[V[((opcode & 0x0F00) >> 8)]]) != 0)
                pc += 4;
            else
                pc += 2;
            break;
        case 0x00A1://skips the next instruction if the key stored in VX isn't pressed.
            if ((key[V[((opcode & 0x0F00) >> 8)]]) == 0)
                pc += 4;
            else
                pc += 2;
            break;
        default://for when you need to smash the emergency glass
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
        }
        break;
    case 0xF000://special case for FX opcodes.
        switch (opcode & 0x00FF)
        {
        case 0x0007://sets VX to the value of the delay timer
            V[(opcode & 0x0F00) >> 8] = delayTimer;
            pc += 2;
            break;
        case 0x000A://a key press is awaited, then stored in VX (all instruction is halted until next key event)
        {
            bool keyPressed = false;

            for (int pos = 0; pos < 16; pos++)
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
            break;
        }
        case 0x0015://sets the delay timer to VX
            delayTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0018://set sound timer to VX
            soundTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x001E://adds VX to I, VF is set to 1 if there is overflow, 0 otherwise.
            if ((I + V[(opcode & 0x0F00) >> 8]) > 0xFFF)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            I += V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0029://sets I to the location of the sprite for the character in VX.//characters 0-F (hex) are represented by a 4x5 font
            I = V[(opcode & 0x0F00) >> 8] * 0x5;//each character is five pixels wide; multiply the result by five so the memory for the font isn't overwritten
            pc += 2;
            break;
        case 0x0033://take VX in decimal form and place hundreds in location I, tens at I+1 and ones at I+2.
        {
            int BCD = V[(opcode & 0x0F00) >> 8];
            memory[I] = (BCD % 1000) / 100;
            memory[I + 1] = (BCD % 100) / 10;
            memory[I + 2] = BCD % 10;
            pc += 2;
            break;
        }
        case 0x0055://stores V0 to VX (including VX)in memory starting at I. the offset from I is increased by 1 for each value written, but I itself is left unmodified.
            for (int mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
            {
                memory[I + mempos] = V[mempos];
            }
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        case 0x0065://fills V0 to VX (including VX) with values from memory starting at I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
            for (int mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
            {
                V[mempos] = memory[I + mempos];
            }
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        default://for when shaggoth comes to crush us all
            printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
        }
        break;
    }*/
    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
    {
        if (soundTimer == 1)
            std::cout << "BEEP MOFO" << std::endl;
        --soundTimer;
    }
}

//Find the initial opcode function using the hiNibble. If validOP returns true, the nibble is passed to jmpTable to initiate the function. If false, it hands off the program to the catch-all function XXXX.
void Chip8::findFunc()
{
    
    if (isValidOp(hiNibble)) (this->*jmpTable[hiNibble].opCo)();
    else
    {
        opFunc unknown = { "Unknown Opcode", &Chip8::XXXX };
        (this->*unknown.opCo)();
    }
}

//Returns true if the nibble provided is apart of a valid opcode.
bool Chip8::isValidOp(const char &nibble)
{
    //char tempNib = ((nibble & 0xF0) >> 4);
    return (0x00 <= nibble && nibble <= 0xF);
}

//Function responsible for loading program into memory. Takes argc and a string pointer to determine path and file name.
bool Chip8::loadROM(const int argc, const char* rom)
{
    std::string romName;

    if (argc < 2)//check to see if program was opened via command line with an argument (or drag and drop) or via GUI/File Explorer.
    {
        std::cout << "Enter the absolute ROM file path(extension included): \n";
        getline(std::cin, romName);
    }
    else if (argc == 2)
    {
        romName = rom;
    }
    else
    {
        std::cerr << "Something went terribly wrong when collecting rom name. CHIP8.CPP LN: 369" << std::endl;
        return false;
    }

    //open rom file using name passed from argv or cin.
    std::ifstream romfile(romName.data(), std::ios::binary);
    if (!romfile)
    {
        std::cerr << "Error opening " << rom << ". Please check the name of your file and try again." << std::endl;
        return false;
    }

    //get rom file size
    romfile.seekg(0L, std::ios::end);
    const int unsigned fsize = romfile.tellg();
    romfile.seekg(0L, std::ios::beg);
    std::cout << romName << " is " << fsize << " bytes." << std::endl;

    //create buffer using rom size
    std::vector<char> romBuffer(fsize);
    if (romBuffer.empty())
    {
        std::cerr << "Error creating memory buffer." << std::endl;
        return false;
    }

    //write file to buffer
    romfile.read(romBuffer.data(), fsize);
    romfile.close();
    if (romBuffer.size() != fsize)
    {
        std::cerr << "Error writing file to buffer." << std::endl;
        return false;
    }

    //write buffer to memory
    if ((4096 - 512) > fsize)
    {
        for (unsigned int pos = 0; pos < fsize; pos++)
        {
            memory[0x200 + pos] = romBuffer[pos];
        }
    }
    else
    {
        std::cerr << "ERROR: ROM file size is too large for memory!" << std::endl;
        return false;
    }
    return true;
}

//Fetches and stores the Opcode in class variable 'opcode'; sets hi and lo nibble for usage in findFunc.
void Chip8::fetch()
{
    hiNibble = (memory[pc] & 0xF0) >> 4;
    loNibble = (memory[pc + 1]);

    opcode = memory[pc] << 8 | memory[pc + 1];
}

#ifdef DEBUG
//Draws graphics buffer 'gfx[x][y]' to the console.
void Chip8::debugRender()
{

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
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
#endif // DEBUG

////////////
///OPCODE FUNCTION IMPLEMENTATION
///////////

//Uses the loNibble to determine between 00E0 and 00EE.
void Chip8::zeroOp()
{
    //Code to determine specific function within '0x00NN' opcode, where NN is either 'E0' or 'EE'.
    if (loNibble == 0xE0)
        (this->*jmpTable[1].opCo)();
    else
        (this->*jmpTable[2].opCo)();
}

//Clears the display buffer.
void Chip8::CLS()
{
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 32; y++)
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
    switch (loNibble)
    {
    case 0x00:
        (this->*jmpTable[11].opCo)();
        break;
    case 0x01:
        (this->*jmpTable[12].opCo)();
        break;
    case 0x02:
        (this->*jmpTable[13].opCo)();
        break;
    case 0x03:
        (this->*jmpTable[14].opCo)();
        break;
    case 0x04:
        (this->*jmpTable[15].opCo)();
        break;
    case 0x05:
        (this->*jmpTable[16].opCo)();
        break;
    case 0x06:
        (this->*jmpTable[17].opCo)();
        break;
    case 0x07:
        (this->*jmpTable[18].opCo)();
        break;
    case 0x0E:
        (this->*jmpTable[19].opCo)();
        break;
    default :
        opFunc unknown = { "Unknown Opcode", &Chip8::XXXX };
        (this->*unknown.opCo)();
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

//8xy4: Adds VY to VX. VF is set to 1 when there's a carry and 0 when there isn't. Then stores the result in VX.
void Chip8::ADDXY()
{
    if ((V[(opcode & 0x0F00) >> 8]) + (V[(opcode & 0x00F0) >> 4]) >= 0xFF)
        V[0xF] = 1;
    else
        V[0xF] = 0;
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    pc += 2;
}


//8xy5: Compares VX to VY. If larger, set VF flag to 1, else set to 0, then subtract VY from VX, store the results in VX.
void Chip8::SUBXY()
{
    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
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
    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
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

//Dxyn: Display n-byte sprite starting at amemory location I at (Vx, Vy), set VF = collision.
void Chip8::DRWXY()
{
    short unsigned x = V[(opcode & 0x0F00) >> 8];
    short unsigned y = V[(opcode & 0x00F0) >> 4];
    short unsigned height = opcode & 0x000F;//maximum row amount, aka, pixel height
    short unsigned pixel;

    V[0xF] = 0;//sets flag register to zero. necessary for correct collision detection.

    for (int yline = 0; yline < height; yline++)//loop for however many rows there are which is determined by 'height'
    {
        pixel = memory[I + yline];//grab the byte from memory and store it in the variable 'pixel'. each bit in this byte is a set value for a pixel to be drawn.
        for (int xline = 0; xline < 8; xline++)//loop for all eight bits
        {
            if ((pixel & (0x80 >> xline)) != 0)//scans the bits one at a time. if the current bit is 1, then it needs to be checked for collision. shifts bits to the right based on loop iteration.
            {
                if (gfx[x + xline][y + yline] == 1)//checks if the bit on the screen is set. if it is, set VF to 1. used for collision.
                {
                    V[0xF] = 1;//set flag register to one for collision
                }
                gfx[x + xline][y + yline] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
}

//Exnn: Determines what 'Exnn' opcode to branch to based on nn.
void Chip8::hexEOp()
{
    if (loNibble == 0x9E)
        (this->*jmpTable[26].opCo)();
    else
        (this->*jmpTable[27].opCo)();
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
    switch (loNibble)
    {    
    case 0x07:
        (this->*jmpTable[30].opCo)();
        break;
    case 0x0A:
        (this->*jmpTable[31].opCo)();
        break;
    case 0x15:
        (this->*jmpTable[32].opCo)();
        break;
    case 0x18:
        (this->*jmpTable[33].opCo)();
        break;
    case 0x1E:
        (this->*jmpTable[34].opCo)();
        break;
    case 0x29:
        (this->*jmpTable[35].opCo)();
        break;
    case 0x33:
        (this->*jmpTable[36].opCo)();
        break;
    case 0x55:
        (this->*jmpTable[37].opCo)();
        break;
    case 0x65:
        (this->*jmpTable[38].opCo)();
        break;
    default:
        opFunc unknown = { "Unknown Opcode", &Chip8::XXXX };
        (this->*unknown.opCo)();
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

    for (int pos = 0; pos < 16; pos++)
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
    I = V[(opcode & 0x0F00) >> 8] * 0x5;//each character is five pixels wide; multiply the result by five so the memory for the font isn't overwritten
    pc += 2;
}

//Fx33: Store BCD representation of Vx in memory locations I, I+1 and I+2. 
void Chip8::LDBX()
{
    int BCD = V[(opcode & 0x0F00) >> 8];
    memory[I] = (BCD % 1000) / 100;
    memory[I + 1] = (BCD % 100) / 10;
    memory[I + 2] = BCD % 10;
    pc += 2;
}

//Fx55: Store registers V- through Vx in memory starting a location I. 
void Chip8::LDIX()
{
    for (int mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        memory[I + mempos] = V[mempos];
    }
    I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

//Fx65: Read register V0 through Vx from memory starting at location I.
void Chip8::LDXI()
{
    for (int mempos = 0; mempos <= ((opcode & 0x0F00) >> 8); mempos++)
    {
        V[mempos] = memory[I + mempos];
    }
    I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

//Catch all for unknown opcodes.
void Chip8::XXXX()
{
    std::cout << "Unknown Opcode: 0x%x%" << opcode << std::endl;
}