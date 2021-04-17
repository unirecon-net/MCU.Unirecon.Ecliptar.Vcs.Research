#include "System/hash.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

Hash32 hasher32(0);
uint32_t hashcode32(void* input, size_t size)
{
	hasher32.init();
	hasher32.add(input, size);
	return hasher32.hash();
}

Hash64 hasher64(0);
uint64_t hashcode64(void* input, size_t size)
{
	hasher64.init();
	hasher64.add(input, size);
	return hasher64.hash();
}

uint32_t hashtableId(uint32_t hash, size_t size)
{
	return hash % size;
}

