/*
 * ecliptar_sys_hashcode.h
 *
 *  Created on: 24 mar 2021
 *      Author: merlin
 */

#ifndef _SYSTEM_HASH_H_
#define _SYSTEM_HASH_H_

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "System/Hash/hash32.h"
#include "System/Hash/hash64.h"


uint32_t hashcode32(void* input, size_t size);

uint64_t hashcode64(void* input, size_t size);

uint32_t hashtableId(uint32_t hash, size_t size);

#endif /* _SYSTEM_HASH_H_ */
