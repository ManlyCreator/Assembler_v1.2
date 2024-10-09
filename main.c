#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <symbolTable.h>
#include <opcodes.h>

#define BUFSIZE 256

// TODO: Fix all memory leaks 
// (alwyas 264 bytes, must be somewhere specific)

typedef struct node {
  char line[BUFSIZE]; 
  struct node* next;
} node;

void strip(char* line);
void handleA_Instruction(char* line, char* aInstruction);
void toBinary(char* decimalString);
void handleC_Instruction(char* line, char* cInstruction);

int symbolCounter = 16;

int main(int argc, char** argv) { 
  // Opens source file provided through CLI
  initializeSymbolTable(); 
  FILE* source = fopen(argv[1], "r");
  char filename[BUFSIZE];
  if (source == NULL) {
    printf("Error While Opening File\n");
    return 1;
  }

  // Appends '.hack' to the end of the filename
  int i = 0;
  while (argv[1][i] != '.') {
    filename[i] = argv[1][i];
    i++;
  }
  filename[i] = '\0';
  strcat(filename, ".hack");

  // Creates the destination file
  remove(filename);
  FILE* destination = fopen(filename, "a");
  if (destination == NULL) {
    printf("Error Creating File\n");
    return 1;
  }

  // Initializes linked list of stripped file lines
  char lineBuf[BUFSIZE];
  node* head = malloc(sizeof(node));
  node* listPtr = head;
  while (fgets(lineBuf, BUFSIZE, source)) {
    strip(lineBuf);
    // Skips lineBuf if it is not an instruction
    if (isspace(lineBuf[0]) || lineBuf[0] == '\n' || lineBuf[0] == '/') {
      continue;
    }
    strcpy(listPtr->line, lineBuf);
    listPtr->next = malloc(sizeof(node));
    listPtr = listPtr->next;
  }

  // First pass to handle label parsing
  int instruction = 0;
  node* iterator = head;
  // Final element in linked list is an empty string,
  // so a check against strlen is made
  while (strlen(iterator->line) > 0) {
    char* line = iterator->line;
    if (!isspace(line[0]) && line[0] != '\n' && line[0] != '/' && line[0] != '(') {
      instruction++;
    } else if (line[0] == '(') {
      char labelName[BUFSIZE];
      // Stores the label
      int i = 1;
      while (line[i] != ')') {
        labelName[i - 1] = line[i];
        i++;
      }
      labelName[i - 1] = '\0';
      createSymbol(labelName, instruction);
    }
    iterator = iterator->next;
  }
  // Second pass to handle instruction translating and variables
  iterator = head;
  while (strlen(iterator->line) > 0) {
    char* line = iterator->line;
    char instruction[17];
    // Translates instructions that are not labels
    if (line[0] != '(') {
      // Checks for A-Instructions
      if (line[0] == '@') {
        handleA_Instruction(line, instruction);
        fprintf(destination, "%s\n", instruction);
      } else {
        handleC_Instruction(line, instruction);
        fprintf(destination, "%s\n", instruction);
      }
    }
    // Creates a temporary pointer to be freed
    // while iterator is moved to the next pointer
    node* iteratorCopy = iterator;
    iterator = iterator->next;
    free(iteratorCopy);
  }
  // Final memory address in linked list can never be reached
  // in the loop, so it is freed afterwards
  free(iterator);

  fclose(source);
  fclose(destination);
  unloadSymbolTable();
  return 0;
}

void strip(char* line) {
  char temp[BUFSIZE];
  strcpy(temp, line);
  unsigned int len = strlen(line);
  int i = 0;
  int j = 0;
  // Finds the final index of line containing whitespace
  while (isspace(line[i])) {
    i++;
  }
  // Iterates throguh the first index containing no whitespace 
  // to the length of line
  for (int k = i; k < len; k++, j++) {
    temp[j] = line[k];
  }
  int tempLen = strlen(temp);
  // If the copied string is large enough, its length - 2 is set
  // as a null terminator to remove the carriage return and line break
  if (tempLen > 2) {
    temp[tempLen - 2] = '\0';
  } else {
    // Sets the string as a space if it is too small
    temp[0] = ' ';
    temp[1] = '\0';
  }
  strcpy(line, temp);
}

void handleA_Instruction(char* line, char* aInstruction) {
  char a[BUFSIZE];
  int i = 1;
  int isSymbol = 0;
  // Copies everything after the '@' to a until an invalid character is detected
  if (!isdigit(line[1])) {
    isSymbol = 1;
  }
  while (i - 1 < strlen(line) - 1) {
    a[i - 1] = line[i];
    i++;
  }
  // Null terminates a  
  a[i - 1] = '\0';
  // Temporarily stores the return value of toBinary so that
  // malloc'd memory can be freed
  // If the a-instruction is a symbol,
  // this copies back its respective value
  if (isSymbol) {
    if (!isInTable(a)) {
      createSymbol(a, symbolCounter);
      symbolCounter++;
    }
    getSymbolValue(a);
  }
  toBinary(a);
  strcpy(aInstruction, a);
}

void toBinary(char* decimalString) {
  // Segmentation fault when writing directly to binaryString,
  // so a second string is declared to write back to binaryString
  // char* binaryString = malloc(17);
  char binaryStringCopy[] = "0000000000000000";
  int n = atoi(decimalString);
  int rem = 0;
  int bit = 15;
  while (n > 0) {
    rem = n % 2;
    n /= 2;
    binaryStringCopy[bit] = rem + '0';
    bit--;
  }
  strcpy(decimalString, binaryStringCopy);
}

void handleC_Instruction(char* line, char* cInstruction) {
  char comp[4]; 
  char jmp[5];
  char destCode[] = "000";
  char compCode[8];
  char jmpCode[4];
  int i = 0;

  // Destination Parsing
  int destExists = 0;
  // Checks if the instruction has a destination
  for (int j = 0; j < 4; j++) {
    if (line[j] == '=') {
      destExists = 1;
      break;
    }
  }
  // If there is a destination then the respective
  // destination bits are turned on
  if (destExists) {
    while (line[i] != '=') {
      if (line[i] == 'A') {
        destCode[0] = '1';
      } else if (line[i] == 'D') {
        destCode[1] = '1'; 
      } else if (line[i] == 'M') {
        destCode[2] = '1';
      }
      i++;
    }
    // Increments i one last time to push the instruction 
    // to the computation section
    i++;
  }
  // Computation Parsing
  // Stores the starting index of the computation 
  int compPtr = i;
  // Finds the last index of the computation
  while (line[i] != ';' && i - compPtr < 3) {
    comp[i - compPtr] = line[i]; 
    i++;
  }
  comp[i - compPtr] = '\0';
  // Converts the parsed computation into instruction bits
  strcpy(compCode, getValue(compInstructions, compCodes, comp));
  // Jump Parsing
  // Checks if the jump section is present
  if (line[i] == ';') {
    i++;
    // Initializes a string with the respective jump characters
    char jmpChars[4] = { line[i], line[i + 1], line[i + 2], '\0' };
    // Copies jmpChars into jmp
    strcpy(jmp, jmpChars);
  } else {
    strcpy(jmp, "null");
  }
  // Converts the parsed jump into instruction bits
  strcpy(jmpCode, getValue(jmpInstructions, jmpCodes, jmp));

  // Assembles the C-Instruction
  sprintf(cInstruction, "111%s%s%s", compCode, destCode, jmpCode);
}

