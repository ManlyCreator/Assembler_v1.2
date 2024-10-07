#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <symbolTable.h>
#include <opcodes.h>

#define BUFSIZE 256

// TODO: Debug A-Instruction handling causing stack smashing
// TODO: Free linked list as the second pass is occurring

typedef struct node {
  char line[BUFSIZE]; 
  struct node* next;
} node;

void strip(char* line);
void insertNode(node* listPtr, node* n);
char* handleA_Instruction(char* line);
char* toBinary(char* decimalString);
char* handleC_Instruction(char* line);

int symbolCounter = 16;

// TODO: Create a function which takes the line as input and strips it of whitespace

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
  strcat(filename, ".hack");
  filename[strlen(filename)] = '\0';

  // Creates the destination file
  remove(filename);
  FILE* destination = fopen(filename, "a");
  if (destination == NULL) {
    printf("Error Creating File\n");
    return 1;
  }

  char* lineBuf = malloc(BUFSIZE);
  // Initializes linked list of stripped file lines
  node* head = malloc(sizeof(node));
  node* listPtr = head;
  int lineCtr = 0;
  while (fgets(lineBuf, BUFSIZE, source)) {
    strip(lineBuf);
    node* lineNode = malloc(sizeof(node));
    strcpy(lineNode->line, lineBuf);
    lineNode->next = NULL;
    insertNode(listPtr, lineNode);
    if (lineCtr > 0) {
      listPtr = listPtr->next;
    }
    lineCtr++;
  }
  
  // First pass to handle label parsing
  int instruction = 0;
  node* iterator = head;
  while (iterator != NULL) {
    char* line = iterator->line;
    if (!isspace(line[0]) && line[0] != '\n' && line[0] != '/' && line[0] != '(') {
      instruction++;
    } else if (line[0] == '(') {
      // label is static for the lifetime of this loop, therefore a malloc'd variable must be used so that
      // name values do not change across iterations
      char* labelNameDest = malloc(BUFSIZE);
      char labelNameSource[BUFSIZE];
      // Stores the label
      int i = 1;
      while (line[i] != ')') {
        labelNameSource[i - 1] = line[i];
        i++;
      }
      labelNameSource[i - 1] = '\0';
      strcpy(labelNameDest, labelNameSource);
      createSymbol(labelNameDest, instruction);
      free(labelNameDest);
    }
    iterator = iterator->next;
  }
  printf("First Pass Complete\n");
  // Second pass to handle instruction translating and variables
  iterator = head;
  while (iterator != NULL) {
    char* line = iterator->line;
    printf("%s\n", line);
    // Handles whitespace, comments, and labels
    if (isspace(line[0]) || line[0] == '\n' || line[0] == '/' || line[0] == '(') {
      printf("Skipping\n");
      iterator = iterator->next;
      continue;
    }
    // Checks for A-Instructions
    if (line[0] == '@') {
      printf("Handling A-Instruction\n");
      char* aInstruction = handleA_Instruction(line);
      printf("A-Instruction Handled\n");
      fprintf(destination, "%s\n", aInstruction);
      printf("Instruction Printed\n");
      free(aInstruction);
    } else {
      char* cInstruction = handleC_Instruction(line);
      fprintf(destination, "%s\n", cInstruction);
      free(cInstruction);
    }
    printf("Going to Next Line\n");
    iterator = iterator->next;
  }

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
  while (isspace(line[i])) {
    i++;
  }
  for (int k = i; k < len; k++) {
    temp[j] = line[k];
    j++;
  }
  temp[j - 1] = '\0';
  len = strlen(temp);
  if (len > 0 && temp[len - 1] == '\n') {
    temp[len - 1] = '\0';
    len--;
  }
  if (len > 0 && temp[len - 1] == '\r') {
    temp[len - 1] = '\0';
  }
  if (strlen(temp) == 0) {
    temp[0] = ' ';
    temp[1] = '\0';
  }
  strcpy(line, temp);
}

void insertNode(node* listPtr, node* n) {
  if (listPtr->line[0] == 0) {
    strcpy(listPtr->line, n->line);
    listPtr->next = NULL;
  } else {
    listPtr->next = n;
  }
}

char* handleA_Instruction(char* line) {
  char* aInstruction = malloc(17);
  char a[BUFSIZE];
  int i = 1;
  int isSymbol = 0;
  // Copies everything after the '@' to a until an invalid character is detected
  if (isdigit(line[1])) {
    while (isdigit(line[i])) {
      a[i - 1] = line[i];
      i++;
    }
  } else {
    isSymbol = 1;
    while (!isspace(line[i]) && line[i] != '\n' && line[i] != '/') {
      a[i - 1] = line[i];
      i++;
    }
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
  char* tempAInstruction = toBinary(a);
  printf("To Binary\n");
  strcpy(aInstruction, tempAInstruction);
  printf("Copied\n");
  free(tempAInstruction);
  printf("A-Instruction: %s\n", aInstruction);
  printf("Length: %lu\n", strlen(aInstruction));
  printf("Freed\n");
  return aInstruction;
}

char* toBinary(char* decimalString) {
  // Segmentation fault when writing directly to binaryString,
  // so a second string is declared to write back to binaryString
  char* binaryString = malloc(17);
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
  strcpy(binaryString, binaryStringCopy);
  return binaryString;
}

char* handleC_Instruction(char* line) {
  char* cInstruction = malloc(17);
  char comp[10]; 
  char jmp[10];
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
  while (!isspace(line[i]) && line[i] != '\n' && line[i] != ';') {
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

  return cInstruction;
}

