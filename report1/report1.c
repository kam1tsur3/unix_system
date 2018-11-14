//61602192 Shogo Iyota

#include <stdio.h>

#define NNODE 6
#define INF 100

int cost[NNODE][NNODE] = {
	{   0,   2,   5,   1, INF, INF},
	{   2,   0,   3,   2, INF, INF},
	{   5,   3,   0,   3,   1,   5},
	{   1,   2,   3,   0,   1, INF},
	{ INF, INF,   1,   1,   0,   2},
	{ INF, INF,   5, INF,   2,   0}
};

void make_tree(int root);

int main()
{
	int i;
	for(i = 0; i < NNODE; i++)
		make_tree(i);
	return 0;
}

void make_tree(int root)
{
	int D[NNODE] = {0};
	int p[NNODE] = {0};
	char minflag[NNODE] = {0};
	minflag[root] = 1;

	//init D p
	int i;
	for(i = 0; i < NNODE; i++){
		D[i] = cost[root][i];
		p[i] = root;
	}

	//make tree
	int min;
	int new_node = root;
	for(i = 1; i < NNODE; i++){
		min = INF;
		int j;
		for(j = 0; j < NNODE; j++){
			if(minflag[j] == 1)continue;
			if(D[j] > cost[new_node][j]+D[new_node]){
				D[j] = cost[new_node][j] + D[new_node];
				p[j] = new_node;
			}
		}
		for(j = 0; j < NNODE; j++){
			if(minflag[j] == 1)continue;
			if(min > D[j]){
				min = D[j];
				new_node = j;
			}
		}
		minflag[new_node] = 1;
	}

	//print result
	printf("root node %c:\n", 'A'+(char)root);
	for(i = 0; i < NNODE; i++)
		printf("[%c,%c,%d] ", (char)i+'A', (char)p[i]+'A', D[i]);
	printf("\n");
}


