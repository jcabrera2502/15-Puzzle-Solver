#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct node {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;   /* location (row and colum) of blank tile 0 */
	struct node *next;
	struct node *parent;          /* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];

void swap(int row1,int column1,int row2,int column2, struct node * pnode){
	int tile = pnode->tiles[row1][column1];
	pnode->tiles[row1][column1]=pnode->tiles[row2][column2];
	pnode->tiles[row2][column2]=tile;
}


/* 0 goes down by a row */
void move_down(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row+1, pnode->zero_column, pnode); 
	pnode->zero_row++;
}

/* 0 goes right by a column */
void move_right(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column+1, pnode); 
	pnode->zero_column++;
}

/* 0 goes up by a row */
void move_up(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row-1, pnode->zero_column, pnode); 
	pnode->zero_row--;
}

/* 0 goes left by a column */
void move_left(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column-1, pnode); 
	pnode->zero_column--;
}

void print_a_node(struct node *pnode, int format) {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) printf("%2d ",pnode->tiles[i][j]);
		if(format==0)
			printf("\n");
	}
	printf("\n");
}

int main(int argc,char **argv) {
	int i,j,k,index,count;
	struct node *pnode;

	if(argc!=2 || sscanf(argv[1], "%d", &count)!=1) {
		printf("%s #steps\n", argv[0]);
		exit(1);
	}

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
	pnode->zero_row = N-1;
	pnode->zero_column = N-1;
	pnode->f=0;
	pnode->g=0;
	pnode->h=0;
	pnode->next=NULL;
	printf("goal state\n"); print_a_node(pnode, 0);

	srand(time(NULL));
	i=0;
	do{
		j = rand();
		k = j % 4;
  		if( k == 0 && pnode->zero_row>0){
     			move_up(pnode);
			i++;
		}
  		if( k == 1 && pnode->zero_row<N-1){
     			move_down(pnode);
			i++;
		}
  		if( k == 2 && pnode->zero_column>0){
     			move_left(pnode);
			i++;
		}
  		if( k == 3 && pnode->zero_column<N-1){
     			move_right(pnode);
			i++;
		}
  	}while(i<count);

	printf("initial state:\n");
	print_a_node(pnode, 0);

	printf("initial state in one line:\n");
	print_a_node(pnode, 1);

	return 0;
}


