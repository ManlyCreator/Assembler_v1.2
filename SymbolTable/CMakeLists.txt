# Specifies library source
add_library(SymbolTable src/symbolTable.c)
# Specifies include directory for library
target_include_directories(SymbolTable PUBLIC include)
