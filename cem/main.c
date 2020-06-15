/*
 * Original source: https://github.com/CMUAbstract/alpaca-oopsla2017/
 * Modified 2020 for use with ICLib by Sivert Sliper, University of Southamption
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "iclib/ic.h"
#include "support/support.h"

/* ------ Parameters ------ */
#define DICT_SIZE 512
#define BLOCK_SIZE 64

#define NUM_LETTERS_IN_SAMPLE 2
#define LETTER_MASK 0x00FF
#define LETTER_SIZE_BITS 8
#define NUM_LETTERS (LETTER_MASK + 1)

/* ------ Types ------ */
#define NIL 0  // like NULL, but for indexes, not real pointers
typedef unsigned index_t;
typedef unsigned letter_t;
typedef unsigned sample_t;

typedef struct _node_t {
  letter_t letter;  // 'letter' of the alphabet
  index_t sibling;  // this node is a member of the parent's children list
  index_t child;    // link-list of children
} node_t;

typedef struct _dict_t {
  node_t nodes[DICT_SIZE];
  unsigned node_count;
} dict_t;

typedef struct _log_t {
  index_t data[BLOCK_SIZE];
  unsigned count;
  unsigned sample_count;
} log_t;

/* ------ Globals ------ */

/* ------ Function prototypes ------ */

//! Initialize dictionary
void init_dict(dict_t *dict);

//! Acquire sample (spoofed as pseudo-random number)
static sample_t acquire_sample(letter_t prev_sample);

//! Sample temperature or letterize
void sample();

//! Take new measurement and update previous sample
void measure_temp();

//! Convert current sample (_v_sample) to letter (_v_letter)
void letterize();

//! Compress?
void compress();

//! Find child
index_t find_child(letter_t letter, index_t parent, dict_t *dict);

//! Add a node to the dictionary
void add_node(letter_t letter, index_t parent, dict_t *dict);

//! Insert new node in compressed output
void add_insert();

//! Append node to compressed output
void append_compressed(index_t parent, log_t *log);

/* ------ Function definitions ------ */

static sample_t acquire_sample(letter_t prev_sample) {
  // letter_t sample = rand() & 0x0F;
  letter_t sample = (prev_sample + 1) & 0x03;
  return sample;
}

void init_dict(dict_t *dict) {
  dict->node_count = 0;
  for (letter_t l = 0; l < NUM_LETTERS; ++l) {
    node_t *node = &dict->nodes[l];
    node->letter = l;
    node->sibling = 0;
    node->child = 0;
    dict->node_count++;
  }
}

index_t find_child(letter_t letter, index_t parent, dict_t *dict) {
  node_t *parent_node = &dict->nodes[parent];
  if (parent_node->child == NIL) {
    return NIL;
  }

  index_t sibling = parent_node->child;
  while (sibling != NIL) {
    node_t *sibling_node = &dict->nodes[sibling];
    if (sibling_node->letter == letter) {  // found
      return sibling;
    } else {  // Continue traversing
      sibling = sibling_node->sibling;
    }
  }

  // Not found
  return NIL;
}

void add_node(letter_t letter, index_t parent, dict_t *dict) {
  if (dict->node_count == DICT_SIZE) {
    while (1)
      ;  // ERROR
  }

  // Initialize the new node
  node_t *node = &dict->nodes[dict->node_count];

  node->letter = letter;
  node->sibling = NIL;
  node->child = NIL;

  index_t node_index = dict->node_count++;

  index_t child = dict->nodes[parent].child;

  if (child) {
    // Find the last sibling in list
    index_t sibling = child;
    node_t *sibling_node = &dict->nodes[sibling];
    while (sibling_node->sibling != NIL) {
      sibling = sibling_node->sibling;
      sibling_node = &dict->nodes[sibling];
    }

    // Link-in the new node
    dict->nodes[sibling].sibling = node_index;
  } else {
    // Only child
    dict->nodes[parent].child = node_index;
  }
}

void append_compressed(index_t parent, log_t *log) {
  log->data[log->count++] = parent;
}

int main() {
  static dict_t dict;
  static log_t log;

  for (volatile unsigned i = 0; i < 3; i++) {
    init_dict(&dict);

    // Initialize the pointer into the dictionary to one of the root nodes
    // Assume all streams start with a fixed prefix ('0'), to avoid having
    // to letterize this out-of-band sample.
    letter_t letter = 0;

    unsigned letter_idx = 0;
    index_t parent, child;
    sample_t sample, prev_sample = 0;

    log.sample_count = 1;  // count the initial sample (see above)
    log.count = 0;         // init compressed counter

    indicate_workload_begin();
    while (log.count < BLOCK_SIZE) {
      child = (index_t)letter;
      if (letter_idx == 0) {
        sample = acquire_sample(prev_sample);
        prev_sample = sample;
      }

      letter_idx++;
      if (letter_idx == NUM_LETTERS_IN_SAMPLE) letter_idx = 0;
      do {
        unsigned letter_idx_tmp =
            (letter_idx == 0) ? NUM_LETTERS_IN_SAMPLE : letter_idx - 1;

        unsigned letter_shift = LETTER_SIZE_BITS * letter_idx_tmp;
        letter = (sample & (LETTER_MASK << letter_shift)) >> letter_shift;

        log.sample_count++;
        parent = child;
        child = find_child(letter, parent, &dict);
      } while (child != NIL);

      append_compressed(parent, &log);
      add_node(letter, parent, &dict);
    }
    indicate_workload_end();
    wait();
  }
  end_experiment();
  return 0;
}
