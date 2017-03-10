program structure
=================

head (32):
	0: noop (1)
	1: call main (2)
	2: jmp 0 (2)
	5: func_size (1)
	6-31 (blank)
func_head (variable):
  ---function_address_table---
	32: main address
	33: fn1 address
	34: fn2 address
	35: fn3 (etc. etc...)
  ---function_name_table---
	36: [4 m a i n]  (string 4+1)
	41: [3 f n 1]  	 (string 3+1)
	45: [3 f n 2]    (string 3+1)
	49: [3 f n 3]    (string 3+1)
data_block (variable):
  ---TOP_[53]---
	TOP+0: [2 5 h e l l o]  (data_type, str_length, str_characters...)
	TOP+7: [0 20]  			(data_type, value)
	TOP+9: [1 3 1 2 3] 		(data_type, arr_length, value...)

program:
	...
	

basic commands
==============

	- func <name>
	- call <name>
	- let <src> = <dst>
	- end { func }