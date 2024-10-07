#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define SYMBOL_SIZE 256

typedef struct symbol {
  char name[SYMBOL_SIZE];
  int value;
  struct symbol* next;
} symbol;

int hash(char* name);
void createSymbol(char* name, int value);
void initializeSymbolTable();
void insertIntoTable(symbol* s);
int isInTable(char* name);
void getSymbolValue(char* key);
void printSymbolTable();
void unloadSymbolTable();

#endif
