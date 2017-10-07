simvm - simple minimalist vm
============================

architecture:
-------------
the program can be interpreted from a string, or compiled. if the latter, program is stored in a const array.
- program (const int32 [0x10000])
- stack (int32 [0x10000])
- reg a-z (26 - int32)
- pc (int32 - constrained 0-0x9999)
- sp (int32 - constrained 0-0x9999)
- external (all variables, arrays, screen. TBD)

commands:
---------
if not interpreted, each command is 3 ints long. shorter commands are padded with 0.
if an error is encountered, the program dies. an error code is put in register w.
- set a (1, b)
- add a (1, b)
- sub, mul, div, mod - (error: divide-by-zero)
- jump mylabel - (set pc to label - value only, hardcoded. error: out-of-range)
- :mylabel - (label)
- cmp (a, 1) (b, 2) - (comparison based on register val: q = query, r = result)
	- q :: ![0] =[1] >=[2] >[3] <=[4] <[5] fail[...]
- if (a, 1) (b, 2) - (runs next line if equal - next line must be jump)
	- (removed) push (a, 1) - (put value on stack)
	- (removed) pop a - (pop stack value)
- pushstate - (push entire state onto stack (32 ints): [a-z(26), pc(27), pad-0]. error: stack-overflow)
- popstate  - (pop state from stack, re-setting each value as above. this will move pc. error: stack-underflow)
- load a - (load from external memory. TBD)
- save - (TBD)
