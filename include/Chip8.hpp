#ifndef CHIP8_HPP
#define CHIP8_HPP

class Chip8
{
public:
    ///////////////////////
    //PUBLIC METHODS
    ///////////////////////

    //Constructor
    Chip8();

    //Destructor
    ~Chip8();

    //Function responsible for loading program into memory. Takes argc and a string pointer to determine path and file name.
    bool loadROM(const int argc, const char* rom);
    
    //Function responsible for a single emulated CPU cycle.
    void emulateCycle();

    //Fetches and stores the Opcode in class variable 'opcode'.
    void fetch();

    //Takes opCode and finds the function required to execute the opcode by using the hiNibble and loNibble respectively to locate the function intializer in vector jmpTable.
    void findFunc();

    //Draws graphics buffer 'gfx[x][y]' to the console.
    void debugRender();

    //Determines if something was written to the display buffer and needs to be pushed.
    bool drawFlag;

    ///////////////////////
    ///PUBLIC MEMBER VARIABLES
    ///////////////////////

    char unsigned gfx[64][32]{0,0};                      //Screen buffer, 2048 pixels total. Switched to a 2D array, last buffer used 1D array with a 64 multiple offset for the y value.
    char unsigned key[16]{0};                          //Key buffer that stores the state of a pressed key; the value does not matter. Anything other than 0 indicates a pressed state.

private:

    //Used as a typedef for opFunc and stores data passed to it by the initializer list in vector 'jmpTable'.
    struct opFunc
    {
        std::string opName;
        void (Chip8::* opCo)(void) = nullptr;
    };

    //Stores intializer lists of type 'opFunc' that contain pointers to a certain function. 
    std::vector<opFunc> jmpTable;


    ///////////////////////
    ///PRIVATE METHOD DECLARATIONS
    ///////////////////////
    void zeroOp(); void CLS(); void RET();
    void JMP();

    ///////////////////////
    ///PRIVATE MEMBER VARIABLES
    ///////////////////////

    char unsigned memory[4096];                     //4KB of memory used to store the ROM and other bits of important data.
    short unsigned emuStack[16];                    //Array that emulate the Stack; capable of storing sixteen levels of indirections.

    char unsigned V[16];                            //registers from V0 to VF, with VF being used for flags

    short unsigned I = 0;                           //Address Register used as an index to keep track of placement in memory.
    short unsigned pc = 0;                          //Program Counter used to keep track of where we are in the memory.
    short unsigned sp = 0;                          //Stack Pointer used to store the pc in the stack for temporary jumps.
    short unsigned opcode = 0x0000;                 //Variable used to store the current opcode being executed.

    char unsigned hiNibble = 0x00;                  //Carries the high nibble of the opcode. Used to determine group function.
    char unsigned loNibble = 0x00;                  //Carries the low nibble of the opcode. Used to determine specific function.

    char unsigned delayTimer = 0;                       //Timer variable. Counts down at 60 Hz.
    char unsigned soundTimer = 0;                       //Sound variable. Counts down at 60 Hz. Rings at non-zero.


}; //Class Chip8

#endif // CHIP8_HPP
