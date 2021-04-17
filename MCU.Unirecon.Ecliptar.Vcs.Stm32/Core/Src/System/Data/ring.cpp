#include "System/Data/ring.h"

#define RING_MEMSET                   memset
#define RING_MEMCPY                   memcpy

#define RING_START_MARK             	0xACEFADED
#define RING_END_MARK                	~0xACEFADED

#if RING_MARKS_MEM
#define RING_IS_VALID               	(Data.startMark == RING_START_MARK && Data.endMark == RING_END_MARK && Data.isActive)
#else
#define RING_IS_VALID               	(Data.isActive)
#endif /* RING_MARKS_MEM */
#define RING_MIN(x, y)              	((x) < (y) ? (x) : (y))
#define RING_MAX(x, y)              	((x) > (y) ? (x) : (y))

Ring::Ring()
{
	if(Init(new uint8_t[255], 255))
		Data.isActive = 1;
}
Ring::Ring(size_t size)
{
	if(Init(new uint8_t[size], size))
		Data.isActive = 1;
}
Ring::Ring(void *buffer, size_t size)
{
	if(Init(buffer, size))
		Data.isActive = 1;
}

void* 		Ring::operator[](size_t index)
{
	size_t full, id, r;
	full = FilledSize();
	if (full < 1)
		return NULL;

	id = RING_MIN(index, full - 1);

	r = Data.readPtr;
	r += id;

	if (r >= Data.size)
	{
		r -= Data.size;
	}

	return &Data.buffer[r];
}

uint8_t 	Ring::Init(void *buffer, size_t size)
{
	if (buffer == NULL || size == 0)
	{
		return 0;
	}

	size = RING_MIN(sizeof(buffer), size);

	RING_MEMSET(buffer, 0x00, sizeof((uint8_t*) buffer));

	Data.size = size;
	Data.buffer = (uint8_t*) buffer;

#if RING_MARKS_MEM
	Data.startMark = RING_START_MARK;
	Data.endMark = RING_END_MARK;
#endif /* RING_MARKS_MEM */

	return 1;
}
uint8_t 	Ring::IsReady()
{
	return RING_IS_VALID;
}
void 		Ring::Free()
{
	if (RING_IS_VALID)
	{
		Data.buffer = NULL;
		delete[] Data.readBuffer;
	}
}

uint8_t 	Ring::Available()
{
	if (RING_IS_VALID)
		return Data.readPtr != Data.writePtr;
	return 0;
}

size_t 		Ring::Write(RING_VOLATILE const void *data, size_t btw)
{
	size_t tocopy;
	if (data == NULL)
	{
		return 0;
	}

	const uint8_t *d = (uint8_t*) data;

	/* Calculate maximum number of bytes available to write */
	btw = RING_MIN(FreeSize(), btw);
	if (btw == 0)
	{
		return 0;
	}

	/* Step 1: Write data to linear part of data */
	tocopy = RING_MIN(Data.size - Data.writePtr, btw);
	RING_MEMCPY(&Data.buffer[Data.writePtr], d, tocopy);
	btw -= tocopy;

	/* Step 2: Write data to beginning of data (overflow part) */
	if (btw > 0)
	{
		RING_MEMCPY(Data.buffer, &d[tocopy], btw);
		Data.writePtr = btw;
	}
	else
	{
		Data.writePtr += tocopy;
		/* Step 3: Check end of data */
		if (Data.writePtr >= Data.size)
		{
			Data.writePtr = 0;
		}
	}

	return tocopy + btw;
}

size_t 		Ring::Rewrite(Ring *destRing, size_t btc)
{
	size_t dll, dwp, srte, srfs = 0, dsz;
	btc = RING_MIN(btc, destRing->FreeSize());
	if (btc == 0)
	{
		return 0;
	}

	dsz = destRing->Data.size;
	dwp = destRing->Data.writePtr;
	dll = RING_MIN(dsz - dwp, btc);
	srte = Read(&destRing->Data.buffer[dwp], dll);
	dll -= srte;
	btc -= srte;

	if (btc > 0 && dll < 1)
	{
		srfs = Read(destRing->Data.buffer, btc);
		dwp = srfs;
	}
	else
	{
		dwp += srte;
		if (dwp >= dsz)
		{
			dwp = 0;
		}

	}
	destRing->Data.writePtr = dwp;

	return srte + srfs;
}
size_t 		Ring::Rewrite(Ring *destRing)
{
	return Rewrite(destRing, FilledSize());
}

size_t 		Ring::Overrewrite(Ring *destRing, size_t btc)
{
	size_t dll, dwp, srte, srfs = 0, dsz, skip, free;
	if (btc == 0)
	{
		return 0;
	}
	free = destRing->FreeSize();
	skip = btc - free;
	if (skip > 0)
	{
		free += destRing->Skip(skip);
	}
	btc = RING_MIN(btc, free);

	dsz = destRing->Data.size;
	dwp = destRing->Data.writePtr;
	dll = RING_MIN(dsz - dwp, btc);
	srte = Read(&destRing->Data.buffer[dwp], dll);
	dll -= srte;
	btc -= srte;

	if (btc > 0 && dll < 1)
	{
		srfs = Read(destRing->Data.buffer, btc);
		dwp = srfs;
	}
	else
	{
		dwp += srte;
		if (dwp >= dsz)
		{
			dwp = 0;
		}
	}
	destRing->Data.writePtr = dwp;

	return srte + srfs;
}
size_t 		Ring::Overrewrite(Ring *destRing)
{
	return Overwrite(destRing, FilledSize());
}

size_t 		Ring::Overwrite(RING_VOLATILE const void *data, size_t btw)
{
	size_t tocopy, skip, free;
	if (data == NULL || btw == 0)
	{
		return 0;
	}

	free = FreeSize();
	skip = btw - free;
	if (skip > 0)
	{
		free += Skip(skip);
	}

	const uint8_t *d = (uint8_t*) data;

	/* Calculate maximum number of bytes available to write */
	btw = RING_MIN(free, btw);

	/* Step 1: Write data to linear part of data */
	tocopy = RING_MIN(Data.size - Data.writePtr, btw);
	RING_MEMCPY(&Data.buffer[Data.writePtr], d, tocopy);
	btw -= tocopy;

	/* Step 2: Write data to beginning of data (overflow part) */
	if (btw > 0)
	{
		RING_MEMCPY(Data.buffer, &d[tocopy], btw);
		Data.writePtr = btw;
	}
	else
	{
		Data.writePtr += tocopy;
		/* Step 3: Check end of data */
		if (Data.writePtr >= Data.size)
		{
			Data.writePtr = 0;
		}
	}

	return tocopy + btw;
}

uint8_t 	Ring::Read()
{

	if (!RING_IS_VALID)
	{
		return false;
	}

	uint32_t w = Data.writePtr;
	uint32_t r = Data.readPtr;

	if (w == r)
	{
		return 0;
	}

	Data.readedByte = Data.buffer[Data.readPtr++];

	if (Data.readPtr >= Data.size)
	{
		Data.readPtr = 0;
	}

	return 1;
}
uint8_t 	Ring::ReadedByte()
{
	return Data.readedByte;
}
size_t 		Ring::Read(void *data, size_t btr)
{
	size_t tocopy;
	uint8_t *d = (uint8_t*) data;

	if (data == NULL)
	{
		return 0;
	}

	/* Calculate maximum number of bytes available to read */
	btr = RING_MIN(FilledSize(), btr);
	if (btr == 0)
	{
		return 0;
	}

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - Data.readPtr, btr);
	RING_MEMCPY(d, &Data.buffer[Data.readPtr], tocopy);
	btr -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btr > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btr);
		Data.readPtr = btr;
	}
	else
	{
		Data.readPtr += tocopy;
		/* Step 3: Check end of Data */
		if (Data.readPtr >= Data.size)
		{
			Data.readPtr = 0;
		}
	}

	return tocopy + btr;
}
void* 		Ring::Read(size_t btr)
{
	size_t tocopy;

	/* Calculate maximum number of bytes available to read */
	btr = RING_MIN(FilledSize(), btr);
	if (btr == 0)
	{
		return 0;
	}

	if (sizeof(Data.readBuffer) != btr)
	{
		delete[] Data.readBuffer;
		Data.readBuffer = new uint8_t[btr];
	}

	uint8_t *d = Data.readBuffer;

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - Data.readPtr, btr);
	RING_MEMCPY(d, &Data.buffer[Data.readPtr], tocopy);
	btr -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btr > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btr);
		Data.readPtr = btr;
	}
	else
	{
		Data.readPtr += tocopy;
		/* Step 3: Check end of Data */
		if (Data.readPtr >= Data.size)
		{
			Data.readPtr = 0;
		}
	}
	return d;
}
void* 		Ring::ReadAll()
{
	size_t tocopy;

	/* Calculate maximum number of bytes available to read */
	size_t btr = FilledSize();
	if (btr == 0)
	{
		return 0;
	}

	if (sizeof(Data.readBuffer) != btr)
	{
		delete[] Data.readBuffer;
		Data.readBuffer = new uint8_t[btr];
	}

	uint8_t *d = Data.readBuffer;

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - Data.readPtr, btr);
	RING_MEMCPY(d, &Data.buffer[Data.readPtr], tocopy);
	btr -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btr > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btr);
		Data.readPtr = btr;
	}
	else
	{
		Data.readPtr += tocopy;
		/* Step 3: Check end of Data */
		if (Data.readPtr >= Data.size)
		{
			Data.readPtr = 0;
		}
	}
	return d;
}

size_t 		Ring::Peek(size_t skip_count, void *data, size_t btp)
{
	size_t full, tocopy, r;

	if (data == NULL || btp == 0)
	{
		return 0;
	}

	uint8_t *d = (uint8_t*) data;

	r = Data.readPtr;

	/* Calculate maximum number of bytes available to read */
	full = FilledSize();

	if (skip_count >= full)
	{
		return 0;
	}

	r += skip_count;
	full -= skip_count;

	if (r >= Data.size)
	{
		r -= Data.size;
	}

	/* Check maximum number of bytes available to read after skip */
	btp = RING_MIN(full, btp);

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - r, btp);
	RING_MEMCPY(d, &Data.buffer[r], tocopy);
	btp -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btp > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btp);
	}
	return tocopy + btp;
}
void* 		Ring::Peek(size_t skip_count, size_t btp)
{
	size_t full, tocopy, r;

	if (btp == 0)
	{
		return 0;
	}

	r = Data.readPtr;

	/* Calculate maximum number of bytes available to read */
	full = FilledSize();

	if (skip_count >= full)
	{
		return 0;
	}

	r += skip_count;
	full -= skip_count;

	if (r >= Data.size)
	{
		r -= Data.size;
	}

	/* Check maximum number of bytes available to read after skip */
	btp = RING_MIN(full, btp);

	if (sizeof(Data.readBuffer) != btp)
	{
		delete[] Data.readBuffer;
		Data.readBuffer = new uint8_t[btp];
	}

	uint8_t *d = Data.readBuffer;

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - r, btp);
	RING_MEMCPY(d, &Data.buffer[r], tocopy);
	btp -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btp > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btp);
	}
	return d;
}
void* 		Ring::PeekAll()
{
	size_t tocopy, r;

	r = Data.readPtr;

	/* Calculate maximum number of bytes available to read */

	/* Check maximum number of bytes available to read after skip */
	size_t btp = FilledSize();
	if (btp == 0)
	{
		return 0;
	}

	if (sizeof(Data.readBuffer) != btp)
	{
		delete[] Data.readBuffer;
		Data.readBuffer = new uint8_t[btp];
	}

	uint8_t *d = Data.readBuffer;

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - r, btp);
	RING_MEMCPY(d, &Data.buffer[r], tocopy);
	btp -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btp > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btp);
	}
	return d;
}
void* 		Ring::PeekLast(size_t btp)
{
	size_t diff = FilledSize() - btp;
	if (diff < 1)
		diff = 0;
	return Peek(diff, btp);
}
void* 		Ring::PeekFirst(size_t btp)
{
	size_t full, tocopy, r;

	if (!RING_IS_VALID || btp == 0)
	{
		return 0;
	}

	r = Data.readPtr;

	/* Calculate maximum number of bytes available to read */
	full = FilledSize();

	/* Check maximum number of bytes available to read after skip */
	btp = RING_MIN(full, btp);
	if (btp == 0)
	{
		return 0;
	}

	if (sizeof(Data.readBuffer) != btp)
	{
		delete[] Data.readBuffer;
		Data.readBuffer = new uint8_t[btp];
	}

	uint8_t *d = Data.readBuffer;

	/* Step 1: Read data from linear part of data */
	tocopy = RING_MIN(Data.size - r, btp);
	RING_MEMCPY(d, &Data.buffer[r], tocopy);
	btp -= tocopy;

	/* Step 2: Read data from beginning of data (overflow part) */
	if (btp > 0)
	{
		RING_MEMCPY(&d[tocopy], Data.buffer, btp);
	}
	return d;
}

size_t 		Ring::Skip(size_t len)
{
	if (len == 0)
	{
		return 0;
	}

	len = RING_MIN(len, FilledSize());
	Data.readPtr += len;
	if (Data.readPtr >= Data.size)
	{
		Data.readPtr -= Data.size;
	}
	return len;
}
size_t 		Ring::Step(size_t len)
{

	if (len == 0)
	{
		return 0;
	}

	len = RING_MIN(len, FreeSize());
	Data.writePtr += len;
	if (Data.writePtr >= Data.size)
	{
		Data.writePtr -= Data.size;
	}
	return len;
}

size_t 		Ring::FreeSize()
{
	size_t w, r;

	if (!RING_IS_VALID)
	{
		return 0;
	}

	/* Use temporary values in case they are changed during operations */
	w = Data.writePtr;
	r = Data.readPtr;

	/* Buffer free size is always 1 less than actual size */

	if (w == r)
		return Data.size - 1;

	if (r > w)
		return r - w - 1;

	return Data.size - (w - r) - 1;

}
size_t 		Ring::FilledSize()
{
	size_t w, r;

	if (!RING_IS_VALID)
	{
		return 0;
	}

	/* Use temporary values in case they are changed during operations */
	w = Data.writePtr;
	r = Data.readPtr;

	if (w == r)
		return 0;

	if (w > r)
		return w - r;

	return Data.size - (r - w);
}
size_t 		Ring::FullSize()
{
	return Data.size;
}

void 		Ring::Reset()
{
	if (RING_IS_VALID)
	{
		Data.writePtr = 0;
		Data.readPtr = 0;
	}
}

void* 		Ring::ReadAddress()
{
	if (!RING_IS_VALID)
	{
		return NULL;
	}
	return &Data.buffer[Data.readPtr];
}
size_t 		Ring::ReadLinearlength()
{
	size_t w, r, len;

	if (!RING_IS_VALID)
	{
		return 0;
	}

	/* Use temporary values in case they are changed during operations */
	w = Data.writePtr;
	r = Data.readPtr;
	if (w > r)
	{
		len = w - r;
	}
	else if (r > w)
	{
		len = Data.size - r;
	}
	else
	{
		len = 0;
	}
	return len;
}

void* 		Ring::WriteAddress()
{
	if (!RING_IS_VALID)
	{
		return NULL;
	}
	return &Data.buffer[Data.writePtr];
}
size_t 		Ring::WriteLinearlength()
{
	size_t w, r, len;

	if (!RING_IS_VALID)
	{
		return 0;
	}

	/* Use temporary values in case they are changed during operations */
	w = Data.writePtr;
	r = Data.readPtr;
	if (w >= r)
	{
		len = Data.size - w;
		/*
		 * When read pointer is 0,
		 * maximal length is one less as if too many bytes
		 * are written, data would be considered empty again (r == w)
		 */
		if (r == 0)
		{
			/*
			 * Cannot overflow:
			 * - If r is not 0, statement does not get called
			 * - Data.size cannot be 0 and if r is 0, len is greater 0
			 */
			--len;
		}
	}
	else
	{
		len = r - w - 1;
	}
	return len;
}

void* 		Ring::StartAddress()
{
	return Data.buffer;
}
