#ifndef BTREE
#define BTREE

#include "types.h"
#include "tag.h"

struct BNode
{
	Tag *tagCurrent, *tagUpdate, *tagQued;
	long long weight, maxWeight;
	int h;
	BNode *father;
	BNode *firstSon, *lastSon;
	BNode *left, *right;
	Element *firstEl, *lastEl;
	BNode *updtNode;
	Element *currEl;
	bool onMyOwn, direction;
	int c;
	
	BNode(int tagSize = 0)
	{
		memset(this, 0, sizeof(BNode));
		tagCurrent = new Tag(tagSize);
		tagUpdate = new Tag(tagSize);
		tagQued = new Tag(tagSize);
	}
	
	bool updtWeight()
	{
		return ++weight == maxWeight;
	}
	
	void updateBorderElements()
	{
		if (h != 0)
		{
			assert(firstSon and lastSon);
			firstEl = firstSon->firstEl;
			lastEl = lastSon->lastEl;
		}
	}
	
	bool inPhase1()
	{
		if (father == NULL)
			return false;
		return updtNode == this or updtNode == father;
	}
	
	void print();
};

struct Element
{
	Tag *tag, *localTag;
	Element *left, *right;
	BNode *leaf;
	
	Element(int a=0, int b=0)
	{
		memset(this, 0, sizeof(Element));
		tag = new Tag(a);
		localTag = new Tag(b);
	}
	
	bool operator< (const Element& other)
	{
		return *tag < *other.tag;
	}
	
	void setLocalTag(Element *b, Element *c)
	{	
		localTag->setAsAverage(b->localTag, c->localTag);
		tag->update(localTag, 0);
	}
};


struct WBBTree
{
	int a, k, noOps, tagSize, elemTagSize, globalTagSize, nodes;
	BNode *uNode, *nil;
	Element *lastElement;
	
	private:
	void makeNewRoot(BNode *old);
	BNode* newNode(BNode *prv);
	void phase2Leaf(BNode *v);
	Tag *tagLeft(BNode *v);
	Tag *tagRight(BNode *v);
	void initPhase1(BNode *v);
	void initPhase2(BNode *v);
	void proceed(BNode *v);
	void updateOldSon(BNode *v);
	void updateNewSon(BNode *v);
	void updateUpdtNode(BNode *v, BNode *oldUn);
	void prepareUpdtData(BNode *v);
	void split(BNode *v);
	
	void testEls()
	{
		BNode *v = lastElement->right->leaf;
		while (v->father)
			v = v->father;
		Element *e = v->firstEl;
		while (v->firstSon)
		{
			v = v->firstSon;
			assert(v->firstEl == e);
		}
		
		while (e->right != lastElement)
		{
			if (!(*e < *e->right))
			{
				printf("NIE DZIALA!\n");
				e->leaf->print();
				e->right->leaf->print();
				
				e->tag->print();
				e->right->tag->print();
				exit(1);
			}
			e = e->right;
		}
		
		//printf("testEls succeded;\n");	
	}
	public:
	Element* init(int aa, int kk);
	Element* insert(Element* prv);
	void access();	
	void print();
};
#endif
