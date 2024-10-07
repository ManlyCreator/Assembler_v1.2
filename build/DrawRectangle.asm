// n = R0
// addr = SCREEN
// for (i = 0; i < n; i++) {
//  RAM[addr] = -1
//  addr += 32
// }

@R0
D=M
@n
M=D // n = R0

@SCREEN
D=A
@addr
M=D // addr = SCREEN

@i
M=0 // i = 0

(LOOP)
// n - i == 0 ? 
@n
D=M
@i
D=D-M
@END
D;JEQ

@addr
A=M
M=-1 // RAM[addr] = -1

@32
D=A
@addr
D=D+M
M=D // addr += 32

@i
M=M+1 // i++

@LOOP
0;JMP

(END)
@END
0;JMP

