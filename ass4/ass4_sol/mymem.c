#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */

	if (head != NULL) {
	  struct memoryList *trav;
	  for(trav=head;trav->next != NULL; trav=trav->next)
	    free(trav->last);
	  free(trav);
	}

	myMemory = malloc(sz);

	/* TODO: initialize your bookkeeping structures... */
	next = head = malloc(sizeof(struct memoryList));
	head->size = sz;
	head->ptr = myMemory;
	head->alloc = 0;
	head->last = head->next = NULL;
	
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
        size_t req = requested;
	assert((int)myStrategy > 0);
	
	if (head == NULL)
	  return NULL;
	
	int bestsize;
	struct memoryList *trav, *temp, *bestnode;
	void *rv = NULL;
	
	switch (myStrategy)
	  {
	  case NotSet: 
	    return NULL;
	  case First:
	    trav = head;
	    while ((trav->size < req) || (trav->alloc == 1))
	      {
		trav = trav->next;
		if (trav == NULL)
		  return NULL;
	      }
	    
	    if (trav->size > req)
	      {
		temp = malloc(sizeof(struct memoryList));
		temp->last = trav;
		temp->next = trav->next;
		if (trav->next != NULL)
		  trav->next->last = temp;
		trav->next = temp;
		
		temp->size = trav->size - req;
		temp->ptr = trav->ptr+req;
		temp->alloc = 0;
		trav->size = req;
	      }
	    trav->alloc = 1;
	    rv = trav->ptr;
	    
	    break;

	  case Best:
	    bestsize = -1;
	    bestnode = NULL;
	    trav = head;
	    while (trav != NULL)
	      {
		if ((trav->alloc == 0) && (trav->size >= req) && ((bestsize == -1) || (trav->size < bestsize)))
		  {
		    bestsize = trav->size;
		    bestnode = trav;
		  }
		trav = trav->next;
	      }
	    
	    if (bestnode == NULL)
		    return NULL;
	    
	    trav = bestnode;
	    
	    if (trav->size > req)
	      {
		temp = malloc(sizeof(struct memoryList));
		temp->last = trav;
		temp->next = trav->next;
		if (trav->next != NULL)
		  trav->next->last = temp;
		trav->next = temp;
		
		temp->size = trav->size - req;
		temp->ptr = trav->ptr+req;
		temp->alloc = 0;
		trav->size = req;
	      }
	    trav->alloc = 1;
	    rv = trav->ptr;
	    
	    break;
	  case Worst:
	    bestsize = 0;
	    bestnode = NULL;
	    trav = head;
	    while (trav != NULL)
	      {
		if ((trav->alloc == 0) && (trav->size > bestsize))
		  {
		    bestsize = trav->size;
		    bestnode = trav;
		  }
		trav = trav->next;
	      }
	    
	    if ((bestnode == NULL) || (bestnode->size < req))
	      {
		    return NULL;
	      }
	    
	    trav = bestnode;
	    
	    if (trav->size > req)
	      {
		temp = malloc(sizeof(struct memoryList));
		temp->last = trav;
		temp->next = trav->next;
		if (trav->next != NULL)
		  trav->next->last = temp;
		trav->next = temp;
		
		temp->size = trav->size - req;
		temp->ptr = trav->ptr + req;
		temp->alloc = 0;
		trav->size = req;
	      }
	    trav->alloc = 1;
	    rv = trav->ptr;
	    
	    break;
	  case Next:
	    trav = next;
	    while (trav != NULL)
	      {
		if ((trav->alloc == 0) && (trav->size >= req))
		  break;
		trav = trav->next;
	      }
	    if (trav == NULL)
	      {
		trav = head;
		while (trav != next)
		  {
		    if ((trav->alloc == 0) && (trav->size > req))
		      break;
		    trav = trav->next;
		  }
		if (trav == next)
		  {
			return NULL;
		  }
	      }
	    
	    if (trav->size > req)
	      {
		temp = malloc(sizeof(struct memoryList));
		temp->last = trav;
		temp->next = trav->next;
		if (trav->next != NULL)
		  trav->next->last = temp;
		trav->next = temp;
		
		temp->size = trav->size - req;
		temp->ptr = trav->ptr + req;
		temp->alloc = 0;
		trav->size = req;
	      }
	    trav->alloc = 1;
	    rv = trav->ptr;
	    
	    next = trav->next;
	    if (next == NULL)
	      {
		next = head;
	      }
	    break;
	  }
	
	return rv;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{

	void* ptr = block;
	if (head == NULL)
	  return;
	
	struct memoryList *trav = head, *temp;
	
	while (trav->ptr != ptr)
	  {
	    trav = trav->next;
	    if (trav == NULL)
	      return;
	  }
	
	trav->alloc = 0;
	
	if ((trav->last != NULL) && (trav->last->alloc == 0))
	  {
	    if(next == trav) {
	      next = trav->next;
	      if (next == NULL) 
		next = head;
	    }
	    trav->last->size += trav->size;
	    temp = trav;
	    if (trav->next != NULL)
	      trav->next->last = trav->last;
	    trav->last->next = trav->next;
	    trav = trav->last;
	    free(temp);
	  }
	if ((trav->next != NULL) && (trav->next->alloc == 0))
	  {
	    if (next == trav->next)
		next = trav;
	    trav->size += trav->next->size;
	    temp = trav->next;
	    if (trav->next->next != NULL)
	      trav->next->next->last = trav;
	    trav->next = trav->next->next;
	    free(temp);
	  }

	//return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
        int hole_count = 0;
	struct memoryList *trav = head;
	while(trav != NULL) {
	  if(trav->alloc == 0)
	    hole_count++;
	  trav = trav->next;
	}

	return hole_count;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
        int alloc = 0;
	struct memoryList *trav = head;
	while(trav != NULL) {
	  if(trav->alloc == 1)
	    alloc += trav->size;
	  trav = trav->next;
	}

	return alloc;
}

/* Number of non-allocated bytes */
int mem_free()
{
        int free_bytes = 0;
	struct memoryList *trav = head;
	while(trav != NULL) {
	  if(trav->alloc == 0)
	    free_bytes += trav->size;
	  trav = trav->next;
	}
	
	return free_bytes;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
        int max = 0;
	struct memoryList *trav = head;
	while(trav != NULL) {
	  if(trav->alloc == 0 && trav->size > max)
	    max = trav->size;
	  trav = trav->next;
	}

	return max;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
       int count = 0; 
       struct memoryList *trav = head;
	while(trav != NULL) {
	  if(trav->alloc == 0 && trav->size <= size)
	    count++;
	  trav = trav->next;
	}

	return count;
}       

char mem_is_alloc(void *ptr)
{
        if(ptr < myMemory || ptr > myMemory + mySize)
	  return 0;
	struct memoryList *trav;
	for(trav=head; ptr >= trav->ptr+trav->size; trav = trav->next);
	return trav->alloc;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
        struct memoryList *trav;
	for(trav=head; trav!=NULL;trav=trav->next) {
	  printf("%d bytes ",trav->size);
	  if (trav->alloc)
	    printf("allocated.\n");
	  else
	    printf("free.\n");
	}
	//return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e, *ptr;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	
	for(ptr=myMemory;ptr<myMemory+mySize;ptr+=10)
	  printf("%d",mem_is_alloc(ptr));
	printf("\n\n");
	
}
