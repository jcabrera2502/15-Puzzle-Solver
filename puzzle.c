#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct node {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;	/* location (row and colum) of blank tile 0 */
	struct node *next;
	struct node *parent;			/* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];
struct node *start,*goal;
struct node *open = NULL, *closed = NULL;
struct node *succ_nodes[4];

void print_a_node(struct node *pnode) {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) 
			printf("%2d ", pnode->tiles[i][j]);
		printf("\n");
	}
	printf("\n");
}

struct node *initialize(char **argv){
	int i,j,k,index, tile;
	struct node *pnode;

	pnode=(struct node *) malloc(sizeof(struct node));
	index = 1;
	for (j=0;j<N;j++)
		for (k=0;k<N;k++) {
			tile=atoi(argv[index++]);
			pnode->tiles[j][k]=tile;
			if(tile==0) {
				pnode->zero_row=j;
				pnode->zero_column=k;
			}
		}
	pnode->f=0;
	pnode->g=0;
	pnode->h=0;
	pnode->next=NULL;
	pnode->parent=NULL;
	start=pnode;
	printf("initial state\n");
	print_a_node(start);

	pnode=(struct node *) malloc(sizeof(struct node));
	goal_rows[0]=3;
	goal_columns[0]=3;

	for(index=1; index<NxN; index++){
		j=(index-1)/N;
		k=(index-1)%N;
		goal_rows[index]=j;
		goal_columns[index]=k;
		pnode->tiles[j][k]=index;
	}
	pnode->tiles[N-1][N-1]=0;	      /* empty tile=0 */
	pnode->f=0;
	pnode->g=0;
	pnode->h=0;
	pnode->next=NULL;
	goal=pnode; 
	printf("goal state\n");
	print_a_node(goal);

	return start;
}

/* merge the remaining nodes pointed by succ_nodes[] into open list. 
 * Insert nodes based on their f values --- keep f values sorted. */
void merge_to_open() { 
	int x = 0;

	struct node *head = open;
	struct node *pnode = head;

	while(x < N)
	{
		if (succ_nodes[x] != NULL)
		{
			// make initial head
			if (head == NULL)
			{
				head = succ_nodes[x];
			}
			else
			{
				if((head)-> f > succ_nodes[x]->f)
				{
					succ_nodes[x]->next = head;
					head = succ_nodes[x];
					
				}
				else
				{
					while(pnode->next != NULL && pnode->next->f < succ_nodes[x]->f)
					{
						pnode = pnode->next;
					}
					succ_nodes[x]->next = pnode->next;
					pnode->next = succ_nodes[x];
					
				}
			}
			pnode = head;
		}
		x++;
	}
	open = head;
}

/*update the f,g,h function values for a node */
void update_fgh(int i) {
	struct node *pnode = succ_nodes[i];
	if (pnode != NULL)
	{
		int tot_dist, row, col, num;

		// Calculate h, iterate through the nodes and count distance

		for (int a=0; a<N; a++)
		{
			for (int b=0; b<N; b++)
			{
				num = pnode->tiles[a][b];

				row = (num-1) / 4;
				col = (num-1) % 4;

				tot_dist = abs(a-row) + abs(b-col) + tot_dist;
			}
		}

		pnode->h = tot_dist;
		if (pnode->parent == NULL)
		{
			pnode->g = 1;
		}
		else
		{
			pnode->g = pnode->parent->g + 1;
		}
		pnode->f = pnode->h + pnode->g;
	}
}

/* 0 goes down by a row */
void move_down(struct node * pnode){

	// Change the 0 tile to the swapped number
	pnode->tiles[pnode->zero_row][pnode->zero_column] = 
	pnode->tiles[(pnode->zero_row) + 1][(pnode->zero_column)];

	// Change the swapped tile to 0
	pnode->tiles[(pnode->zero_row) + 1][(pnode->zero_column)] = 0;

	// Keep track of 0
	pnode->zero_row = pnode->zero_row + 1;
}

/* 0 goes right by a column */
void move_right(struct node * pnode){

	// Change the 0 tile to the swapped number
	pnode->tiles[pnode->zero_row][pnode->zero_column] = 
	pnode->tiles[(pnode->zero_row)][(pnode->zero_column) + 1];

	// Change the swapped tile to 0
	pnode->tiles[(pnode->zero_row)][(pnode->zero_column) + 1] = 0;

	// Keep track of 0
	pnode->zero_column = pnode->zero_column + 1;
}

/* 0 goes up by a row */
void move_up(struct node * pnode){

	// Change the 0 tile to the swapped number
	pnode->tiles[pnode->zero_row][pnode->zero_column] = 
	pnode->tiles[(pnode->zero_row) - 1][(pnode->zero_column)];

	// Change the swapped tile to 0
	pnode->tiles[(pnode->zero_row) - 1][(pnode->zero_column)] = 0;

	// Keep track of 0
	pnode->zero_row = pnode->zero_row - 1;
}

/* 0 goes left by a column */
void move_left(struct node * pnode){

	// Change the 0 tile to the swapped number
	pnode->tiles[pnode->zero_row][pnode->zero_column] = 
	pnode->tiles[(pnode->zero_row)][(pnode->zero_column) - 1];

	// Change the swapped tile to 0
	pnode->tiles[(pnode->zero_row)][(pnode->zero_column) - 1] = 0;

	// Keep track of 0
	pnode->zero_column = pnode->zero_column - 1;
}

/* expand a node, get its children nodes, and organize the children nodes using
 * array succ_nodes.
 */

void expand(struct node *selected) {
	int succ_count = 0;
	struct node *newnode;
	succ_nodes[0] = NULL;
	succ_nodes[1] = NULL;
	succ_nodes[2] = NULL;
	succ_nodes[3] = NULL;
	// Check directions that we can move, then move there
	if (selected->zero_row > 0) // move down
	{
		newnode=(struct node *) malloc(sizeof(struct node));
		*newnode = *selected;
		move_up(newnode);
		succ_nodes[succ_count] = newnode;
		succ_count++;
	}
	if (selected->zero_row < 3) // move up
	{
		newnode=(struct node *) malloc(sizeof(struct node));
		*newnode = *selected;
		move_down(newnode);
		succ_nodes[succ_count] = newnode;
		succ_count++;
	}
	if (selected->zero_column < 3) // move right
	{
		newnode=(struct node *) malloc(sizeof(struct node));
		*newnode = *selected;
		move_right(newnode);
		succ_nodes[succ_count] = newnode;
		succ_count++;
	}
	if (selected->zero_column > 0) // move left
	{
		newnode=(struct node *) malloc(sizeof(struct node));
		*newnode = *selected;
		move_left(newnode);
		succ_nodes[succ_count] = newnode;
		succ_count++;
	}
	succ_count = 0;		
	
}

int nodes_same(struct node *a,struct node *b) {
	int flg=FALSE;
	if (memcmp(a->tiles, b->tiles, sizeof(int)*NxN) == 0)
		flg=TRUE;
	return flg;
}

/* Filtering. Some states in the nodes pointed by succ_nodes may already be included in 
 * either open or closed list. There is no need to check these states. Release the 
 * corresponding nodes and set the corresponding pointers in succ_nodes to NULL. This is 
 * important to reduce execution time.
 */


void filter(int i, struct node *pnode_list){ 

	struct node *pnode = succ_nodes[i];

	while (pnode_list != NULL && pnode != NULL)
	{
		if (nodes_same(pnode, pnode_list))
		{
			free(pnode);
			succ_nodes[i] = NULL;
			break;
		}
		
		pnode_list=pnode_list->next;
	}
	
}

int main(int argc,char **argv) {
	int iter,cnt, solved = 0;
	struct node *copen, *cp, *solution_path;
	int ret, i, pathlen=0, index[N-1];

	solution_path=NULL;
	start=initialize(argv);	/* init initial and goal states */
	open=start; 

	iter=0; 
	while (open!=NULL) {	/* Termination cond with a solution is tested in expand. */
		copen=open;
		open=open->next; /* get the first node from open to expand */
		

		if(nodes_same(copen,goal)){ /* goal is found */
			do{ /* trace back and add the nodes on the path to a list */
				copen->next=solution_path;
				solution_path=copen;
				copen=copen->parent;
				pathlen++;
			} while(copen!=NULL);
			printf("Path (length=%d):\n", pathlen); 
			printf("\n");
			copen=solution_path;
			
			while (solution_path!=NULL)
			{
				print_a_node(solution_path);
				solution_path=solution_path->next;
			}
			solved = 1;
			 /* print out the nodes on the list */
			break;
		}

		/* Find new successors */
		expand(copen);
		

		for(i=0;i<4;i++){
			filter(i,open);
			filter(i,closed);
			update_fgh(i);
			if (succ_nodes[i] != NULL)
				succ_nodes[i]->parent = copen;
		}

		merge_to_open(); /* New open list */

		copen->next=closed;
		closed=copen;		/* New closed */
		/* print out something so that you know your 
		 * program is still making progress 
		 */
		iter++;
		//  if(iter %10000 == 0)
		//  	printf("iter %d\n", iter);
	}

	if (open == NULL && solved == 0)
	{
		printf("no solution");
	}
	return 0;
} /* end of main */
