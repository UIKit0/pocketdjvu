#pragma once

class CSIPState : protected SIPINFO
{
public:
  CSIPState()
  {
      ZeroMemory( this, sizeof SIPINFO );
      cbSize = sizeof SIPINFO;
      
      m_bValid = SHSipInfo( SPI_GETSIPINFO, 0, this, 0 );
  }

  ~CSIPState()
  {
    if( IsValid() )
    {
      SHSipInfo( SPI_SETSIPINFO, 0, this, 0 );
    }
  }
  bool IsValid()
  {
    return !!m_bValid;
  }
private:
  BOOL m_bValid;
};
