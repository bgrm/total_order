#include "ord.h"

Chunk* TotalOrder::initFirstChunk(int a, int k)
{
	Chunk *firstChunk = new Chunk;
	firstChunk->ord = this;
	firstChunk->rootTag = btree->init(a, k);
	firstChunk->root = firstRoot;
	firstChunk->num = 1;
	card->add(firstChunk);
	return firstChunk;
}

void TotalOrder::initFirstNodes(Chunk *firstChunk)
{
	CNode *son = new CNode, *leaf = new CNode;
	firstRoot->tag = new Tag(CHTagSize);
	nil->tag = new Tag(CHTagSize);
	son->tag = new Tag(CHTagSize);
	leaf->tag = new Tag(CHTagSize);
	*son->tag = *initialTagLeft;
	*leaf->tag = *initialTagLeft;
	
	firstRoot->chunk = son->chunk = leaf->chunk = firstChunk;
	
	leaf->father = firstRoot->firstSon = firstRoot->lastSon = son;
	son->firstSon = son->lastSon = leaf;
	son->father = firstRoot;	
	nil->father = nil;
	
	firstRoot->left = firstRoot->right = nil;
	son->left = son->right = nil;
	leaf->left = leaf->right = nil;
	
	firstRoot->weight = son->weight = leaf->weight = 1;
}

void *TotalOrder::init(int a, int k)
{
	firstRoot = splitR = new CNode;
	nil = new CNode;
	btree = new WBBTree;
	n = logn = 1;
	card = new Cardinalities;
	initialTagLeft = new Tag(CHTagSize);
	initialTagRight = new Tag(CHTagSize);
	
	initialTagLeft->setBit(WORDSIZE);
	initialTagRight->setBit(CHTagSize-1);
	initialTagRight->setBit(CHTagSize-2);
	
	Chunk *firstChunk = initFirstChunk(a, k);
	initFirstNodes(firstChunk);
	
	return (void*)(firstRoot->firstSon->firstSon);
}

bool TotalOrder::query(void *av, void *bv)
{
	CNode *a = (CNode*)av, *b = (CNode*)bv;
	if (a->fchunk() != b->fchunk())
		return *a->fchunk()->rootTag < *b->fchunk()->rootTag;
	if (a->father != b->father)
		return *a->father->tag < *b->father->tag;	
	return *a->tag < *b->tag;
}

void *TotalOrder::insert(void *prv)
{
	CNode *v = new CNode((CNode*)prv), *w = v->father;
	v->weight = 1;
	w->weight++;
	w->father->weight++;
	v->fchunk()->updateCard(1);

	n++;
	ctr++;
	
	if (w->weight == logn)
		w->initPhase1(logn/2);
		
	if (ctr == logn)
	{
		splitR = card->getBiggestChunk()->root;
		splitR->initPhase1(splitR->weight / 2);
		assert(splitR->updtNode == NULL);
		new Chunk(splitR->chunk);
		ctr = 0;
		logn = max(2, (int)WORDSIZE - 1 -__builtin_clzll(n));
	}
	
	for (int step=0; step<6; step++)
	{
		splitR->passingSons ? splitR->proceedPhase1() : splitR->proceedPhase2();
		splitR->lastSon->proceedPhase1();
		splitR->lastSon->proceedPhase2();
		w->proceedPhase1();
		w->proceedPhase2();
		w->left->proceedPhase1();
	}

	btree->access();	
	return (void*)v;
}

void TotalOrder::print()
{
	printf("\n===========================\n\n");

	for (CNode *r=firstRoot; r!=nil; r=r->right)
	{
		printf("\nroot:\n");
		r->print();
		printf("\n");
		
		for (CNode *s=r->firstSon; s and s->father == r; s=s->right)
		{
			printf("son:\n");
			s->print();
			printf("\n");
			
			for (CNode *g=s->firstSon; g and g->father == s; g=g->right)
				g->print();
			printf("\n"); 
		}
		
		printf("\n");
	}
	
	printf("\n=============================\n\n");
}

void Cardinalities::remove(Chunk *ch)
{
	int n = ch->num;
	L[n].erase(ch->cardPtr);
	if (L[n].empty())
		bitset->set(n, 0);
}

void Cardinalities::add(Chunk *ch)
{
	int n = ch->num;
	if (n == L.size())
		L.resize(L.size()+1);
	if (L[n].empty())
		bitset->set(n, 1);
	ch->cardPtr = L[n].insert(L[n].end(), ch);
}
