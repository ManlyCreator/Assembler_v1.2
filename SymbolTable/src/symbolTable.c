#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

#define BUCKETS 512

symbol* symbolTable[BUCKETS];

int hash(char* name) {
  int length = strlen(name);
  int hashValue = 0;
  for (int i = 0; i < length; i++) {
    hashValue += name[i];
    hashValue = (hashValue * name[i]) % BUCKETS;
  }
  return hashValue;
}

void insertIntoTable(symbol* s) {
  int hashValue = hash(s->name);
  if (symbolTable[hashValue] == NULL) {
    symbolTable[hashValue] = s;
  } else {
    symbol* temp = symbolTable[hashValue];
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = s;
  }
}

void createSymbol(char* name, int value) {
  symbol* s = malloc(sizeof(symbol));
  // name is copied and not assigned to avoid having to free
  // memory allocations on final unload
  strcpy(s->name, name);
  s->value = value;
  s->next = NULL;
  insertIntoTable(s);
}

void initializeSymbolTable() {
  for (int i = 0; i < 16; i++) {
    // regNameSource is static throughout each iteration,
    // so a dynamically allocated string is created to avoid
    // symbol names "sticking" throughout the loop
    char* regNameDest = malloc(4);
    char regNameSource[4];
    sprintf(regNameSource, "R%d", i);
    strcpy(regNameDest, regNameSource);
    createSymbol(regNameDest, i);
    free(regNameDest);
  }
  createSymbol("SP", 0);
  createSymbol("LCL", 1);
  createSymbol("ARG", 2);
  createSymbol("THIS", 3);
  createSymbol("THAT", 4);
  createSymbol("SCREEN", 16384);
  createSymbol("KBD", 24576);
}

int isInTable(char *name) {
  int hashValue = hash(name);
  symbol* temp = symbolTable[hashValue];
  while (temp != NULL) {
    if (strcmp(temp->name, name) == 0) {
      return 1;
    } 
    temp = temp->next;
  }
  return 0;
}

void getSymbolValue(char* key) {
  int hashValue = hash(key);
  symbol* temp = symbolTable[hashValue];
  while (temp != NULL && (strcmp(key, temp->name) != 0)) {
    temp = temp->next;
  }
  sprintf(key, "%d", temp->value);
}

void printSymbolTable() {
  for (int i = 0; i < BUCKETS; i++) {
    printf("Bucket %d: ", i);
    symbol* temp = symbolTable[i];
    while (temp != NULL) {
      printf("%s, %d -> ", temp->name, temp->value);
      temp = temp->next;
    }
    printf("\n");
  }
}

void unloadSymbolTable() {
  for (int i = 0; i < BUCKETS; i++) {
    if (symbolTable[i] != NULL) {
      symbol* temp = symbolTable[i];
      while (temp) {
        // Creates a new symbol to store temp->next in before temp is freed
        symbol* nextSymbol = temp->next;
        free(temp);
        temp = nextSymbol;
      }
    }
  }
}
