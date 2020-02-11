#include <stdlib.h>
#include <dict.h>
#include <stdio.h>
#include <string.h>
#include <sequence.h>

Dict* newDict(unsigned long long hashSize) {
  Dict *newDict = malloc(sizeof(Dict));

  newDict->hashSize = hashSize;
  newDict->count = 0;
  newDict->searches = 0;
  newDict->table = malloc(sizeof(Sequence *) * hashSize); // Sequence* table[hashSize]

  for(int i = 0; i < newDict->hashSize; i++) newDict->table[i] = 0; // init

  return newDict;

}

void deleteDictDeep(Dict* dict) {
  int i;
  Sequence *s, *next;

  for(i = 0; i < dict->hashSize; i++) {             // for the whole table
    for(s = dict->table[i]; s != 0; s = next) {     // at each entry
      next = s->next;                               // traverse linked lists

      deleteSequence(s);                            // free the sequences
    }
  }

  free(dict->table);                                // free the table
  free(dict);                                       // free the dict

}

bool searchDict(Dict* dict, Sequence* key) {
  dict->searches++;

  Sequence* searchArea = dict->table[key->bucket];

  if (searchArea == NULL) return false;                 // nothing at table[key]

  if (identicalSequences(searchArea, key)) return true; // key == table[key]

  while (searchArea->next != NULL) {                    // traverse linked list
    searchArea = searchArea->next;                      // at table[key]
    if (identicalSequences(searchArea, key)) return true;   //true
  }

  return false;                                             // false
}

void insertDict(Dict* dict, Sequence* key, unsigned int code) {
  key->code = code;

  if (dict->table[key->bucket] != NULL) {           // nothing at table[key]
    dict->table[key->bucket] = key;                 // table[key] = key
  } else {
    Sequence* oldKey = dict->table[key->bucket];    // add key
    dict->table[key->bucket] = key;                 // to table[key]'s
    dict->table[key->bucket]->next = oldKey;        // linked list
  }

  dict->count++;

  return;
}
