#pragma once

#include "smart_ptr.h"

//-----------------------------------------------------------------------------
struct IPage : public siv_hlpr::CRefCntr<>
{
  virtual ~IPage() {};

  virtual WTL::CBitmap const & GetBmp() const = 0;
  virtual int GetPageIndex() const = 0;
  virtual WTL::CRect const & GetRect() const = 0;
  virtual void GetImgSize( int & o_width, int & o_height  ) const = 0;
  virtual void Scroll( int dx, int dy ) = 0;

  virtual bool LoadBmpSync() = 0;
  virtual bool LoadBmpAsync() = 0;

  virtual void Draw( WTL::CDCHandle dc ) = 0;
};

//-----------------------------------------------------------------------------
typedef siv_hlpr::CSimpSPtr<IPage> PagePtr;
