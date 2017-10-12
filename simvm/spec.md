simvm - simple minimalist vm
============================

architecture:
-------------
the program can be interpreted from a string, or compiled. if the latter, program is stored in a const array.
- program (const int32 [0x10000])
- stack (int32 [0x1000])
- reg a-z (26 - int32)
- pc (int32 - constrained 0-0x9999)
- sp (int32 - constrained 0-0x999)
- external (all variables, arrays, screen. TBD)

commands:
---------
if not interpreted, each command is 3 ints long. shorter commands are padded with 0.
if an error is encountered, the program dies. an error code is put in register [w]. registers [w]-[z] are special registers. [w]-[z] are persistant, and are not affected by pushstate.
- set a (1, b)
- add a (1, b)
- sub, mul, div, mod - (error: divide-by-zero)
- jump mylabel - (set pc to label - value only, hardcoded. error: out-of-range)
- :mylabel - (label)
- cmp a (b, 2) - (comparison based on register val: x = query, y = result)
	- q :: ![0] =[1] >=[2] >[3] <=[4] <[5] fail[...]
- if a (b, 2) - (runs next line if equal - next line must be jump)
- pushstate - (push entire state onto stack (32 ints): [a-v(22), pc(27), pad-0]. error: stack-overflow)
- popstate  - (pop state from stack, re-setting each value as above. this will move pc. error: stack-underflow)
- defseg (1, a) - (set memory address for peek and poke. magic numbers explained below) 
- peek a (0, b) - (peek from memory address at offset [b] into register [a]. error: unreadable memory)
- poke a (0, b) - (poke from register [a] into memory at offset [b]. error: unwritable memory)

segments:
---------
peek and poke into these segments.
peek -1 to get memory length. peek/poke -2 for control codes.
the following basic segments are guaranteed to exist.
- 0 :: program (default) - read only program data
- 1 :: stdout - 256 int character buffer, to be written to the screen
- 2 :: stderr - as above
- 3 :: stdin - as above. contains raw keyboard input
- 4 :: scratch - 0x1000 (4k) ints or basic data. can save small amounts of generic program data here without allocating.
- 5 :: screenbuffer - raw pixel data
- 6-9 :: reserved
- 10 :: system memory starts here
