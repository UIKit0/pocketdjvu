#pragma once

class CSIPState : public SIPINFO
{
public:
  CSIPState() throw()
  {
      ZeroMemory( this, sizeof SIPINFO );
      cbSize = sizeof SIPINFO;
      
      m_bValid = SHSipInfo( SPI_GETSIPINFO, 0, this, 0 );
  }

  ~CSIPState() throw()
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
