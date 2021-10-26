#include "btree.h"

void BNode::print()
{
	printf("ptr:%p, w:%lld/%lld, h:%d, fs:%p,  ls:%p, lt:%p, rt:%p, uN:%p, dir:%d, own:%d, father:%p\n",
	this, weight, maxWeight, h, firstSon, lastSon, left, right, updtNode, direction, onMyOwn, father);
	
	printf("tagCur:\n");
	tagCurrent->print();
	printf("tagUpdt:\n");
	tagUpdate->print();
	printf("tagQd:\n");
	tagQued->print();
	printf("\n");
	
	if (h == 1)
		return;
		
	for (BNode *u = firstSon; h and u->father == this; u=u->right)
	{
		assert(u);
		if (u->right->father != this)
			assert(u == lastSon);
		u->print();
	}
	
	if (h != 0)
		return;
		
	printf("ELEMS!:\n");
	for (Element *el=firstEl; el->leaf==this; el=el->right)
	{
		printf("ptr: %p, tagLocal: ", el);
		el->localTag->print();
		printf("global:\n");
		el->tag->print();
		printf("\n");
	}
}

void WBBTree::print()
{
	BNode *v = lastElement->left->leaf;
	assert(v);
	while (v->father)
		v = v->father;
		
	printf("\n================================\nBTREE!: (uNode: %p)\n\n", uNode);
		
	v->print();
	
	printf("\n================================\n\n");
}

void WBBTree::makeNewRoot(BNode *old)
{
	BNode *r = new BNode(tagSize);
	r->weight = old->weight - 1;
	r->maxWeight = old->maxWeight * a;
	r->h = old->h + 1;
	r->left = r->right = nil;
	r->firstSon = r->lastSon = old;
	r->updateBorderElements();
	r->c = noOps;
	old->father = r;
}

BNode *WBBTree::newNode(BNode *prv)
{
	BNode *v = new BNode(tagSize), *nxt = prv->right;
	*v->tagCurrent = *prv->tagCurrent;
	v->maxWeight = prv->maxWeight;
	v->h = prv->h;
	
	if (prv->father == NULL)
		makeNewRoot(prv);
	v->father = prv->father;
	v->left = prv;
	v->right = nxt;
	prv->right = nxt->left = v;
	
	if (v->father->lastSon == prv)
		v->father->lastSon = v;
		
	v->c = noOps;
	nodes++;
	return v;
}

void WBBTree::phase2Leaf(BNode *v)
{
	Element *first = v->firstEl;
	first->localTag->setAll(0);
	first->tag->update(first->localTag, 0);		

	for (Element *el = first->right; el->leaf == v; el = el->right)
		el->setLocalTag(el->left, lastElement);
}

Tag *WBBTree::tagLeft(BNode *v)
{
	BNode* w = v->father;
	if (w != NULL and w->updtNode == v and w->direction)
		return v->tagUpdate;
	else
		return v->tagCurrent;
}

Tag *WBBTree::tagRight(BNode *v)
{
	BNode* w = v->father;
	if (w != NULL and w->updtNode == v and !w->direction)
		return v->tagUpdate;
	else
		return v->tagCurrent;
}

void WBBTree::initPhase1(BNode *v)
{
	BNode *uncle = v->right->father;
	
	if (uncle != v->father and !uncle->inPhase1())
	{
		v->tagQued->setAsTwice(v->left->tagQued);
		v->tagUpdate->setAsTwice(tagRight(v->left));
	}
	
	else
	{
		v->tagQued->setAsAverage(v->left->tagQued, v->right->tagQued);
		v->tagUpdate->setAsAverage(tagRight(v->left), tagLeft(v->right));
	}

	v->updtNode = v;
	v->currEl = v->lastEl;
	v->onMyOwn = true;
	v->direction = false;
	
	if (v->father->updtNode == v)
	{
		assert(v->c == noOps);
		v->c = noOps * 2;
		*v->tagUpdate = *v->left->tagUpdate;
		v->updtNode = v->father;
		v->onMyOwn = false;
	}
	
	else
		proceed(v);
}

void WBBTree::initPhase2(BNode *v)
{
	v->onMyOwn = true;
	v->c = noOps;
	
	if (v->h == 0)
	{
		phase2Leaf(v);
		v->updtNode = NULL;
		return;
	}
	
	BNode *first = v->firstSon;
	v->direction = first->tagCurrent->msb();
	v->updtNode = v->direction ? v->firstSon : v->lastSon;
	v->currEl = v->direction ? v->firstEl : v->lastEl;
	first->tagQued->setAll(0);
	if (!v->direction)
		first->tagQued->setBit(tagSize-1);

	for (BNode *u = first->right; u->father == v; u = u->right)	
		u->tagQued->setAsTwice(u->left->tagQued);
}

void WBBTree::proceed(BNode *v)
{
	if (v->updtNode == NULL)
		return;
		
	Element *el = v->currEl;
	BNode *w = v->updtNode;
	el->tag->update(w->tagUpdate, elemTagSize + tagSize * w->h);
}

void WBBTree::updateOldSon(BNode *v)
{
	*v->tagCurrent = *v->tagUpdate;		
	v->onMyOwn = true;
		
	if (v->updtNode == v->father)
		initPhase1(v);
	
	else if (v->updtNode == v)
	{
		v->direction = false;
		v->currEl = v->firstEl;
	}
}

void WBBTree::updateNewSon(BNode *v)
{
	if (v == NULL)
		return;
	*v->tagUpdate = *v->tagQued;
	
	if (v->updtNode == v)
		v->onMyOwn = false;
}

void WBBTree::updateUpdtNode(BNode *v, BNode *oldUn)
{	
	if (oldUn == v)
	{
		*v->tagCurrent = *v->tagUpdate;
		initPhase2(v);
		initPhase2(v->left);
		updateNewSon(v->left->updtNode);
		proceed(v->left);
	}
	
	if (v->h == 0)
		return;
	
	if (v->updtNode == v->lastSon->right or v->updtNode == v->firstSon->left)
		v->updtNode = NULL;
	
	updateNewSon(v->updtNode);
	
	if (oldUn != v)
		updateOldSon(oldUn);
}

void WBBTree::prepareUpdtData(BNode *v)
{
	assert(v);
	BNode *oldUn = v->updtNode;
	Element *oldEl = v->currEl;
	v->currEl = v->direction ? oldEl->right : oldEl->left;
	if (v->direction and oldEl == oldUn->lastEl)
		v->updtNode = oldUn->right;
	if (!v->direction and oldEl == oldUn->firstEl)
		v->updtNode = oldUn->left;

	if (oldUn != v->updtNode)
		updateUpdtNode(v, oldUn);
}

void WBBTree::split(BNode *v)
{
	BNode *u = newNode(v);
	
	if (v->h != 0)
	{
		u->lastSon = v->lastSon;
		while (v->weight > v->maxWeight/2)
		{
			BNode *w = v->lastSon;
			w->father = u;
			v->weight -= w->weight;
			v->lastSon = w->left;
			u->weight += w->weight;
			u->firstSon = w;
		}
		v->updateBorderElements();
		u->updateBorderElements();
	}
	
	else
	{
		u->lastEl = v->lastEl;
		for (int step=0; step<k; step++)
		{
			Element *el = v->lastEl;
			el->leaf = u;
			v->weight--;
			v->lastEl = el->left;
			u->weight++;
			u->firstEl = el;
		}
	}
}

Element *WBBTree::init(int aa, int kk)
{
	a = aa;
	k = kk;
	noOps = 8;
	tagSize 		= a*8 + 1;
	elemTagSize 	= k*4 + 1;
	globalTagSize 	= tagSize * WORDSIZE + elemTagSize;
	uNode = NULL;
	nil = new BNode(tagSize);
	nodes = 1;

	Element *elem 	= new Element(globalTagSize, elemTagSize);
	lastElement 	= new Element(globalTagSize, elemTagSize);
	BNode *v 		= new BNode(tagSize);
	
	v->weight = 1;
	v->maxWeight = k*2;
	v->left = v->right = nil;
	v->firstEl = v->lastEl = elem;
	v->c = noOps;
	nil->father = nil;
	elem->leaf = v;
	elem->left = elem->right = lastElement;
	lastElement->left = elem;
	lastElement->leaf = nil;
	lastElement->localTag->setBit(elemTagSize-1);
	return elem;
}

Element *WBBTree::insert(Element *prv)
{
	Element *el = new Element(globalTagSize, elemTagSize);
	Element *nxt = prv->right;
	BNode *leaf = prv->leaf, *leaf2 = nxt->leaf;

	el->leaf = leaf;
	el->left = prv;
	el->right = nxt;
	nxt->left = prv->right = el;		
	*el->tag = *prv->tag;
	if (leaf2 != leaf and !leaf2->inPhase1())
		el->setLocalTag(prv, lastElement);
	else	
		el->setLocalTag(prv, nxt);
	
	if (leaf->lastEl == prv)
		leaf->lastEl = el;
	
	assert(uNode == NULL);
	uNode = leaf;
	return el;
}

void WBBTree::access()
{
	if (uNode == NULL)
		return;
		
	if (uNode->father)
		uNode->father->updateBorderElements();
		
	if (uNode->updtWeight())
	{
		assert(uNode->updtNode == NULL);
		split(uNode);
		BNode *w = uNode->father;
		if (w->updtNode == uNode and *uNode->lastEl < *w->currEl)
		{
			w->updtNode = uNode->right;
			if (w->direction)
				updateOldSon(uNode);
		}
		initPhase1(uNode->right);
	}
	
	if (uNode->right->inPhase1())
	{
		assert(uNode->updtNode == NULL);
		uNode = uNode->right;
	}

	for (int step=0; step<uNode->c; step++)
	{
		if (uNode->updtNode == NULL)
			break;
		BNode *v = uNode->onMyOwn ? uNode : uNode->father;
		prepareUpdtData(v);
		proceed(v);
	}
	
	uNode = uNode->father;			
}

