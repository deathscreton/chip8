#ifndef CHIP8_HPP
#define CHIP8_HPP

class Chip8
{
public:
    Chip8();                                        //Constructor
    ~Chip8();                                       //Destructor

    bool loadROM(const int argc, const char* rom);  //Function responsible for loading program into memory. Takes argc and a string pointer to determine path and file name.
    void emulateCycle();                            //Function responsible for a single emulated CPU cycle.

    short fetch();                                   //Fetches and stores the Opcode in class variable 'opcode'.

    void debugRender();                             //Draws graphics buffer 'gfx[x][y]' to the console.

    bool drawFlag;                                  //Determines if something was written to the display buffer and needs to be pushed.

    char unsigned gfx[64][32];                      //Screen buffer, 2048 pixels total. Switched to a 2D array, last buffer used 1D array with a 64 multiple offset for the y value.
    char unsigned key[16];                          //Key buffer that stores the state of a pressed key; the value does not matter. Anything other than 0 indicates a pressed state.

private:

    char unsigned memory[4096];                     //4KB of memory used to store the ROM and other bits of important data.
    short unsigned emuStack[16];                    //Array that emulate the Stack; capable of storing sixteen levels of indirections.

    char unsigned V[16];                            //registers from V0 to VF, with VF being used for flags

    short unsigned I;                               //Address Register used as an index to keep track of placement in memory.
    short unsigned pc;                              //Program Counter used to keep track of where we are in the memory.
    short unsigned sp;                              //Stack Pointer used to store the pc in the stack for temporary jumps.
    short unsigned opcode;                          //Variable used to store the current opcode being executed.

    char unsigned delayTimer;                       //Timer variable. Counts down at 60 Hz.
    char unsigned soundTimer;                       //Sound variable. Counts down at 60 Hz. Rings at non-zero.

};

#endif // CHIP8_HPP
