#include <inc/lib.h>
uint32 P[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE];
uint32 nextFitPlace=USER_HEAP_START;
uint32 * nextFit( uint32 size)
{
	uint32 ptrStartAllocation;
            size=ROUNDUP(size,PAGE_SIZE);
			uint32 noOfRequiredPages=size/PAGE_SIZE;
			uint32 freePagesCount =0;

			struct Frame_Info* result ;
			for ( uint32 i =nextFitPlace;i<USER_HEAP_MAX;i+=PAGE_SIZE)
			{

				if(P[(i- USER_HEAP_START) / PAGE_SIZE] == 0 )
				{ freePagesCount++;

				if ( freePagesCount==noOfRequiredPages)
				{ptrStartAllocation= i-(noOfRequiredPages*PAGE_SIZE);
					ptrStartAllocation+= PAGE_SIZE;

						nextFitPlace= ptrStartAllocation;

				return ( uint32 *)ptrStartAllocation;
				}

		}// end outer if
		else if ( P[(i - USER_HEAP_START) / PAGE_SIZE] != 0)
		{
			//nextFitPlace+=PAGE_SIZE;
			freePagesCount=0;

		}
	}// end first  for

freePagesCount=0;
 for ( uint32 i =USER_HEAP_START;i<nextFitPlace ;i+=PAGE_SIZE)
 {

     if (P[(i - USER_HEAP_START) / PAGE_SIZE]== 0)
     {freePagesCount++;
         if ( freePagesCount==noOfRequiredPages)
             	{	ptrStartAllocation= (i-(noOfRequiredPages*PAGE_SIZE));
             		ptrStartAllocation+=PAGE_SIZE;
             	  nextFitPlace= ptrStartAllocation;

             	return ( uint32 *)ptrStartAllocation;
             	}
     }// end if
     else if( P[(i - USER_HEAP_START) / PAGE_SIZE]!= 0 )
       	{ freePagesCount=0;

       	}


 } // end second for
return NULL;

 } // function end*/

uint32 ptrStartAllocation;
void* malloc(uint32 size)
{
	if (sys_isUHeapPlacementStrategyNEXTFIT())
		ptrStartAllocation= (uint32)nextFit(ROUNDUP(size,PAGE_SIZE));
	if(ptrStartAllocation != 0)
	{
	sys_allocateMem(ptrStartAllocation, ROUNDUP(size,PAGE_SIZE));
   uint32 Initial_address,Initial_size;
     for(Initial_address = ptrStartAllocation, Initial_size = ROUNDUP(size,PAGE_SIZE) ; Initial_size > 0 ; Initial_address += PAGE_SIZE , Initial_size -= PAGE_SIZE)
	   {
		P[(Initial_address - USER_HEAP_START) / PAGE_SIZE] = ROUNDUP(size,PAGE_SIZE);
       }
     nextFitPlace =ROUNDUP(ptrStartAllocation+size,PAGE_SIZE);
	return (void*)ptrStartAllocation;}
	else
		return NULL;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{

	uint32 virtual = (uint32)virtual_address;
	uint32 size = P[(virtual - USER_HEAP_START) / PAGE_SIZE];
	//uint32 size =virtual-(P[0]);

	sys_freeMem(virtual, size);
    //  Start+=size;
	for(uint32 i = virtual ; i <virtual+ size ; i+=PAGE_SIZE)
	{
		P[(i - USER_HEAP_START) / PAGE_SIZE] = 0;

	}


//cprintf("free is worked successfully");
	cprintf("free is done :) !");

}

void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
