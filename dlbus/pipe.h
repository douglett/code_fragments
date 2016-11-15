#pragma once

extern "C" {
	const    int     PIPE_LEN = 255;
	typedef  struct  {  int l;  const char* c;  }  PIPE_t;
	typedef  void    (*PIPE_in_t ) (PIPE_t);
	typedef  PIPE_t  (*PIPE_out_t) ();
	typedef  int     (*PIPE_sig_t) ();
}