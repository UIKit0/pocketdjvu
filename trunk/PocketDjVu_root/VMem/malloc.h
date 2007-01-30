#pragma once

extern "C"
{
  // Prototypes from dlmalloc.
  typedef void* mspace;
  size_t destroy_mspace(mspace msp);
  mspace create_mspace_with_base(void* base, size_t capacity, int locked);
  void* mspace_malloc(mspace msp, size_t bytes);
  void mspace_free(mspace msp, void* mem);
  void* mspace_realloc(mspace msp, void* mem, size_t newsize);
  void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);
  void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);
};