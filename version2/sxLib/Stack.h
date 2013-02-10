/********************************************************************
	created:	2012/04/05
	filename: 	Stack.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains stack class to store data in stack
*********************************************************************/
#ifndef GUARD_sxStack_HEADER_FILE
#define GUARD_sxStack_HEADER_FILE

#include "Memory.h"

/*!
this stack class doesn't use the copy constructor of class objects when it pushes them.
so it's suitable for pointers which has no copy constructor to decrease function calls and increase speed
*/
template<typename T>
class Stack
{
	SEGAN_STERILE_CLASS(Stack);

public:
	Stack( sint sampler = 0 )
		: m_item(null)
		, m_count(0)
		, m_size(0)
		, m_sampler(sampler)
	{
	}

	~Stack( void )
	{
		mem_free( m_item );
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
		mem_free( m_item );
		m_item = null;
		m_count = 0;
	}

	SEGAN_LIB_INLINE void SetCount( sint newCount )
	{
		m_count = newCount;
		_Realloc( m_count );
	}

	SEGAN_LIB_INLINE void SetSize( sint newSize )
	{
		m_sampler = newSize;
		_Realloc( newSize-1 );
	}

	SEGAN_LIB_INLINE sint Count( void ) const
	{
		return m_count;
	}

	SEGAN_LIB_INLINE bool IsEmpty( void ) const
	{
		return ( m_count == 0 );
	}

	SEGAN_LIB_INLINE void Push( const T& newItem )
	{
		_Realloc( ++m_count );
		m_item[m_count-1] = newItem;
	}

	SEGAN_LIB_INLINE void Pop( void )
	{
		if ( !m_count ) return;
		_Realloc( --m_count );
	}

	SEGAN_LIB_INLINE bool Pop( OUT T& Item )
	{
		if ( !m_count ) return false;
		Item = m_item[--m_count];
		_Realloc( m_count );
		return true;
	}

	SEGAN_LIB_INLINE T& Top( void )
	{
		sx_assert(m_count>0);
		return m_item[m_count-1];
	}


private:
	SEGAN_LIB_INLINE void _Realloc( sint newSize )
	{
		if ( m_sampler ) {
			if ( newSize > m_size || ( m_size - newSize ) > m_sampler ) {
				m_size = sint( newSize / m_sampler ) * m_sampler + m_sampler;
				mem_realloc( (void*&)m_item, m_size * sizeof(T) );
			}
		} else {
			m_size = newSize;
			mem_realloc( (void*&)m_item, m_size * sizeof(T) );
		}
	}

private:

	T*		m_item;		//  items in stack
	sint	m_count;	//  number of items in stack
	sint	m_size;		//	size of the whole stack
	sint	m_sampler;	//  use to sample memory to reduce allocation

};



/*!
this stack class doesn't use the copy constructor of class objects when it pushes them.
so it's suitable for pointers which has no copy constructor to decrease function calls and increase speed
*/
template<typename T, uint count>
class Stack_fix
{
public:
	Stack_fix( void ): m_count(0)
	{
		sx_assert( count > 0 );
#if _DEBUG
		memset( m_item, 0, sizeof(m_item) );
#endif
	}

	~Stack_fix( void )
	{
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
		m_count = 0;
#if _DEBUG
		memset( m_item, 0, sizeof(m_item) );
#endif
	}

	SEGAN_LIB_INLINE void SetCount( sint newCount )
	{
		if ( newCount >= count ) return;
		m_count = newCount;
	}

	SEGAN_LIB_INLINE sint Count( void ) const
	{
		return m_count;
	}

	SEGAN_LIB_INLINE bool IsEmpty( void ) const
	{
		return ( m_count == 0 );
	}

	SEGAN_LIB_INLINE void Push( const T& newItem )
	{
		if ( m_count >= count ) return;
		m_item[m_count++] = newItem;
	}

	SEGAN_LIB_INLINE void Pop( void )
	{
		if ( !m_count ) return;
		m_count--;
	}

	SEGAN_LIB_INLINE bool Pop( OUT T& Item )
	{
		if ( !m_count ) return false;
		sx_assert(m_count);
		Item = m_item[--m_count];
		return true;
	}

	SEGAN_LIB_INLINE T& Top( void )
	{
		sx_assert(m_count>0);
		return m_item[m_count-1];
	}

private:

	T		m_item[count];	//  items in stack
	sint	m_count;		//  number of items in stack

};


#endif	//	GUARD_Stack_HEADER_FILE
