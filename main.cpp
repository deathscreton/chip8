#include "EmuCore.hpp"

//Chunk of code responsible for resolving "unresolved external symbol __iob_func linker" errors. 
//Had to include legacy_stdio_definitions.lib as well. Will debug this later. 
#define stdin  (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
//



int main(int argc, char* argv[])
{
    EmuCore emucore;
    emucore.Init(argc, argv[1]);

    return 0;
}
