#include "../PocketDjvu/stdafx.h"

#include "./malloc.h"

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
