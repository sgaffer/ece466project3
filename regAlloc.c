#include <stdio.h>
#include <stdlib.h>
#include "s3inst.h"
#include "functions.h"

void removeEdges(intNode* graph, int size, int edge)
{
	int i, j, k;
	int degree;
	int found;

	for(i=0;i<size;i++)
	{
		degree = graph[i].degree;
		found = 0;

		for(j=0;j<degree;j++)
		{
			if(graph[i].interferences[j] == edge)
			{
				found = 1;
				break;
			}
		}

		if(found == 1)
		{
			for(k=j;k<(degree-1);k++)
			{
				graph[i].interferences[k] = graph[i].interferences[k+1];
			}

			graph[i].degree--;
		}
	}
}


int intsWith(int reg, int index, intNode* graph, int size, int* registerMaps)
{
	int i;

	for(i=0;i<graph[index].degree;i++)
	{
		if(registerMaps[graph[index].interferences[i]] == reg)
			return 1;
	}

	return 0;
}


void regAlloc(intNode* intGraph, int size, int k, inst_t instList)
{
	int i, j;

	int 		largest;
	int 		largestDegree;
	int 		regCount = size;
	int*		stack;
	int 		stackPtr = 0;
	int 		found;
	int*		registerMaps;
	int		allocRegs = 1;
	inst_t	current;

	stack							= (int*)malloc(size*sizeof(int));
	intNode* modGraph = (intNode*)malloc(size*sizeof(intNode));

	// copy interference graph so values can be modified without destroying originals
	for(i=0;i<size;i++)
	{
		modGraph[i].degree = intGraph[i].degree;

		// malloc memory for interferences and copy each interference
		// size forumla is to stay consistant with extra space added for registers
		modGraph[i].interferences = 
					(int*)malloc(5*((intGraph[i].degree/5)+1)*sizeof(int));
					
		for(j=0;j<intGraph[i].degree;j++)
			modGraph[i].interferences[j] = intGraph[i].interferences[j];
			
		modGraph[i].valid = 1;
	}

	// remove nodes from the graph and push to the priority stack
	while(regCount >= 0)
	{
		largest = 0;
		largestDegree = -1;
		found = 0;
		
		// find largest degree node less than size to remove
		for(i=0;i<size;i++)
		{
			if((modGraph[i].degree > largestDegree) && (modGraph[i].degree < k))
			{
				found = 1;
				largest = i;
				largestDegree = modGraph[i].degree;
			}
		}

		if((i == size) && (found == 0))
		{
			for(i=0;i<size;i++)
			{
				if((modGraph[i].degree > largestDegree))
				{
					found = 1;
					largest = i;
					largestDegree = modGraph[i].degree;
				}
			}
		}

		// remove largest node from graph if found
		if(found == 1)
		{
			// remove node by setting values to special removed values and remove edges from other nodes
			removeEdges(modGraph, size, largest);
			modGraph[largest].degree = -1;
			modGraph[largest].interferences[0] = -1;		
		
			// push register to priority stack
			stack[stackPtr++] = largest;
			
			regCount--;
		}
		else		//no largest found less than k
			break;

	 	/*printf("___________________________________________\n");
		for(i=0;i<10;i++)
		{
			printf("R%d: ", i);
			for(j=0;j<modGraph[i].degree;j++)
			{
				printf("%d ", modGraph[i].interferences[j]);
			}
			printf("\n");
		}
		printf("_____________________________________________\n\n");*/
	}


	/*for(i=0;i<size;i++)
		printf("R%d\n", stack[i]);*/

	// if coloring is possible rename registers
	if(regCount == 0)
	{
		registerMaps = (int*)malloc(size*sizeof(int));

		// init registerMaps with -1
		for(i=0;i<size;i++)
			registerMaps[i] = -1;

		// map all registers to new values
		for(i=(stackPtr-1);i>=0;i--)					// old register
		{
			for(j=0;j<k;j++)								// new register k mapping
			{
				if(intsWith(j, stack[i], intGraph, size, registerMaps) == 0)
				{
					registerMaps[stack[i]] = j;
					break;
				}
			}

			if(registerMaps[stack[i]] == -1)			// check that coloring is possible
			{
				printf("No Possible Coloring with k = %d\n\n", k);
				allocRegs = 0;
				break;
			}
		}
	}
	else
	{
		printf("Program Error: No Possible Coloring with k = %d\n\n", k);
		allocRegs = 0;
	}

	if(allocRegs == 1)
	{
		// shift registers from new colors to register names
		// in range of R10 to R(10+k-1)
		for(i=0;i<size;i++)
			registerMaps[i] += 10;

		// do not rename spcial registers  R0, R4, R5, R6, and R7
		registerMaps[0] = 0;
		registerMaps[4] = 4;
		registerMaps[5] = 5;
		registerMaps[6] = 6;
		registerMaps[7] = 7;

		// debug print register mapping
		/*printf("_____________Register Mapping_____________\n");
		for(i=0;i<size;i++)
		{
			printf("R%03d -> R%03d\n", i, registerMaps[i]);
		}
		printf("_______________End Mapping_______________\n");*/

		// rename all registers in the code
		current = instList;
		while(current != NULL)
		{
			for(i=0;i<3;i++)
			{
				if(current->ops[i].t == op_reg)
					current->ops[i].reg = registerMaps[current->ops[i].reg];
			}
		
			current = current->next;
		}
	}
}

















