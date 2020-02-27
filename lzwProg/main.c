#include <stdlib.h>
#include <stdio.h>
#include <dict.h>
#include <lzw.h>
#include <sequence.h>
#include <unistd.h>
#include <fcntl.h>
#include <bitStream.h>

typedef struct _ioContext {
	FILE* input;
	FILE* output;
} IOcontext;

int readFunc(void* context) {
	return fgetc(((IOcontext*) context)->input);
}

void writeFunc(unsigned char c, void* context) {
	putc(c,((IOcontext*) context)->output);
}

int main(int argc, char * argv[]){

	IOcontext* io = malloc(sizeof(IOcontext));
	io->input = stdin;
	io->output = stdout;

	void *context = io;

	int x = lzwDecode(8, 16, readFunc, writeFunc, context);

	if (x == false) {
		printf("failed!!\n");
	}

	free(io);


	return 0;
}
