#include "my_mem_allocator.h"

extern ALLOC_POLICY alloc_policy;

void *my_realloc(void *ptr, size_t size)
{
  BLOCK_HDR* hdr = (BLOCK_HDR*)(ptr - sizeof(BLOCK_HDR));

  if (hdr->next != (void*)MAGIC_NUM) {
    printf("Error: invalid memory block\n");
    return NULL;
  }

  if (size > hdr->size) {
  
    //Alloc new block
    ALLOC_POLICY old_policy = alloc_policy;
    set_alloc_policy(AP_FIRST);
    void* new_block = my_malloc(size);
    set_alloc_policy(old_policy);

    if (new_block == NULL) {
      return ptr;
    }

    //Copy contents
    for (unsigned i = 0; i < hdr->size; i++) {
      ((char*)new_block)[i] = ((char*)ptr)[i];
    }

    //Free old block
    my_free(ptr);
    
    return new_block;
    
  }
  else {
    if (hdr->size - size >= sizeof(BLOCK_HDR) + 1) {

      //Form a mock allocd block
      BLOCK_HDR* mock_block = (void*)hdr + sizeof(BLOCK_HDR) + size;
      mock_block->size = hdr->size - size - sizeof(BLOCK_HDR);
      mock_block->next = (void*)MAGIC_NUM;

      //Free it
      my_free((void*)mock_block + sizeof(BLOCK_HDR));

      hdr->size = size;
    }
    
    return ptr;
  }
  
}
