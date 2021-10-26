#ifndef LIN
#define LIN
#include <bits/stdc++.h>
using namespace std;

struct Linear
{
	list <void*>* L;

	void *init()
	{
		L = new list <void*>;
		void *ptr = new int;
		L->clear();
		L->push_back(ptr);
		return ptr;
	}

	void *insert(void *prv)
	{
		for (list <void*> :: iterator it = L->begin(); it != L->end(); it++)
		{
			if (*it == prv)
			{
				void *ptr = new int;
				L->insert(++it, ptr);
				return ptr;
			}
		}
		
		assert(false);
	}

	bool query(void *a, void *b)
	{
		bool afterA = false;
		for (list <void*> :: iterator it = L->begin(); it != L->end(); it++)
		{
			if (*it == b)
				return afterA;
			if (*it == a)
				afterA = true;
		}
		assert(false);
	}
	
	int rzad(void *a)
	{
		int b = 0;
		for (list <void*> :: iterator it = L->begin(); it != L->end(); it++)
		{
			if (*it == a)
				return b;
			b++;
		}
		assert(false);		
	}
};
#endif
