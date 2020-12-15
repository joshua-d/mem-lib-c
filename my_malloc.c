#include "my_mem_allocator.h"

extern BLOCK_HDR* fl_head;
extern ALLOC_POLICY alloc_policy;

void* alloc_block(BLOCK_HDR* current_block, BLOCK_HDR* prev_block, size_t size) {

  if (current_block->size - size > sizeof(BLOCK_HDR)) {  //Split block

    size_t total_size = current_block->size;

    //Set allocd block size
    current_block->size = size;

    //Set new free block in location
    BLOCK_HDR* new_free_block = (BLOCK_HDR*)((void*)current_block + sizeof(BLOCK_HDR) + current_block->size);
    new_free_block->size = total_size - size - sizeof(BLOCK_HDR);
    new_free_block->next = current_block->next;

    //Replace next with magic num
    current_block->next = (void*)MAGIC_NUM;

    //Update free list
    if (prev_block == NULL)
      fl_head = new_free_block;
    else
      prev_block->next = (void*)new_free_block;
	    
  }
  else {
    //Update free list
    if (prev_block == NULL)
      fl_head = current_block->next;
    else 
      prev_block->next = current_block->next;

    //Set allocd block next to magic num
    current_block->next = (void*)MAGIC_NUM;
  }

  return (void*)current_block + sizeof(BLOCK_HDR);

}

void *my_malloc(size_t size)
{

  if (size < 1) {
    printf("Error: cannot allocate 0 bytes of memory\n");
    return NULL;
  }
  
  if (fl_head == NULL) {
    printf("Error: no free space available\n");
    return NULL;
  }

  //Search free list for valid free block
  BLOCK_HDR* current_block = fl_head;
  BLOCK_HDR* prev_block = NULL;
    
  if (alloc_policy == AP_FIRST) {
    while (current_block != NULL) {
      if (current_block->size >= size) {
	    //Found block
	    return alloc_block(current_block, prev_block, size);
      }

      prev_block = current_block;
      current_block = current_block->next;
    }
  }
  else {  //Best fit
      
    BLOCK_HDR* current_best = fl_head;
    BLOCK_HDR* current_best_prev = NULL;
    size_t current_best_size;
      
    if (fl_head->size >= size)
      current_best_size = fl_head->size;
    else
      current_best_size = 0;

    //Get best fit block
    while (current_block != NULL) {
      if (current_block->size >= size && (current_block->size < current_best_size || current_best_size == 0)) {
	current_best_size = current_block->size;
	current_best = current_block;
	current_best_prev = prev_block;
      }

      prev_block = current_block;
      current_block = current_block->next;
    }

    if (current_best_size != 0)
      return alloc_block(current_best, current_best_prev, size);
  }

  printf("Error: no free block large enough\n");
  return NULL;
    
}
