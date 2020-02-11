#include <stdlib.h>
#include <stdio.h>
#include <sequence.h>
#include <string.h>

Sequence* newSequence(unsigned char firstByte, unsigned long long hashSize) {
  Sequence* newSequence = malloc(sizeof(Sequence));

  newSequence->length = 1;
  newSequence->usage = 0;
  newSequence->code = 0;
  newSequence->hash = firstByte * 27;
  newSequence->bucket = (newSequence->hash % hashSize);
  newSequence->next = NULL;
  newSequence->data.bytes[0] = firstByte; // allocating every entry in bytes for
  newSequence->data.bytes[1] = 0;
  newSequence->data.bytes[2] = 0;         // no
  newSequence->data.bytes[3] = 0;
  newSequence->data.bytes[4] = 0;         // loss
  newSequence->data.bytes[5] = 0;
  newSequence->data.bytes[6] = 0;         // of data
  newSequence->data.bytes[7] = 0;

  return newSequence;
}

void deleteSequence(Sequence* sequence) {
  free(sequence);
}

Sequence* copySequenceAppending(Sequence* sequence, unsigned char newByte, unsigned long long hashSize) {
  Sequence* tempSequence = newSequence(sequence->data.bytes[0], hashSize);
  unsigned long long newHash = newByte;

  tempSequence->length = sequence->length + 1;

  int i = 0;
  while(sequence->data.bytes[i] != 0){            // find a free spot in the
                                                  // sequenes data.bytes for
                                                  // a new byte
    tempSequence->data.bytes[i] = sequence->data.bytes[i];
    newHash += tempSequence->data.bytes[i];
    i++;
  }

  tempSequence->data.bytes[i] = newByte;                 // add the new byte
  tempSequence->hash *= 27;                              // update the hash
  tempSequence->bucket = tempSequence->hash % hashSize;  // and the bucket

  return tempSequence;
}

unsigned int outputSequence(Sequence* sequence,
  void (*writeFunc)(unsigned char c, void* context),
  void* context) {
    int i = 0;

    while(sequence->data.bytes[i] != 0){           // for chars in data.bytes
      writeFunc(sequence->data.bytes[i], context); // print chars in data.bytes
      i++;
    }
    return i;
  }

  bool identicalSequences(Sequence* a, Sequence* b) {
    if(a->length != b->length) return false;        // make sure length is same

    int i = 0;
    while(a->data.bytes[i] != 0 && b->data.bytes[i] != 0) { // check each char
      if (a->data.bytes[i] != b->data.bytes[i]) return false;
      i++;
    }
    return true;
  }
