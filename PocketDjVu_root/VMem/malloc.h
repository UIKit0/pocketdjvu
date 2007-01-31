#pragma once

namespace siv
{
  void     vm_free( void * pMem );
  void *   vm_malloc( size_t bytes );
  void *   vm_realloc( void * pMem, size_t bytes );
  void *   vm_calloc( size_t n_elements, size_t elem_size );
} // namespace siv
