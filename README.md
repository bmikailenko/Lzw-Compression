LZW Compression / Compressor

Program uses the lzw algorithm for lossless compression

Whats in here:

	LzwLib - creates a C linkable library
	LzwProg - creates a C executable

Note: Lzw prog creates and links a lib so the lib target in the makefile will need to be specified. Also, in main.c you must specify which function, decode or encode you want and how many bits. Bits should be equal when encoding and decoding the same file.

To make:

	make

to run:

	./main < (name of input file) > (name of output file)

to clean:

	make clean
