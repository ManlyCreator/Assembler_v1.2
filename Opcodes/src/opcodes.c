#include <string.h>
#include "opcodes.h"

// Instruction-Code Association for Computation Instructions
char* compInstructions[28] = { 
  "0",   "1",   "-1",  "D",   "A",   "!D",  "!A",
  "-D",  "-A",  "D+1", "A+1", "D-1", "A-1", "D+A",
  "D-A", "A-D", "D&A", "D|A", "M",   "!M",  "-M", 
  "M+1", "M-1", "D+M", "D-M", "M-D", "D&M", "D|M"
};
char* compCodes[28] = {
  "0101010", "1111111", "0111010", "0001100", "0110000", "0001101", "0110001",
  "0001111", "0110011", "0011111", "0110111", "0001110", "0110010", "0000010",
  "0010011", "0000111", "0000000", "0010101", "1110000", "1110001", "1110011",
  "1110111", "1110010", "1000010", "1010011", "1000111", "1000000", "1010101"
};

// Instruction-Code Association for Jump Instructions
char* jmpInstructions[8] = {
  "null", "JGT", "JEQ", "JGE",
  "JLT",  "JNE", "JLE", "JMP"
};
char* jmpCodes[8] = {
  "000", "001", "010", "011",
  "100", "101", "110", "111"
};

char* getValue(char** keyTable, char** valueTable, char* key) {
  int i = 0;
  while (strcmp(keyTable[i], key) != 0) {
    i++;
  }
  return valueTable[i];
}
