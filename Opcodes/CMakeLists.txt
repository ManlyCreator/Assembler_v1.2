# Specifies library source
add_library(Opcodes src/opcodes.c)
# Specifies include directory for library
target_include_directories(Opcodes PUBLIC include)
