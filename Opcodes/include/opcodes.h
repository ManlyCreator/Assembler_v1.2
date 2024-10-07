#ifndef OPCODES_H
#define OPCODES_H

extern char* compInstructions[28];
extern char* compCodes[28];

extern char* jmpInstructions[8];
extern char* jmpCodes[8];

char* getValue(char** keyTable, char** valueTable, char* key);

#endif
