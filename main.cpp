#include "types.h"
#include "ord.h"
#include "linear.h"
#include <ctime>
using namespace std;

vector <pair <void*, void*> > T;
TotalOrder *order;
Linear *brutForce;

const int checkConst = 5;
bool checkCorrectness = true;

void check()
{
	int n = T.size();
	int a = rand() % n, b = rand() % n;
	bool p = order->query(T[a].first, T[b].first);
	bool q = p;
	if (checkCorrectness)
		q = brutForce->query(T[a].second, T[b].second);
	
	if (p == q)
		return;
	
	printf("fail, n=%d, a=%d, b=%d;   ", n, a, b);
	printf("O1:%d, linear:%d; \n", p, q);

	vector <BNode*> p1, p2;
	
	BNode *v = ((CNode*)T[a].first)->fchunk()->rootTag->leaf;
	printf("path1:\n");
	while (v)
		p1.push_back(v), printf("%p\n", v), v = v->father;
		
	v = ((CNode*)T[b].first)->fchunk()->rootTag->leaf;
	printf("\npath2:\n");
	while (v)
		p2.push_back(v), printf("%p\n", v), v = v->father;
	
	printf("\n\nels:\n%p\n%p\n\n", ((CNode*)T[a].first)->fchunk()->rootTag,
		((CNode*)T[b].first)->fchunk()->rootTag);
		
	printf("el tags:\n");
	((CNode*)T[a].first)->fchunk()->rootTag->tag->print();
	((CNode*)T[b].first)->fchunk()->rootTag->tag->print();
	
	printf("\n================\nSUBTREE WITH ERROR::\n");
	
	for (int i=0; i<p1.size(); i++)
	{
		if (p1[i] != p2[i])
			continue;
		((BNode*)p1[i])->print();
		break;
	}
	
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("usage: ./main '# inserts' OPTIONAL: 'param a' 'param k'\n");
		return 1;
	}
	
	order = new TotalOrder;
	brutForce = new Linear;
	
	int a = 5, k = 5;
	
	if (argc >= 4)
		a = atoi(argv[2]), k = atoi(argv[3]);
	
	int n = atoi(argv[1]);
	srand(n);
	T.resize(1);
	T[0] = {order->init(a, k), brutForce->init()};
	
	long long start = time(NULL), prv = start;
	
	for (int sz=0; sz<n; sz++)
	{
		int i = rand() % T.size();
		pair <void*, void*> p;
		p.first = order->insert(T[i].first);
		p.second = brutForce->insert(T[i].second);
		T.push_back(p);
		
	//	order->print();
		
		for (int step=0; step<checkConst; step++)
			check();
			
		if (sz % 500 == 0)
		{
			long long cr = time(NULL);
			printf("n = %d; dt = %lld, t = %lld\n", sz, cr - prv, cr - start);
			prv = cr;
		}
	}
	printf("reached size of %d, passed all tests, total time: %llds.\n", n, time(NULL) - start);
	return 0;
}
