#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>
//#include <time.h>
//clock_t start, end;
//double cpu_time_used;
uint32 nextFitPlace=KERNEL_HEAP_START;
uint32 * nextFit( uint32 size)
{
	//start = clock();
	uint32 ptrStartAllocation;
	size=ROUNDUP(size,PAGE_SIZE);
	uint32 noOfRequiredPages=size/PAGE_SIZE;
	uint32 freePagesCount =0;// count the free pages found
	struct Frame_Info* result ;
	for ( uint32 i =nextFitPlace;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
		{
			uint32 *ptrPageTable = NULL;
			result= get_frame_info(ptr_page_directory ,(void*)i,&ptrPageTable);
			if( result==NULL )
			  {
				freePagesCount++;
				if ( freePagesCount==noOfRequiredPages)
				{
					ptrStartAllocation= i-(noOfRequiredPages*PAGE_SIZE);
					ptrStartAllocation+= PAGE_SIZE;
					nextFitPlace= ptrStartAllocation;
				    return ( uint32 *)ptrStartAllocation;
				}
		      }// end outer if
		   else if ( result!=NULL)
		      {
			    freePagesCount=0;
		      }
	  }// end first  for
   freePagesCount=0;
   for ( uint32 i =KERNEL_HEAP_START;i<nextFitPlace ;i+=PAGE_SIZE)
   {
	   uint32 *ptrPageTable = NULL;
       result= get_frame_info(ptr_page_directory ,(void*)i,&ptrPageTable);
       if (result==NULL  )
          {
    	   freePagesCount++;
           if ( freePagesCount==noOfRequiredPages)
             {
        	   ptrStartAllocation= (i-(noOfRequiredPages*PAGE_SIZE));
               ptrStartAllocation+=PAGE_SIZE;
               nextFitPlace= ptrStartAllocation;
               return ( uint32 *)ptrStartAllocation;
             }
           }// end if
     else if( result!=NULL )
       	{
    	 freePagesCount=0;
       	}
 } // end second for
 return NULL;
 // end = clock();
 //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 } // function end*/

//clock_t start, end;
//double cpu_time_used;
struct bestFit
{
	uint32 noOfFreePages;
	uint32  stratVa;
} bstFit[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
struct bestFit BSTFIT[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
uint32 BESTFIT (uint32 size)
{
	//start = clock();
	 uint32 freePagesCount=0;
	 uint32 freeAddress = 0;
	 uint32 count=0;
	//loop to get the best fit place
	 for ( uint32 i=KERNEL_HEAP_START; i<KERNEL_HEAP_MAX; i+=PAGE_SIZE)
	 {
			uint32 *ptrPageTable = NULL;
			struct Frame_Info* result= get_frame_info(ptr_page_directory ,(void*)i,&ptrPageTable);
			if (result==NULL)
			{
				if(freeAddress == 0)
				{
					freeAddress = i;
					freePagesCount++;
				}
				else
				{
					freePagesCount++;
				}
			}// end outer if
			else if ( result!=NULL)
			{
				freePagesCount=0;
				freeAddress = 0;
				continue;
			}
			if (freePagesCount >= size)
			{
				bstFit[count].noOfFreePages=freePagesCount;
				bstFit[count].stratVa = freeAddress;
				++count;
			}
	 }// end for
	 //loop on the struct to get the best fit place
	 int cntr = 0;
	 if(count > 0)
	 {
		 BSTFIT[0].noOfFreePages = bstFit[0].noOfFreePages;
		 BSTFIT[0].stratVa = bstFit[0].stratVa;
		 for(int i = 1; i < count; ++i)
		 {
		 	if(bstFit[i].stratVa == BSTFIT[cntr].stratVa)
		 	{
		 		BSTFIT[cntr].noOfFreePages = bstFit[i].noOfFreePages;
		 	}
		 	else{
		 		cntr++;
		 		BSTFIT[cntr].noOfFreePages = bstFit[i].noOfFreePages;
		 		BSTFIT[cntr].stratVa = bstFit[i].stratVa;
		 	}
		 }
	 }
	 if(count > 0)
	 {
		 uint32 PagesCount=BSTFIT[0].noOfFreePages;
		 uint32 pageAddr = BSTFIT[0].stratVa;
		 for (uint32 i=1; i<cntr; i++)
		 {
		 	if (BSTFIT[i].noOfFreePages<PagesCount)
		 	{
		 	   PagesCount=BSTFIT[i].noOfFreePages;
		 	  pageAddr=BSTFIT[i].stratVa;
		    }
		  }// end for
	     return pageAddr;
	 }
	 return 0;
	 //end = clock();
	 // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
}// end bonus function

uint32 arr_KernalStrart[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
int arr_PageRound[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
int count = 0;
uint32 arr_size[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];

void* kmalloc(unsigned int size)
{
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 Round_pages = size/PAGE_SIZE ;
	uint32 allocationPlace ;
	struct Frame_Info * ptr_frame_info;
	int  ret_frame;
	if(isKHeapPlacementStrategyNEXTFIT())
		{
		  allocationPlace =(uint32) nextFit(size);
		}
	if(isKHeapPlacementStrategyBESTFIT())
		{
		  allocationPlace =(uint32) BESTFIT(Round_pages);
		}
	if ( allocationPlace ==0 )
		{
		  return NULL;
		}
	arr_KernalStrart[count] =( uint32 ) allocationPlace;
	arr_PageRound[count] = Round_pages;
	for (uint32 i =0;i<Round_pages;i++)
		{
			 ret_frame = allocate_frame(& ptr_frame_info);
		     ret_frame = map_frame(ptr_page_directory,ptr_frame_info, (void*)allocationPlace, PERM_WRITEABLE|PERM_PRESENT);
		     allocationPlace += PAGE_SIZE;
		}
	nextFitPlace=(uint32 )allocationPlace;
	arr_size[count]=size;
	count++;
	return (void *) arr_KernalStrart[count-1];
}

void kfree(void* virtual_address)
{
	uint32  va =  ( uint32 )virtual_address;
	uint32 noOfPages =0;
	uint32 indexFound=-1;
	for(int index1=0;index1<count;index1++)
		{
			if( (void *)arr_KernalStrart[index1]==virtual_address)
				{
				  indexFound=index1;
				  cprintf("found location %d\n ",indexFound);
				  for(int index2=0;index2<arr_PageRound[index1];index2++)
					{
						unmap_frame(ptr_page_directory,  (void *)va);
						va += PAGE_SIZE;
					}
					break;
				 }// end if
		}// end for
	 if(indexFound!=-1)
		{
		    cprintf("updating array\n");
			for ( uint32 i=indexFound;i<count-1 ;i++)
			    {
				   arr_KernalStrart[i]=  arr_KernalStrart[i+1];
				   arr_PageRound[i]= arr_PageRound[i+1];
			     }//end for
			count--;
		}//end if
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
		unsigned int Updated_virtual;
		unsigned int address_to_start=KERNEL_HEAP_START;
		while(address_to_start<KERNEL_HEAP_MAX)
		{
			Updated_virtual=kheap_physical_address(address_to_start);
			if(Updated_virtual==physical_address)
			{
				return address_to_start;
			}
			address_to_start+=PAGE_SIZE;
		}
		return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	uint32 va = virtual_address;
	uint32 *ptr_page_table = NULL;
	get_page_table(ptr_page_directory,(void*)va, &ptr_page_table);
	if(ptr_page_table == NULL)
			{
				return 0;
			}
	else
			{
				uint32 Index_page = ptr_page_table[PTX((void*)va)];
				return Index_page & KERNEL_HEAP_MAX;
			}
		return 0;
}
