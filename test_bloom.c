#include <string.h>
#include <utime.h>
#include <sys/random.h>
#include "bloom.h"

#define test_num_elems 1000UL
#define test_num_lookups 9000000ULL
#define key_size 32U

int get_random(void *buf, size_t count) {
    if (count <= 0) {
        return 0;
    }

    while (count > 0) {
        size_t to_read = count > 256 ? 256 : count;
        ssize_t read = getrandom(buf, to_read, 0);
        if (read < 0) {
            return -1;
        }
        count -= read;
        buf += read;
    }
    return 0;
}

bloom *test_bf_setup(double p)
{
	bloom *bf = bloom_alloc(p, test_num_elems, NULL, 0);
	if (!bf) {
		fprintf(stderr, "Fatal calloc error\n");
		exit(EXIT_FAILURE);
	}
	return bf;
}

int test_bloom_add(bloom *bf, uint8_t *data_array, uint32_t elem_size, uint32_t num_elems)
{
	uint8_t *data_ptr = data_array;
	for (uint32_t i = 0; i < num_elems; i++) {
		bloom_add(bf, data_ptr, elem_size);
		data_ptr += elem_size;
	}
    return 0;
}

int test_bloom_lookup(bloom* bf, uint8_t* data_array, uint32_t elem_size, uint32_t num_elems, char* msg)
{
	uint8_t *data_ptr = data_array;
	long pos = 0;
	for (uint32_t i = 0; i < num_elems; i++) {
		pos += !bloom_test(bf, data_ptr, elem_size);
		data_ptr += elem_size;
	}
	printf("Number of lookups on %s: %u | Positive: %ld | Pos rate: %f\n", msg, num_elems, pos, (double) pos / num_elems);
		return 0;
}

uint8_t *test_generate_data(uint32_t elem_size, uint32_t num_elems)
{
	uint8_t *data_array = calloc(num_elems, elem_size);
	if (!data_array) {
		fprintf(stderr, "fatal calloc error\n");
		exit(EXIT_FAILURE);
	}

	get_random(data_array, num_elems * elem_size);
	return data_array;
}

int main(int argc, char **argv)
{
    bloom *bf = test_bf_setup(0.01);
	uint8_t* data = test_generate_data(key_size, test_num_elems);
	uint8_t* false_lookup_data = test_generate_data(key_size, test_num_lookups);
    bloom_print(bf);
    test_bloom_add(bf, data, key_size, test_num_elems);
    test_bloom_lookup(bf, data, key_size, test_num_elems, "Real data");
    test_bloom_lookup(bf, false_lookup_data, key_size, test_num_lookups, "Fake data");
    bloom_free(bf);
    free(data);
    free(false_lookup_data);

    return 0;
}


