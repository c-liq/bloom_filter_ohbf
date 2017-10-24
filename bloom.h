#ifndef BLOOM_OHBF_H
#define BLOOM_OHBF_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xxhash.h"

typedef struct bloom {
  uint8_t *base_ptr;
  uint8_t *bloom_ptr;
  uint64_t size;
  uint64_t *partition_lengths;
  uint64_t num_partitions;
  uint8_t **partition_ptrs;
  double false_pos_rate;
  uint64_t total_size;
  uint64_t prefix_len;
  uint64_t num_elems;
  uint64_t capacity;
  bool alloced;
} bloom;

bloom *bloom_alloc(double p, uint64_t n, uint8_t *data, uint64_t prefix_len);

int bloom_init(bloom *bf, double p, uint64_t n, uint8_t *data, uint64_t prefix_len);

void bloom_clear(bloom *bf);

void bloom_free(bloom *bf);

uint8_t *bloom_get_filter(bloom *bf);

uint8_t *bloom_get_prefix(bloom *bf);

int bloom_add(bloom *bf, uint8_t *data, uint64_t data_len);

int bloom_test(bloom *bf, uint8_t *data, uint64_t data_len);

void bloom_print(bloom *bf);

uint64_t bloom_remaining_capacity(bloom *bf);

#endif  // BLOOM_OHBF_H
