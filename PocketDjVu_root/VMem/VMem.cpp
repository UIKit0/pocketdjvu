#include "../PocketDjvu/stdafx.h"

#include "./malloc.h"

extern "C"
{
  // Prototypes from dlmalloc.
  typedef void* mspace;
  size_t destroy_mspace(mspace msp);
  mspace create_mspace_with_base(void* base, size_t capacity, int locked);
  void*  mspace_malloc(mspace msp, size_t bytes);
  void   mspace_free(mspace msp, void* mem);
  void*  mspace_realloc(mspace msp, void* mem, size_t newsize);
  void*  mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);
  void*  mspace_memalign(mspace msp, size_t alignment, size_t bytes);
};

namespace siv
{

  void     vm_free( void * pMem )
  {
    free(pMem);
  }

  void *   vm_malloc( size_t bytes )
  {
    return malloc(bytes);
  }

  void *   vm_realloc( void * pMem, size_t bytes )
  {
    return realloc( pMem, bytes );
  }

  void *   vm_calloc( size_t n_elements, size_t elem_size )
  {
    return calloc( n_elements, elem_size );
  }
} // namespace siv

#if 0
wchar_t const * fileName = L"\\SD Card\\file.map";

  //CWaitCursor wc;
  DWORD err = 0;

  HANDLE hF = CreateFile( fileName, GENERIC_READ|GENERIC_WRITE,
                          FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
  if ( INVALID_HANDLE_VALUE == hF )
  {
    err = GetLastError();
    wInfo.SetWindowText( L"CreateFile is failed." );
    return;
  }

  const DWORD numBytesFM = mb * 1024*1024;
  HANDLE hFM = CreateFileMapping( hF, NULL, PAGE_READWRITE, 0, numBytesFM, L"fileName" );
  if ( !hFM )
  {
    err = GetLastError();
    CloseHandle( hF );
    wInfo.SetWindowText( L"CreateFileMapping is failed." );
    return;
  }

  CloseHandle( hF );

  LPVOID addr = MapViewOfFile( hFM, FILE_MAP_WRITE, 0, 0, numBytesFM );
  CloseHandle( hFM );
  if ( !addr )
  {
    err = GetLastError();
    wInfo.SetWindowText( L"MapViewOfFile is failed." );
    return;
  }

  unsigned i=0;
  __try
  {
    BYTE * pCh = (BYTE *)addr;
    for ( i=0;  i < numBytesFM; ++i )
    {
      pCh[i] = 'i';
    }

    TestDLmalloc( addr, numBytesFM );
  }
  __except( EXCEPTION_EXECUTE_HANDLER )
  {
    err = GetLastError();
    UnmapViewOfFile( addr );
    wInfo.SetWindowText( L"Exception!" );
    return;
  }
#endif
