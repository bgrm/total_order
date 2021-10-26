#ifndef CHUNK
#define CHUNK

#include "types.h"
#include "tag.h"
#include "btree.h"
#include "ord.h"

#define CHTagSize (WORDSIZE * 2 + 1)
using namespace std;

struct CNode
{
	Tag *tag;
	Chunk *chunk;
	CNode *father;
	CNode *firstSon, *lastSon;
	CNode *left, *right;
	int weight;

	bool passingSons, hasQuedLimit;
	int weightToPass, quedLimit;
	
	CNode *updtNode;
	bool direction;
	
	CNode()
	{
		memset(this, 0, sizeof(CNode));
	}
	
	void setTag();
	CNode(CNode *prv);
	void initPhase1(int limit);
	void proceedPhase1();
	void finishPhase1();
	void initPhase2();
	void proceedPhase2();
	void print(bool rec=false);
	Chunk *fchunk();
};

struct Chunk
{
	TotalOrder *ord;
	Element *rootTag;
	CNode *root;
	int num;
	list <Chunk*> :: iterator cardPtr;
	
	Chunk()
	{
		memset(this, 0, sizeof(Chunk));
	}
	
	Chunk(Chunk *prv);
	void updateCard(int d);
};

#endif
