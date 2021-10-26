#ifndef TAG
#define TAG

#include "types.h"
typedef __uint64_t Mask_t;
using namespace std;

#define WORDSIZE (sizeof(Mask_t) * 8)

struct Tag
{
	private:
	int size, noBlocks;
	Mask_t *T;
	
	Mask_t operator[] (int i)	const
	{	return T[i];	}
	
	void setHalf()
	{
		for (int i=0; i<noBlocks; i++)
		{
			T[i] >>= 1;
			assert((T[i] >> WORDSIZE-1) == 0);
			if (i != noBlocks-1)
				T[i] |= (1ull & T[i+1]) << WORDSIZE-1;
		}
	}
	
	bool setAsSum(Tag *a, Tag *b)
	{
		assert(size == a->size and size == b->size);
		bool carry = false;
		for (int i=0; i<noBlocks; i++)
		{
			Mask_t x = (a->T)[i], y = (b->T)[i];
			T[i] = x + y + carry;
			carry = ((x & y) > (~x & ~y)) or (x == ~y and carry);
		}
		return carry;
	}
	
	void updateMask(Mask_t from, Mask_t& to, int i1, int i2, int len)
	{
		Mask_t m = 0;
		for (int i=0; i<len; i++)
			m |= 1ull << i;
		from >>= i1;
		from &= m;
		from <<= i2;
		m = ~(m << i2);
		to &= m;
		to |= from;
	}
	
	public:
	Tag(int n=1)
	{
		size = n;
		noBlocks = size/WORDSIZE + 1;
		T = new Mask_t [noBlocks + 1];
		for (int i=0; i<=noBlocks; i++)
			T[i] = 0;
	}
	
	void operator= (const Tag& other)
	{
		assert(size == other.size);
		for (int i=0; i<noBlocks; i++)
			T[i] = other.T[i];
	}
	
	bool operator< (const Tag& other)	const
	{
		assert(size == other.size);
		for (int i=noBlocks-1; i>=0; i--)
			if (T[i] != other[i])
				return T[i] < other[i];
		return false;
	}
	
	void update(Tag *other, int ind)
	{
		assert(ind + other->size <= size);
		int d = ind % WORDSIZE, start = ind / WORDSIZE, s = other->noBlocks-1;
		for (int i=0; i<s; i++)
		{
			updateMask((*other)[i], T[start+i], 0, d, WORDSIZE-d);
			updateMask((*other)[i], T[start+i+1], WORDSIZE-d, 0, d);
		}
		
		int w = other->size % WORDSIZE;
		if (w == 0)
			w = WORDSIZE;
		updateMask((*other)[s], T[start+s], 0, d, min(WORDSIZE-d, w));
		if (w > WORDSIZE-d)
		{
			w -= WORDSIZE-d;
			updateMask((*other)[s], T[start+s+1], WORDSIZE-d, 0, w);
		}
	}
	
	void setBit(int i, bool val=true)
	{
		assert(0 <= i and i < size);
		Mask_t m = 1ull << i%WORDSIZE;
		if (val)
			T[i/WORDSIZE] |= m;
		else
			T[i/WORDSIZE] &= ~m;
	}
	
	bool checkBit(int i)	const
	{	
		assert(i >= 0 and i < size);
		return ((T[i/WORDSIZE] >> i%WORDSIZE) & 1ull) != 0;
	}
	
	void setAll(int v=1)
	{
		for (int i=0; i<size; i++)
			setBit(i, v);
	}
	
	bool msb()	const
	{
		assert(size > 0);
		return checkBit(size-1);
	}
	
	void setAsAverage(Tag *a, Tag *b)
	{
		bool carry = setAsSum(a, b);
		T[noBlocks++] = carry ? 1 : 0;
		setHalf();
		noBlocks--;
	}
	
	void setAsHalf(Tag *other)
	{
		*this = *other;
		bool b = msb();
		setBit(size-1, false);
		setHalf();
		setBit(size-1, b);		
	}
	
	void setAsTwice(Tag *other)
	{
		*this = *other;	
		bool b = msb();
		setBit(size-1, true);
		setHalf();
		setBit(size-1, b);
	}
	
	int min(int a, int b)	const
	{	return a < b ? a : b;	}
	
	void print()	const
	{
		for (int i=min(size-1, 200); i>=0; i--)
			printf("%d", checkBit(i));
		printf("\n");	
	}
};

#endif

