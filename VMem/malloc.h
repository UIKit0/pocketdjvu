#pragma once

namespace siv_vm
{
    void     vm_free( void * pMem, int level );
    void *   vm_malloc( size_t bytes, int level );
    void *   vm_realloc( void * pMem, size_t bytes, int level );
    void *   vm_calloc( size_t n_elements, size_t elem_size, int level );
} // namespace siv_vm
