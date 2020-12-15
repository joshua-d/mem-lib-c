#include "my_mem_allocator.h"

extern BLOCK_HDR* fl_head;

void my_free(void *ptr)
{    
  BLOCK_HDR* hdr = (BLOCK_HDR*)(ptr - sizeof(BLOCK_HDR));

  if (hdr->next != (void*)MAGIC_NUM) {
    printf("Error: invalid memory block\n");
    return;
  }

  if (fl_head == NULL) {
    fl_head = hdr;
    hdr->next = NULL;
    return;
  }

  //Find where this block should go in free list
  BLOCK_HDR* current_block = fl_head;
  BLOCK_HDR* prev_block = NULL;
  
  while (current_block != NULL) {
    if (current_block > hdr)
      break;

    prev_block = current_block;
    current_block = current_block->next;
  }

  if (current_block == NULL) {
    //past end of free list
    hdr->next = NULL;
    prev_block->next = (void*)hdr;
  }
  else if (current_block == fl_head) {
    //before beginning of free list
    hdr->next = fl_head;
    fl_head = hdr;
  }
  else {
    //in free list
    hdr->next = prev_block->next;
    prev_block->next = (void*)hdr;
  }

  //coalesce
  if (hdr->next != NULL && hdr->next == (void*)hdr + sizeof(BLOCK_HDR) + hdr->size) {
    hdr->size += sizeof(BLOCK_HDR) + ((BLOCK_HDR*)(hdr->next))->size;
    hdr->next = ((BLOCK_HDR*)(hdr->next))->next;
  }
  if (hdr != fl_head && hdr == (void*)prev_block + sizeof(BLOCK_HDR) + prev_block->size) {
    prev_block->size += sizeof(BLOCK_HDR) + hdr->size;
    prev_block->next = hdr->next;
  }
}
