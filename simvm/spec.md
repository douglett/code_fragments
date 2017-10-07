simvm - simple minimalist vm
============================

architecture:
-------------
- memory (const int32 [0x10000])
- stack (int32 [0x10000])
- reg a-z (26 - int32)
- pc (int32 - constrained 0-0x9999)
- sp (int32 - constrained 0-0x9999)

commands:
---------
