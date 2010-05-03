#pragma once

class CNofyIcon
{
public:
  CNofyIcon() : m_bInstalled(), m_ni()
  {
  }

  void Setup( HWND hwnd, UINT msgID, int iconID ) 
  {
    m_ni.cbSize           = sizeof m_ni;
    m_ni.hIcon            = ::LoadIcon( ::GetModuleHandle(0), MAKEINTRESOURCE( iconID ) );
    m_ni.hWnd             = hwnd;
//    m_ni.szTip            = L"PocketDjVu";
    m_ni.uCallbackMessage = msgID;
    m_ni.uFlags           = NIF_ICON|NIF_MESSAGE;//|NIF_TIP;
    m_ni.uID              = 12 + msgID;    
  }

  bool Install()
  {
    if ( !m_ni.hIcon )
      return false;

    m_bInstalled = !!Shell_NotifyIcon( NIM_ADD, &m_ni );
    return m_bInstalled;
  }

  bool Deinstall()
  {
    if ( !m_bInstalled )
      return false;

    bool res = !!Shell_NotifyIcon( NIM_DELETE, &m_ni );
    if ( res )
      m_bInstalled = true;
    return res;
  }

  bool IsInstalled()
  {
    return m_bInstalled;
  }

  virtual ~CNofyIcon()
  {
    if ( m_bInstalled )
      Deinstall();
  }

private:
  bool m_bInstalled;
  NOTIFYICONDATA m_ni;
};