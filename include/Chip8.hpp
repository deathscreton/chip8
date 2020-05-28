#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

class Display;

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

    void connectDisplay(Display* n);

    //Function responsible for loading program into memory.
    bool loadROM();
    
    //Function responsible for a single emulated CPU cycle.
    void emulateCycle();

    //Draws graphics buffer 'gfx[x][y]' to the console.
    void debugRender();

    //Resets program using current ROM in memory.
    void softReset();

    //Resets program and clears ALL memory. Reruns loadRom function. 
    void hardReset();

    //Method responsible for setting chip8 opening variables from argc and argv. 
    bool setOpenParams(const int argc, const char* rom);

    ///////////////////////
    ///PUBLIC MEMBER VARIABLES
    ///////////////////////

    bool drawFlag;                                     //Determines if something was written to the display buffer and needs to be pushed.
    bool playSound;                                    //Determines if the beep sound is ready to play. 
    bool quitFlag;                                     //Determines if the program/interpreter requested the application to quit. Currently closes application, but may change it to reset instead.
    bool superFlag;                                    //Determines if the system should use highres or low. 

    char unsigned gfx[64][32]{0,0};                    //Screen buffer, 2048 pixels total. Switched to a 2D array, last buffer used 1D array with a 64 multiple offset for the y value.
    char unsigned key[16]{0};                          //Key buffer that stores the state of a pressed key; the value does not matter. Anything other than 0 indicates a pressed state.

private:

    Display* p_display = nullptr;

    /*Used as a typedef for opFunc and stores data passed to it by the initializer list in vector 'parentFuncTable'.*/
    struct opFunc
    {
        std::string opName;
        void (Chip8::*opCo)(void) = nullptr;
    };

    //Defines the offset required to select the proper childOpcode based on the parentOpcode.
    enum childFuncOffset { noOffset = 0, opCode8 = 2, opCodeE = 11, opCodeF = 13, } offset;

    //Stores intializer lists of type 'opFunc' that contain pointers to a certain function. 
    std::vector<opFunc> parentFuncTable;

    //Stores initializer lists of type 'opFunc' that contain pointers to opcode functions (child functions) derived from a parent opcode, such as 8XXX.
    std::vector<opFunc> childFuncTable;

    ///////////////////////
    //PRIVATE HELPER FUNCTIONS
    ///////////////////////

    //Fetches and stores the Opcode in class variable 'opcode'; sets hi and lo nibble for usage in findFunc. Also sets the offset for child opcode functions in childFuncTable.
    void fetch();

    //Find the initial opcode function using the hinibble. If validOP returns true, the nibble is passed to parentFuncTable to initiate the function. If false, it hands off the program to the catch-all function XXXX.
    void findFunc();

    //Checks to make sure the opcode being generated is a legal instruction.
    bool isValidOp(const char &nibble);

    //Checks to see if a sound needs to be played. If so, returns true.
    bool isSoundReady();

    //Function responsible for returning the offset value based on the child opcode function requested by the loNibble.
    enum childFuncOffset getOffset();

    ///////////////////////
    ///ORIGINAL CHIP8 OPCODE FUNCTION DECLARATIONS
    ///////////////////////

    void zeroOp();  void CLS();     void RET();
    void JMP();     void CALL();    void SEXB();
    void SNEXB();   void SEXY();    void LDXB();
    void ADXB();    void eightOp(); void LDXY();
    void ORXY();    void ANDXY();   void XORXY();
    void ADDXY();   void SUBXY();   void SHRX();
    void SUBNXY();  void SHLXY();   void SNEXY();
    void LDI();     void JP0B();    void RNDXB();
    void DRWXY();   void hexEOp();  void SKPX();
    void SKNPX();   void fOp();     void LDXDT();
    void LDXK();    void LDDTX();   void LDSTX();      
    void ADDIX();   void LDFX();    void LDBX();
    void LDIX();    void LDXI();    void XXXX();

    ///////////////////////
    ///SUPER CHIP8 OPCODE FUNCTION DECLARATIONS
    ///////////////////////

    void SCUN();    void SCDN();    void SCR();
    void SCL();     void EXIT();    void LOW();
    void HIGH();    void LDISC();   void LDRVX();
    void LDVXR();

    ///////////////////////
    ///PRIVATE MEMBER VARIABLES
    ///////////////////////

    char unsigned memory[4096];                     //4KB of memory used to store the ROM and other bits of important data.
    short unsigned emuStack[16];                    //Array that emulate the Stack; capable of storing sixteen levels of indirections.
    short unsigned openArg;                         //Object member that stores argc values.

    char unsigned V[16];                            //registers from V0 to VF, with VF being used for flags
    char unsigned RPL_FLAGS[7];                     //RPL Flags used by SCHIP games. 

    short unsigned I = 0;                           //Address Register used as an index to keep track of placement in memory.
    short unsigned pc = 0;                          //Program Counter used to keep track of where we are in the memory.
    short unsigned sp = 0;                          //Stack Pointer used to store the pc in the stack for temporary jumps.
    short unsigned opcode = 0x0000;                 //Variable used to store the current opcode being executed.

    char unsigned hiNibble = 0x00;                  //Carries the high nibble of the opcode. Used to determine group function.
    char unsigned loNibble = 0x00;                  //Carries the low nibble of the opcode. Used to determine specific function.

    char unsigned delayTimer = 0;                   //Timer variable. Counts down at 60 Hz.
    char unsigned soundTimer = 0;                   //Sound variable. Counts down at 60 Hz. Rings at non-zero.

    std::string romName;                            //String variable that carries the current ROM in memory.

    int unsigned const FONT_OFFSET = 80;            //Offset for SCHIP8 Fontset.

}; //Class Chip8

#endif // CHIP8_HPP
