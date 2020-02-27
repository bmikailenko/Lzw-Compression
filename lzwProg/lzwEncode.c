// Ben Mikailenko

// program uses lzw algorithm to compress files

#include <sequence.h>
#include <bitStream.h>
#include <dict.h>
#include <lzw.h>
#include <stdlib.h>
#include <stdio.h>

// power works the same as pow(x, y), ex: power(2, 8) == 2^8 == 256
unsigned int power(unsigned int x, unsigned int y) {
	unsigned int result = 1;
  while (y != 0){
    result *= x;
    y--;
  }
  return result;
}

bool lzwEncode(unsigned int bits, unsigned int maxBits,
  int (*readFunc)(void* context),
  void (*writeFunc)(unsigned char c, void* context),
  void* context) {
    BitStream* input = openInputBitStream(readFunc, context);
    BitStream* output = openOutputBitStream(writeFunc, context);
		Dict* dict = newDict(6000);     // init directory D

    for (int i = 0; i < 256; i++) { // insert ascii 0-255 as single char bits
      insertDict(dict, newSequence(i, dict->hashSize), i);
    }

    int nextCode = 256;

    unsigned int code; // code holds the int value of what we read in

		// read a byte into code
    if (readInBits(input, 8, &code) == false) return false; // case: empty file

    Sequence* w = newSequence(code, 6000);

    while(readInBits(input, 8, &code) != false) {				// while there is
																											  // something to be read
      unsigned int c = code;

			// x is the string (sequence) w + (char) c
      Sequence* x = copySequenceAppending(w, c, 6000);

      if (searchDict(dict, x) == true) {				// if x is in the dictionary,
																								// w = x
				w = x;
				// free(w);																// boring data swap...
        // w = newSequence(x->code,6000);
				// w->data.bytes[0] = x->data.bytes[0];
				// w->data.bytes[1] = x->data.bytes[1];
				// w->data.bytes[2] = x->data.bytes[2];
				// w->data.bytes[3] = x->data.bytes[3];
				// w->data.bytes[4] = x->data.bytes[4];
				// w->data.bytes[5] = x->data.bytes[5];
				// w->data.bytes[6] = x->data.bytes[6];
				// w->data.bytes[7] = x->data.bytes[7];
				// free(x);																// and go to begenning of loop

      } else {																	// if x is not in dictionary

				// find w in the dictionary
        Sequence* tempSequence = dict->table[w->bucket];
        while (identicalSequences(tempSequence, w) != true)
          tempSequence = tempSequence->next;

				// and print it out
        if (nextCode == (int)power(2, bits) + 1){ // (make sure nextCode
																									// isn't greater then 2^bits)
          bits++;
          outputBits(output, bits, tempSequence->code);
        } else {
          outputBits(output, bits, tempSequence->code);
        }

        if (nextCode != (int)power(2, maxBits)) {
          insertDict(dict, x, nextCode);				// add x to the dictionary now
          nextCode++;
        }
				//free(w);
        w = newSequence(c, 6000);		 // make w so that it holds just code now
      }
    }

		// find the last value left
    Sequence* tempSequence = dict->table[w->bucket];
    while (identicalSequences(tempSequence, w) != true)
      tempSequence = tempSequence->next;
		// and print it
		outputBits(output, bits, tempSequence->code);

		// free data
    flushOutBits(output);
		free(input);
		free(output);
		deleteDictDeep(dict);
		free(w);

    return true;
  }
