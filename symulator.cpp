#include "types.h"
#include "ord.h"
#include "linear.h"
using namespace std;

vector <void*> ptrs;

int main(int argc, char *argv[])
{
	if (argc < 2 or (argv[1][0] != 'L' and argv[1][0] != 'C'))
	{
		printf("usage: ./symulator 'mode'  [L - linear, C - constant]\n");
		return 1;
	}

	int q, a, k;
	scanf ("%d%d%d", &q, &a, &k);
	bool tryp = argv[1][0] == 'C';
	TotalOrder *order = new TotalOrder;
	Linear *brutForce = new Linear;
	ptrs.push_back(tryp ? order->init(a,k) : brutForce->init());
	
	while (q--)
	{
		int event, x, y;
		scanf ("%d", &event);
		
		if (event == 0) // insert
		{
			scanf ("%d", &x);
			void *ptr = tryp ? order->insert(ptrs[x]) : brutForce->insert(ptrs[x]);
			ptrs.push_back(ptr);
		}
		
		else	// query
		{
			scanf ("%d%d", &x, &y);
			printf("%d", tryp ? order->query(ptrs[x], ptrs[y]) : brutForce->query(ptrs[x], ptrs[y]));		
		}
	}
	
	printf("\n");
	
	return 0;
}
