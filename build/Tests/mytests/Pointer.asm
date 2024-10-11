// D = 23
@23
D=A

// RAM[257] = 23
// D = 257
@257
M=D
D=A

// RAM[p] = 257
@p
M=D

// D = RAM[p] = 257
D=M// FUCK
// A = 257
A=D
// D = RAM[257] 
D=M

// RAM[0] = RAM[257]
@R0
M=D

(END)
@END
0;JMP
