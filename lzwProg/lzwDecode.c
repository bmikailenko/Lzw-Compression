// Ben mikailenko

// program uses lzw algorithm to decompress compressed files

#include <sequence.h>
#include <bitStream.h>
#include <dict.h>
#include <lzw.h>
#include <stdlib.h>
#include <stdio.h>

// power works the same as pow(x, y), ex: power(2, 8) == 2^8 == 256
int power(unsigned int x, unsigned int y) {
	int result = 1;
  while (y != 0){
    result *= x;
    y--;
  }
  return result;
}

bool lzwDecode(unsigned int bits, unsigned int maxBits,
               int  (*readFunc )(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context) {
	int i = 0, MAX_BITS = (int)power(2,maxBits);
  BitStream* input = openInputBitStream(readFunc, context);
	BitStream* output = openOutputBitStream(writeFunc, context);
  Sequence** sequences = malloc(sizeof(Sequence *) * MAX_BITS); // Sequence* T[MAX_BITS]

  for(i = 0; i < MAX_BITS; i++) sequences[i] = 0; // initalize table

  for (i = 0; i < 256; i++) {											// add ascii 0-255 to table
    sequences[i] = newSequence(i, MAX_BITS);
    sequences[i]->code = i;
  }

  int nextIndex = 256;

  unsigned int code;		// code holds the int value of what we read in

	// read in x amount of bits based on bits into code
  if (readInBits(input, bits, &code) == false) return false; // nothing in file

	writeFunc(code, context);														// write code out to file

  if (nextIndex > (int)power(2, bits)-1) {						// update bits if needed
		++bits;																						// case: bits == 8
	}

  unsigned int previousCode = code;
	unsigned int c = 0;

  while(readInBits(input, bits, &code) != false) {     // while there is
																											 // something to be read
    int flag = 0;
    unsigned int currentCode = code;

    for (i = 0; i < nextIndex; i++) {                  // search table for
			if (currentCode == sequences[i]->code) flag = 1; // current code read
		}

    if (flag == 1) {																	 // if found
      c = sequences[currentCode]->data.bytes[0];			 // c = current code
    } else {																					 // if not
      c = sequences[previousCode]->data.bytes[0];			 // c = previous code
    }

		// if we can add more to the table
    if (nextIndex != MAX_BITS){

			// add to the table of sequences a new sequence w
			// w is the string "previousCode + c"
      Sequence* w = copySequenceAppending(sequences[previousCode],c,6000);
      sequences[nextIndex] = w;
      nextIndex++;

      if (nextIndex > (int)power(2, bits)) ++bits; // update bits if needed
    }

		// output current code
    unsigned int x = outputSequence(sequences[currentCode], writeFunc, context);
		output->byteCount += x;
    previousCode = currentCode;

  }

	// free memory
	for(i = 0; i < nextIndex; i++) deleteSequence(sequences[i]);
	free(sequences);
	free(input);
	free(output);


  return true;

}
