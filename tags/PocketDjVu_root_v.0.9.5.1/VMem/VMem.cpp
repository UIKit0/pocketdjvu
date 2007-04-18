#include "../PocketDjvu/stdafx.h"

#include <exception>
#include "./malloc.h"

#include "../PocketDjvu/misc.h"
#include "../PocketDjvu/constants.h"
#include "../PocketDjvu/resource.h"

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

  static WTL::CString g_SwapFileName( SWAP_FILENAME );
  static DWORD g_SizeMB = 64;
  //---------------------------------------------------------------------------
  class CMemInit
  {
  private:
    //.........................................................................
    explicit CMemInit( wchar_t const * szSwapFileName ) :
      m_baseAddr()
      , m_size(g_SizeMB*1024*1024)
      , m_err()
      , m_msp()
    {
      DWORD dwCreateDisposition = CREATE_ALWAYS;
      WIN32_FILE_ATTRIBUTE_DATA fad = {0};
      BOOL res = GetFileAttributesEx( szSwapFileName, GetFileExInfoStandard, &fad );
      if ( res )
      {
        dwCreateDisposition = OPEN_EXISTING;        
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

      if ( OPEN_EXISTING == dwCreateDisposition )
      {
        SetFilePointer( hF, m_size, NULL, FILE_BEGIN );
        if( NO_ERROR == GetLastError() )
        {
          if ( !SetEndOfFile( hF ) )
          {
            m_err = GetLastError();
            CloseHandle( hF );
            return;
          }
        }
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
    //.........................................................................
    ~CMemInit()
    {
      Destroy();
    }
    //.........................................................................
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
    //.........................................................................
    static void ShowWarning()
    {
      WTL::CString szWarning;
      szWarning.LoadString( IDS_WARNING );

      WTL::CString szText;
      szText.LoadString( IDS_CAN_NOT_CREATE_MMFILE );
      ShowNotification( 0, szWarning, szText );
    }
    //.........................................................................
    static bool ReadRegValues()
    {
      ATL::CRegKey rk;

      bool res = ERROR_SUCCESS == rk.Open( HKEY_CURRENT_USER, APP_REG_PATH_VM );
      if ( !res )
      {
        return false;
      }

      g_SizeMB = 0;
      res = (ERROR_SUCCESS == rk.QueryDWORDValue( L"SizeMB", g_SizeMB )) && g_SizeMB;
      if ( !res || g_SizeMB < g_cSwapLowLimitMB || g_cSwapUpperLimitMB < g_SizeMB )
      {
        return false;
      }

      
      ULONG nChars = MAX_PATH;
      res = 
        ERROR_SUCCESS == rk.QueryStringValue( L"SwapFileName", 
                                              g_SwapFileName.GetBufferSetLength(nChars),
                                              &nChars );
      g_SwapFileName.ReleaseBuffer();
      if ( !res )
      {
        return false;
      }

      DWORD l=0;
      res = (ERROR_SUCCESS == rk.QueryDWORDValue( L"Level", l )) && l;
      m_level = l;

      return res;
    }

  public:
    //.........................................................................
    static CMemInit * GetInstance()
    {
      if ( !m_bInited )
      {
        ATL::CCritSecLock lock( m_cs );
        if ( !m_bInited )
        {
          m_bInited = true;
          if ( ReadRegValues() )
          {
            m_instance = ::new( m_space ) CMemInit( g_SwapFileName );
            if ( !m_instance->IsValid() )
            {
              m_instance->~CMemInit();
              m_instance = 0;
              
              ShowWarning();
            }
          }          
        }
      }
      return m_instance;
    }
    //.........................................................................
    ATL::CCriticalSection & GetCS()
    {
      return m_cs;
    }
    //.........................................................................
    bool IsValid()
    {
      return !!m_msp;
    }
    //.........................................................................
    DWORD GetLastError()
    {
      return m_err;
    }
    //.........................................................................
    operator mspace()
    {
      return m_msp;
    }
    //.........................................................................
    int GetLevel()
    {
      return m_level;
    }

  // DATA:
  private:
    LPVOID m_baseAddr;
    DWORD  m_size;
    DWORD  m_err;
    mspace m_msp;

    static bool volatile            m_bInited;
    static ::ATL::CCriticalSection  m_cs;
    static CMemInit *               m_instance;
    static char                     m_space[];
    static int                      m_level;
  };

  //---------------------------------------------------------------------------
  bool volatile           CMemInit::m_bInited = false;
  ::ATL::CCriticalSection CMemInit::m_cs;
  CMemInit *              CMemInit::m_instance = 0;
  char                    CMemInit::m_space[ sizeof CMemInit ];
  int                     CMemInit::m_level = 0;

  //---------------------------------------------------------------------------
  void vm_free( void * pMem, int level )
  {
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi || pmi->GetLevel() < level )
    {
      free( pMem );
      return;
    }
    ATL::CCritSecLock lock( pmi->GetCS() );
    mspace_free( *pmi, pMem );
  }

  void * vm_malloc( size_t bytes, int level )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi || pmi->GetLevel() < level )
    {
      p = malloc( bytes );
    }
    else
    {
      ATL::CCritSecLock lock( pmi->GetCS() );
      p = mspace_malloc( *pmi, bytes );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }

  void * vm_realloc( void * pMem, size_t bytes, int level )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi || pmi->GetLevel() < level )
    {
      p = realloc( pMem, bytes );
    }
    else
    {
      ATL::CCritSecLock lock( pmi->GetCS() );
      p = mspace_realloc( *pmi, pMem, bytes );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }

  void * vm_calloc( size_t n_elements, size_t elem_size, int level )
  {
    void * p = 0;
    CMemInit * pmi = CMemInit::GetInstance();
    if ( !pmi || pmi->GetLevel() < level )
    {
      p = calloc( n_elements, elem_size );
    }
    else
    {
      ATL::CCritSecLock lock( pmi->GetCS() );
      p = mspace_calloc( *pmi, n_elements, elem_size );
    }

    if ( !p )
    {
      throw std::bad_alloc( "Not enought memory." );
    }
    return p;
  }
} // namespace siv