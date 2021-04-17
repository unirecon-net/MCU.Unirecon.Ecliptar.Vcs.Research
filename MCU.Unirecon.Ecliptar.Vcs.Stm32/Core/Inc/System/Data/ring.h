#ifndef _SYSTEM_DATA_RING_H
#define _SYSTEM_DATA_RING_H

#include <string.h>
#include <inttypes.h>
#include <typeinfo>

#define RING_VOLATILE                       volatile

#define RING_MARKS_MEM                      0

typedef struct
{
#if RING_MARKS_MEM
	uint32_t startMark = 0;
#endif /* RING_MARKS_MEM */
	uint8_t *buffer = NULL;
	uint8_t *readBuffer = new uint8_t[0];
	uint8_t readedByte = 0;
	size_t size = 0;
	size_t readPtr = 0;
	size_t writePtr = 0;
	uint8_t isActive = 0;
#if RING_MARKS_MEM
	uint32_t endMark = 0;
#endif /* RING_MARKS_MEM */
} RingStructure;


class Ring
{
public:
	~Ring() { delete[] Data.readBuffer; }
	Ring();
	Ring(size_t size);
	Ring(void *buffer, size_t size);

	void* operator[](size_t index);
	const void* operator[](size_t index) const;

	RING_VOLATILE
	RingStructure Data;

	uint8_t 	Init(void *buffer, size_t size);
	uint8_t 	IsReady();
	void 		Free();
	void 		Reset();

	uint8_t 	Available();

	size_t 		Write(RING_VOLATILE const void *data, size_t btw);

	size_t		Rewrite(Ring* destRing, size_t btc);
	size_t		Rewrite(Ring* destRing);

	size_t		Overrewrite(Ring* destRing, size_t btc);
	size_t		Overrewrite(Ring* destRing);

	size_t 		Overwrite(RING_VOLATILE const void *data, size_t btw);

	uint8_t		Read();
	uint8_t		ReadedByte();
	size_t 		Read(void *data, size_t btr);

	void*		Read(size_t btr);
	void* 		ReadAll();

	size_t 		Peek(size_t skip_count, void *data, size_t btp);
	void* 		Peek(size_t skip_count, size_t btp);
	void* 		PeekAll();
	void* 		PeekLast(size_t btp);
	void* 		PeekFirst(size_t btp);

	size_t 		FreeSize();
	size_t 		FilledSize();
	size_t 		FullSize();

	void* 		ReadAddress();
	size_t 		ReadLinearlength();
	size_t 		Skip(size_t len);

	void* 		WriteAddress();
	size_t 		WriteLinearlength();
	size_t 		Step(size_t len);

	void*		StartAddress();

};
#endif /* _SYSTEM_DATA_RING_H */
