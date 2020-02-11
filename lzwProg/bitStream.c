#include <stdlib.h>
#include <bitStream.h>
#include <unistd.h>
#include <stdio.h>

BitStream* openInputBitStream(int (*readFunc)(void* context), void* context) {
  BitStream* tempBitStream = malloc(sizeof(BitStream));

  tempBitStream->readFunc = readFunc;
  tempBitStream->context = context;
  tempBitStream->direction = 1;
  tempBitStream->extraCount = 0;
  tempBitStream->extraBits = 0;
  tempBitStream->byteCount = 0;

  return tempBitStream;
}

BitStream* openOutputBitStream(void (*writeFunc)(unsigned char c,void* context),void* context) {
  BitStream* tempBitStream = malloc(sizeof(BitStream));

  tempBitStream->writeFunc = writeFunc;
  tempBitStream->context = context;
  tempBitStream->direction = 0;
  tempBitStream->extraCount = 0;
  tempBitStream->extraBits = 0;
  tempBitStream->byteCount = 0;

  return tempBitStream;
}

void closeAndDeleteBitStream(BitStream* bs) {
  flushOutBits(bs);
  free(bs);
}

void flushOutBits(BitStream* bs){
  unsigned char c = 0;
  int i, k;
  unsigned int extraBits[bs->extraCount], results[8];

  for (i = 0; i < bs->extraCount; i++){          // extraBits[extraCount] = 0
    extraBits[i] = 0;
  }

  for (i = 0; i < 8; i++){                       // results[8] = 0
    results[i] = 0;
  }

  for (i = 0; bs->extraBits > 0; i++) {                   // convert BITSTREAMS EXTRA BITS to binary and
    extraBits[bs->extraCount-i-1] = bs->extraBits % 2;    // place into the FUNCTIONS'S EXTRA BITS array
    bs->extraBits = bs->extraBits / 2;
  }

  for (i = bs->extraCount-1; i >= 0; i--) {               // Pass binary value of extraBits
    results[7-i] = extraBits[i];                          // into results array
  }

                                      // printing each byte in code
  k = 0;                              // counts 8 bits
  for (i = 7; i >= 0; i--) {          // for every bit
    c *= 2;                           // convert binary to decimal into c
    if (results[i] == 1) c++;         //
    k++;                              //
    if (k == 8) {                     // if you made a byte
      bs->writeFunc(c, bs->context);  // output the byte
    }
  }
}

void outputBits(BitStream* bs, unsigned int nBits, unsigned int code) {
  unsigned char c = 0;
  int i, k;
  unsigned int extraBits[bs->extraCount];

  for (i = 0; i < bs->extraCount; i++)        // extraBits[extraCount] = 0
    extraBits[i] = 0;

  for (i = 0; bs->extraBits > 0; i++) {       // convert bs->extraBits to binary
    extraBits[i] = bs->extraBits % 2;         // into the functions extraBits
    bs->extraBits = bs->extraBits / 2;
  }

  unsigned int result[nBits+bs->extraCount];  // result array for all bits

  k = bs->extraCount - 1;
  for(i = nBits + bs->extraCount - 1; nBits<= i; i--){  // place extraBits into
    result[i] = extraBits[k];                           // most significant bits
    k--;                                                // of result array
  }

  for(i = nBits - 1; 0 <= i; i--) {  // place code bits into least significant
    result[i] = (unsigned int)((code >> i) & 0x01); // of result array
  }

                                     // printing each byte in code
  k = 0;                             // counts 8 bits
  for (i = nBits + bs->extraCount - 1; i >= 0; i--) { // for every bit
    c *= 2;                          // convert binary to decimal into c
    if (result[i] == 1) c++;         //
    k++;                             //
    if (k == 8) {                    // if you made a byte
      bs->writeFunc(c, bs->context);        // output the byte
      c = 0;                         // reset c
      k = 0;                         // reset count
    }
  }
  if (k != 0) {          // if there are leftover bits
    bs->extraBits = c;             // save leftover bits
    bs->extraCount = k;            // to bs
  }
}

/* Read an nBit code from fileStream, if EOF, return false,
else write the code to the pointer argument and return true. */
bool readInBits(BitStream* bs, unsigned int nBits, unsigned int* code) {
  unsigned int bitsRead = bs->extraCount;
  int read;
  int k = 0;
  *code = 0;      // we are writing directly to code in this function

  for(int i = 1; i <= bs->extraCount; i++){ // for each bit in extraBits

    int mask = 1 << (bs->extraCount-i);     // get the bit value

    if (bs->extraBits & mask)                       // and place it into most
      *code = *code | mask << (nBits - bitsRead);   // significant byte of *code

  }

  read = bs->readFunc(bs->context);        // read in 8 bits
  if (read < 0) return false;
  bitsRead += 8;                           // update count

  for(int i = 1; i <= nBits - bs->extraCount; i++){

    k++;                                      // (k shifts our mask)

    if (k == 9 && i != 0) {                   // if nBits > bitsRead

      read = bs->readFunc(bs->context);       // read in 8 bits

      if (read < 0) return false;             // return false if EOF

      bitsRead += 8;                          // update counts
      k = 1;                                  //

    }

    int shift = bitsRead - nBits;  // (shift shifts where our *code bit looks)

    int mask = 1 << (8-k);         // we do (read & mask) to see which bit == 1
                                   // ex: 00010000 to access 5th bit
                                   // mask goes from most significant bits
                                   // to least
                                   // ex:
                                   // 1000 0000 -> 0100 0000 -> 0010 0000


    if (read & mask) {             // see if nth bit of read == mask
                                   // ex:
                                   // mask:   0010 0000
                                   // read:   1110 0100 (ascii: 't')
                                   //         TRUE


                                  // this next secion selects which *code bit
                                  // is changed based on nBits
                                  // and how many bits were read (bitsRead)

                                  // a 1 is placed in the selected *code bit
                                  // when the read and mask bits align

      if (shift == 0) {           // case 1: we need 8 bits and we read 8 bits
                                  // no need to shift

        *code = *code | mask;     // mask:   0010 0000
                                  // read:   1110 0100 (ascii: 't')
                                  // code:   1110 0000
                                  // (bit 6 of *code went from 0 -> 1 here)
      }

                                  // cases 2 and 3 are chosen based on the
                                  // sign of shift

      if (shift > 0) {                     // case 2: shift right

        *code = *code | mask  >> (shift);
      }
      if (shift < 0) {                     // case 3: shift left

        shift *= -1;
        *code = *code | mask  << (shift);
      }
    }

  }

  bs->extraCount = (bitsRead - nBits);    // update the extra-bits-count we have
  bs->extraBits = 0;                      // clear previous extra bits

  for(int i = 1; i <= bs->extraCount; i++){

    k++;

    int mask = 1 << (8-k);

    if (read & mask)
      bs->extraBits = bs->extraBits | mask;  // and update the extra bits

  }

  return true;

}
