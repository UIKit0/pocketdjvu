#include "../PocketDjvu/stdafx.h"

#include "./malloc.h"
#include <exception>

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

  //static wchar_t const * g_cSwapFileName = L"\\swap.map";
  static wchar_t const * g_cSwapFileName = L"\\SD Card\\file.map";
  //static wchar_t const * g_cSwapFileName = L"\\Storage Card\\file.map";
  //---------------------------------------------------------------------------
  class CMemInit
  {
  private:
    explicit CMemInit( wchar_t const * szSwapFileName, unsigned sizeMB = 88/*128*/ ) :
      m_baseAddr()
      , m_size(sizeMB*1024*1024)
      , m_err()
      , m_msp()
    {
      DWORD dwCreateDisposition = CREATE_ALWAYS;
      WIN32_FILE_ATTRIBUTE_DATA fad = {0};
      BOOL res = GetFileAttributesEx( szSwapFileName, GetFileExInfoStandard, &fad );
      if ( res )
      {
        if ( fad.nFileSizeLow >= m_size )
        {
          dwCreateDisposition = OPEN_EXISTING;
        }
      }

      HANDLE hF = CreateFile( szSwapFileName, GENERIC_READ|GENERIC_WRITE,
                              FILE_SHARE_READ,
                              NULL, dwCreateDisposition,
                              FILE_ATTRIBUTE_NORMAL |FILE_FLAG_NO_BUFFERING|FILE_FLAG_RANDOM_ACCESS,
                              NULL );
      if ( INVALID_HANDLE_VALUE == hF )
      {
        m_err = GetLastError();
        return;
      }

      HANDLE hFM = CreateFileMapping( hF, NULL, PAGE_READWRITE, 0, m_size, NULL );
      if ( !hFM )
      {
        m_err = GetLastError();
        CloseHandle( hF );
        return;
      }

      CloseHandle( hF );

      m_baseAddr = MapViewOfFile( hFM, FILE_MAP_WRITE, 0, 0, m_size );
      if ( !m_baseAddr )
      {
        m_err = GetLastError();
      }
      CloseHandle( hFM );
      
      m_msp = create_mspace_with_base( m_baseAddr, m_size, true );
    }

    ~CMemInit()
    {
      Destroy();
    }

    void Destroy()
    {
      if ( m_baseAddr )
      {
        if ( m_msp )
        {
          destroy_mspace( m_msp );
          m_msp = 0;
        }
        UnmapViewOfFile( m_baseAddr );
        m_baseAddr = 0;
      }
      m_bInited = false;
    }

  public:
    static CMemInit * GetInstance()
    {
      if ( !m_bInited )
      {
        ATL::CCritSecLock lock( m_cs );
        if ( !m_bInited )
        {
          m_instance = ::new( m_space ) CMemInit( g_cSwapFileName );
          if ( !m_instance->IsValid() )
          {
            m_instance->~CMemInit();
            m_instance = 0;
            // TODO: Show the critical error message here.
          }
          m_bInited = true;
        }
      }
      return m_instance;
    }

    bool IsValid()
    {
      return !!m_msp;
    }

    DWORD GetLastError()
    {
      return m_err;
    }

    operator mspace()
    {
      return m_msp;
    }

    void Enable( bool bEnable )
    {
      ::ATL::CCritSecLock lock( m_cs );
      m_bEnable = bEnable;
    }

  // DATA:
  private:
    LPVOID m_baseAddr;
    DWORD  m_size;
    DWORD  m_err;
    mspace m_msp;

    static bool volatile            m_bEnable;
    static bool volatile            m_bInited;
    static ::ATL::CCriticalSection  m_cs;
    static CMemInit *               m_instance;
    static char                     m_space[];
  };

  __declspec(selectany) bool volatile           CMemInit::m_bEnable = true;
  __declspec(selectany) bool volatile           CMemInit::m_bInited = false;
  __declspec(selectany) ::ATL::CCriticalSection CMemInit::m_cs;
  __declspec(selectany) CMemInit *              CMemInit::m_instance = 0;
  __declspec(selectany) char                    CMemInit::m_space[ sizeof CMemInit ];

  //---------------------------------------------------------------------------
  void vm_free( void * pMem )
  {
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi )
    {
      free( pMem );
      return;
    }
    mspace_free( *pmi, pMem );
  }

  void * vm_malloc( size_t bytes )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi )
    {
      p = malloc( bytes );
    }
    else
    {
      p = mspace_malloc( *pmi, bytes );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }

  void * vm_realloc( void * pMem, size_t bytes )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi )
    {
      p = realloc( pMem, bytes );
    }
    else
    {
      p = mspace_realloc( *pmi, pMem, bytes );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }

  void * vm_calloc( size_t n_elements, size_t elem_size )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi )
    {
      p = calloc( n_elements, elem_size );
    }
    else
    {
      p = mspace_calloc( *pmi, n_elements, elem_size );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }
} // namespace siv