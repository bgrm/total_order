#ifndef ORD
#define ORD

#include "types.h"
#include "tag.h"
#include "btree.h"
#include "chunk.h"
#include "linear.h"

using namespace std;

struct Bitset
{
	unsigned long long *T, t;
	
	Bitset()
	{
		T = new unsigned long long [WORDSIZE];
		memset(T, 0, sizeof(T));
		t = 0;
	}
	
	void set(int i, int v)
	{
		int j = i / WORDSIZE, k = i % WORDSIZE;
		unsigned long long mj = 1ull << j, mk = 1ull << k;
		
		if (v == 1)
		{
			if (T[j] == 0)
				t |= mj;
			T[j] |= mk;
		}
		
		if (v == 0)
		{
			T[j] &= ~mk;
			if (T[j] == 0)
				t &= ~mj;
		}
	}
	
	int msb()
	{
		int j = WORDSIZE - 1 - __builtin_clzll(t);
		int k = WORDSIZE - 1 - __builtin_clzll(T[j]);
		return j * WORDSIZE + k;
	}
};

struct Cardinalities
{
	Bitset *bitset;
	vector <list <Chunk*> > L;
	
	Cardinalities()
	{
		bitset = new Bitset;
		L.resize(1);
	}
	void remove(Chunk *ch);
	void add(Chunk *ch);
	Chunk *getBiggestChunk()
	{
		return L[bitset->msb()].back();
	}
};


struct TotalOrder
{
	CNode *firstRoot, *splitR, *nil;
	WBBTree *btree;
	int n, logn, ctr;
	Cardinalities *card;
	Tag *initialTagLeft, *initialTagRight;
	
	TotalOrder()
	{
		memset(this, 0, sizeof(TotalOrder));
	}
	
	Chunk *initFirstChunk(int a, int k);
	void initFirstNodes(Chunk *firstChunk);
	void *init(int a, int k);
	bool query(void *a, void *b);
	void *insert(void *prv);
	void print();
};

#endif
