#pragma once
#ifndef SMART_PTR_H_
#	define SMART_PTR_H_

#include <assert.h>

namespace siv_hlpr
{
	//--------------------------------------------------------------------------
	// Simple policy class to increment/decrement integers.
	struct CSimplIncDec
	{
		static void Inc( unsigned & i ) { ++i; }
		static void Dec( unsigned & i ) { --i; }
	};
	//--------------------------------------------------------------------------
	/** Reference counter interface. 
	*/
	class IRefCntr
	{
	public:
		virtual unsigned AddRef() const = 0;
		virtual unsigned Release() const = 0;
		virtual unsigned GetCnt() const = 0;
	};

	//--------------------------------------------------------------------------
	/**	This helper class implements IRefCntr.
	 *	Base is the user's class that derives from IRefCntr and whatever
	 *	interfaces (which derived from IRefCntr too) the user wants to support
	 *	on the object.
	 */
	template <class Base, class CNTPolycy=siv_hlpr::CSimplIncDec>
	class CRefCntrObj : public Base
	{
	public:
		CRefCntrObj() : m_cnt() {}
		virtual ~CRefCntrObj() {}

		virtual unsigned AddRef() const
		{
			CNTPolycy::Inc( m_cnt );
			assert( m_cnt ); // anti overflow
			return m_cnt;
		}

		virtual unsigned Release() const
		{
			assert( m_cnt );
			CNTPolycy::Dec( m_cnt );
			if ( !m_cnt )
			{
				delete this;
				return 0;
			}
			return m_cnt;
		}
		virtual unsigned GetCnt() const { return m_cnt; }
	private:
		mutable unsigned m_cnt;
	};

	//--------------------------------------------------------------------------
	/** Reference counter logic (as implemented IRefCntr interface). 
	*/
	template <typename CNTPolycy=siv_hlpr::CSimplIncDec>
	class CRefCntr : public IRefCntr
	{
	public:
		CRefCntr() : m_cnt() {} // explicit zeroing
		CRefCntr( CRefCntr const& ) { m_cnt=0; } // explicit zeroing
		virtual ~CRefCntr() {}

		CRefCntr& operator = (CRefCntr const& r)
		{
			// skip counter copy.
			return *this;
		}

		virtual unsigned AddRef() const
		{
			CNTPolycy::Inc( m_cnt );
			assert( m_cnt ); // anti overflow
			return m_cnt;
		}

		virtual unsigned Release() const
		{
			assert( m_cnt );
			CNTPolycy::Dec( m_cnt );
			if ( !m_cnt )
			{
				delete this;
				return 0;
			}
			return m_cnt;
		}

		virtual unsigned GetCnt() const { return m_cnt; }

	private:
		mutable unsigned m_cnt;
	};

  //==============================================================================
  /** Simple intrusive (based on embedded reference counter) smart pointer class.
   * T - type on which pointers wiil be point.
   * The reference counting logic can be implemented by derivation from CRefCntr<...>.
   */
  template <typename T>
  class CSimpSPtr
  {
  private:
	  class CIntBool
	  {
	  };
	  typedef CIntBool* CIntBoolPtr;

  public:
	  CSimpSPtr() : m_ptr() {}

	  /**	Constructor.
	  */
	  CSimpSPtr( CSimpSPtr const& p )
	  {
		  m_ptr = p.m_ptr;
		  if ( m_ptr )
		  {
			  m_ptr->AddRef();
		  }
	  }

	  template <class U>
	  CSimpSPtr( CSimpSPtr<U> const& p )
	  {
		  m_ptr = p.m_ptr;
		  if ( m_ptr )
		  {
			  m_ptr->AddRef();
		  }
	  }

	  /**	Constructor.
	  *	\b explicit to prevent unexpected ordinary pointer conversion 
	  *	to smartpointer while assignment operator.
	  */
	  explicit CSimpSPtr( T * p )
	  {
		  m_ptr = p;
		  if ( m_ptr )
		  {
			  m_ptr->AddRef();
		  }
	  }

	  ~CSimpSPtr()
	  {
		  if ( m_ptr )
		  {
			  m_ptr->Release(); 
		  }
	  }

  /** Disabled to prevent unexpected ordinary pointer conversion to smartpointer.
	  CSimpSPtr const & operator =( T * p ) const
	  {
		  if ( m_ptr != p )
		  {
			  if ( m_ptr )
			  {
				  m_ptr->Release();
				  m_ptr = 0;
			  }
			  m_ptr = p;
			  m_ptr->AddRef();
		  }
		  return *this;
	  }
  **/	

	  CSimpSPtr const & operator =( CSimpSPtr const & ref ) const
	  {
		  if ( m_ptr != ref.m_ptr )
		  {
			  if ( m_ptr )
			  {
				  m_ptr->Release();
				  m_ptr = 0;
			  }
			  m_ptr = ref.m_ptr;
			  if ( m_ptr )
			  {
				  m_ptr->AddRef();
			  }
		  }
		  return *this;
	  }

	  T & operator *() const
	  {
		  return *m_ptr;
	  }

	  T * operator ->() const
	  {
		  return m_ptr;
	  }

	  T * GetPtr() const
	  {
		  return m_ptr;
	  }

	  void Reset( T * p=0 ) const
	  {
		  if ( m_ptr != p )
		  {
			  if ( m_ptr )
			  {
				  m_ptr->Release();
				  m_ptr = 0;
			  }
			  m_ptr = p;
			  if ( m_ptr )
			  {
				  m_ptr->AddRef();
			  }
		  }
	  }

	  friend bool operator == ( CSimpSPtr const& ref1, CSimpSPtr const& ref2 )
	  {
		  return ref1.m_ptr == ref2.m_ptr;
	  }

	  friend bool operator != ( CSimpSPtr const& ref1, CSimpSPtr const& ref2 )
	  {
		  return !(ref1 == ref2);
	  }

	  friend bool operator == ( CSimpSPtr const& ref1, T const * ptr2 )
	  {
		  return ref1.m_ptr == ptr2;
	  }
	  friend bool operator != ( CSimpSPtr const& ref1, T const * ptr2 )
	  {
		  return ref1.m_ptr != ptr2;
	  }
	  friend bool operator == ( T const * ptr1, CSimpSPtr const& ref2 )
	  {
		  return ptr1 == ref2.m_ptr;
	  }
	  friend bool operator != ( T const * ptr1, CSimpSPtr const& ref2 )
	  {
		  return ptr1 != ref2.m_ptr;
	  }

	  /// The operator "<" is very important for STL associative containers!
	  friend bool operator < ( CSimpSPtr const& ref1, CSimpSPtr const& ref2 )
	  {
		  return ref1.m_ptr < ref2.m_ptr;
	  }

	  bool operator !() const
	  {
		  return !m_ptr;
	  }

	  operator CIntBoolPtr() const
	  {
		  return CIntBoolPtr(m_ptr);
	  }

  private:
      template<class U> friend class CSimpSPtr;

	  mutable T * m_ptr;
  };

} // namespace siv_hlpr
#endif // SMART_PTR_H_
