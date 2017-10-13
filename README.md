# One Hash Bloom Filter
A [Bloom Filter](https://en.wikipedia.org/wiki/Bloom_filter) is a type of probablistic
data structure that allows for testing set membership. They can be very space efficient, but the trade off is the 
possibility of false positives when performing a membership test. On the
 other hand, false negatives will never occur.

This is an implementation of a particular variation of a bloom fillter called
 the [One-Hashing Bloom Filter](https://www.researchgate.net/publication/284283336_One-Hashing_Bloom_Filter)'.
 A standard bloom filter is a bit array, and elements are mapped onto the array by calculating the output from _k_ hash
 functions modulo the size of the array, and setting the corresponding bits in the array to 1.
 As the name indicates, OHBF uses a single hash function, and instead logically splits the bit array
 into a number of uniquely sized, prime length partitions. Rather than _k_ hash functions mapping onto one array, a single
 hash output _h_ is calculated, and one bit in each partition _i_ is set by calculating _h_ % _partition_lengths[i]_.
 ## Usage
 A bloom filter can be parameterised on a combination of several variables depending on specific requirements; the number
 of elements _n_ the filter will hold, the target size of the bit array _m_, the target false
 probability rate _p_, and the number of partitions (hash functions in a regular filter) _k_. In this implementation,
 a filter is initialised with a chosen _n_ and _p_, and the optimal number of partitions
 and partitions sizes is calculated from those parameters.
 ````c
 typedef struct bloom {
   uint8_t *base_ptr;       
   uint8_t *bloom_ptr;      
   uint64_t size;   
   uint64_t total_size; 
   uint64_t *partition_lengths; 
   uint64_t num_partitions;
   uint8_t **partition_ptrs;
   double false_pos_rate;
   uint64_t prefix_len;
   uint64_t num_elems;
   uint64_t capacity;
   bool alloced;
 } bloom;
 
 // Initialisation function prototypes
 bloom *bloom_alloc(double p, uint64_t n, uint8_t *data, uint64_t prefix_len);
 int bloom_init(bloom *bf, double p, uint64_t n, uint8_t *data, uint64_t prefix_len);
 ````
 The `data` parameter can be used to pass in a pointer to the underlying array
 from an existing bloom filter to re-create the structure. It should be NULL
 when creating a new, empty filter. If a prefix length is specified, the amount in bytes will be allocated before
 the beginning of the actual filter array. This can be helpful if you need to transfer the filter
 (eg. over a network) and want to prepend a header of some kind without having to copy
 the entire filter.
 ## Initialisation
 ````c
 uint64_t n = 10000; // number of elements
 double p = 0.0001; // target false positive rate
 uint64_t prefix_len = 32;
 bloom bf;
 bloom_init(&bf, p, n, NULL, prefix_length);
 bloom *bf_ptr = bloom_alloc(p, n, NULL, prefix_length)
 // bf->base_ptr holds the address where the prefix begins
 // bf->bloom_ptr = bf->bloom_ptr + bf->prefix_len
 // bf->total_size = bf->size + bf->prefix_len
 ````
 Once created, there are essentially just the two operations, adding an element to the filter
 and testing for membership of an element.
 ````c
 uint64_t data_elem_size = 32;
 uint8_t *data = get_new_data();
 uint8_t *some_other_data = get_new_data();
 
 bloom_add_elem(&bf, data, data_elem_size);
 
 // always returns 0 as the element is in the set
 bloom_lookup(&bf, data, data_elem_size);
 
 // not in the set, so should return 1, but because of false positives
 // will return 0 with roughly 'p' probability
 bloom_lookup(&bf, some_other_data, data_elem_size);
 
 // check how much capacity is remaining in the filter.
 uint64_t remaining = bloom_remaining_capacity(&buf);
````
The `bloom_add_elem` function does not check whether a filter is at full capacity, so it's important
to ensure this does not happen as the user. Exceeding the capacity of a filter will dramatically increase
the rate of false positives.
## Cleanup
When using the clear/free functions, note that if an existing array is passed to the initialisation function,
then it will not be freed by the cleanup functions. So it is always safe to call clear on a filter initialised in that way,
regardless of the original source, but be sure to manually free/save a pointer to the memory before clearing the structure
or a memory leak may occur.
````c
bloom_clear(&bf);
// if the structure itself is dynamically allocated
bloom_free(bf_ptr);
// filter initialised from existing dynamically allocated array
uint8_t *data = bf->base_ptr;
free(data);
bloom_clear(&bf);