#include "chunk.h"
#include "btree.h"

Chunk::Chunk(Chunk *prv)
{
	memset(this, 0, sizeof(Chunk));
	ord = prv->ord;
	rootTag = ord->btree->insert(prv->rootTag);
	root = prv->root->right;
	root->chunk = this;
	ord->card->add(this);
}

void Chunk::updateCard(int d)
{
	ord->card->remove(this);
	num += d;
	ord->card->add(this);
}

Chunk *CNode::fchunk()
{
	CNode *v = this;
	while (v->father != NULL)
		v = v->father;
	return v->chunk;
}

void CNode::setTag()
{
	if (father == NULL)
		return;
	if (left->father != father and right->father != father)
		*tag = *chunk->ord->initialTagRight;
	else if (left->father != father)
		tag->setAsHalf(right->tag);
	else if (right->father != father)
		tag->setAsTwice(left->tag);
	else if (left->tag->msb() != right->tag->msb())
	{
		assert(father->updtNode);
		if (father->direction)
			assert(father->updtNode == right);
		else
			assert(father->updtNode == left);
		father->updtNode = this;
		father->proceedPhase2();
	}
	else
		tag->setAsAverage(left->tag, right->tag);
}	

CNode::CNode(CNode *prv)
{
	memset(this, 0, sizeof(CNode));
	chunk = prv->chunk;
	father = prv->father;
	left = prv;
	right = prv->right;
	left->right = right->left = this;
	if (father and father->lastSon == prv)
	{
		if (father->updtNode == prv and !father->direction)
			father->updtNode = this;
		father->lastSon = this;
	}
	tag = new Tag(CHTagSize);
	setTag();
}

void CNode::initPhase1(int limit)
{	
	assert(weight > 0);
	assert(firstSon and lastSon);
	if (limit <= 0)
		return;

	if (passingSons)
	{
		hasQuedLimit = true;
		quedLimit = limit;
		return;
	}

	assert(limit < weight);
	passingSons = true;
	hasQuedLimit = false;
	weightToPass = limit;
	new CNode(this);
}

void CNode::proceedPhase1()
{
	if (!passingSons)
		return;
	if (weightToPass <= 0)
	{
		finishPhase1();
		return;
	}
	
	CNode *v = lastSon;
	int w = v->weight;
	
	if (weightToPass < w)
	{
		assert(father == NULL);
		v->initPhase1(weightToPass);
		v = v->right;
		w = v->weight; 
		if (w != 0)
			return;
	}
	
	assert(v != firstSon);
	for (CNode *u = this; u; u = u->father)
	{
		u->weight -= w;
		u->weightToPass -= w;
		u->quedLimit -= w;
	}
	fchunk()->updateCard(-w);
	lastSon = v->left;
	
	right->firstSon = v;
	if (right->lastSon == NULL)
		right->lastSon = v;
	for (CNode *u = right; u; u = u->father)
		u->weight += w;
	right->fchunk()->updateCard(w);
	v->chunk = right->chunk;
	v->father = right;
	v->setTag();
	
	if (w == 0)
	{
		finishPhase1();
		return;
	}
		
	if (father != NULL and right->weight == chunk->ord->logn)
	{
		right->initPhase1(right->weight / 2);
		if (weightToPass > 0)
			new CNode(this);
	}
}

void CNode::finishPhase1()
{
	passingSons = false;
	
	if (hasQuedLimit and quedLimit != 0)
	{
		if (quedLimit > 0)
			initPhase1(quedLimit);
		else
		{
			right->initPhase1(right->weight + quedLimit);
			initPhase2();
		}
	}
	
	else
		initPhase2();
}

void CNode::initPhase2()
{
	direction = firstSon->tag->msb();
	updtNode = direction ? firstSon : lastSon;
	assert(updtNode);
}

void CNode::proceedPhase2()
{
	if (updtNode == NULL)
		return;
		
	if (updtNode->father != this)
	{
		if (direction)
		{
			updtNode = NULL;
			return;
		}
		
		else
			updtNode = lastSon;
	}
	
	if (direction)
	{
		if (updtNode == firstSon)
			*updtNode->tag = *chunk->ord->initialTagLeft;
		
		else
			updtNode->tag->setAsTwice(updtNode->left->tag);
	}
	
	else
	{
		if (updtNode == lastSon)
			*updtNode->tag = *chunk->ord->initialTagRight;
		
		else
			updtNode->tag->setAsHalf(updtNode->right->tag);
	}

	updtNode = direction ? updtNode->right : updtNode->left;
	if (updtNode->father != this)
		updtNode = NULL;
}

void CNode::print(bool rec)
{
	printf("\nptr=%p, w=%d, ph1:%d, ph2:%d, dir:%d, uN:%p;\n", this, weight, passingSons, updtNode != NULL, direction, updtNode);
	tag->print();
	
	if (father and left->father == father and tag->msb() != left->tag->msb())
	{
		assert(father->updtNode != NULL);
	}
	
	if (!rec)	return;
	
	for (CNode *u = firstSon; u and u->father == this; u = u->right)
		u->print(false);
}

