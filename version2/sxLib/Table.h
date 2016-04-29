/********************************************************************
	created:	2016/4/29
	filename: 	Table.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a fixed sized indexed table class
				to access data by specified index. Useful for hash
				tables as base container. It's allocation free in 
				add/remove operations.
*********************************************************************/
#ifndef DEFINED_Table
#define DEFINED_Table

#include "Def.h"

template<typename T>
class Table
{
	SEGAN_STERILE_CLASS(Table);
public:

	Table( uint capacity = 0 )
		: m_slots(null)
		, m_stack(null)
		, m_numfree(0)
		, m_size(0)
	{
		set_size( capacity );
	}
	
	~Table( void )
	{
		mem_free(m_slots);
		mem_free(m_stack);
	}

	//! set a new capacity for the manager
	void set_size( uint cap )
	{
		if ( cap < 1 )
		{
			mem_free( m_slots );
			mem_free( m_stack );
			m_size = 0;
			m_numfree = 0;
			return;
		}

		// reallocate needed memory
		m_slots = (T*)mem_realloc( m_slots, cap * sizeof(T) );
		m_stack = (uint*)mem_realloc( m_stack, cap * sizeof(uint) );

		// set zero allocated memory
		if ( cap > m_size )
			mem_set( &m_slots[m_size], 0, (cap - m_size) * sizeof(T) );
		m_size = cap;

		// fill out stack by free slots
		m_numfree = 0;
		for ( int i = m_size - 1; i >= 0; --i )
			if ( !m_slots[i] )
				m_stack[m_numfree++] = i;
	}

	//! return current capacity
	SEGAN_LIB_INLINE uint size( void )
	{
		return m_size;
	}

	//! return current number of items in container
	SEGAN_LIB_INLINE uint count( void )
	{
		return m_size - m_numfree;
	}

	//! add a new item to the table and return the index of that
	SEGAN_LIB_INLINE uint add( const T& item )
	{
		sx_assert(m_numfree);
		uint index = m_stack[--m_numfree];
		m_slots[index] = item;
		return index;
	}

	//! return an item specified by index. return 0 if item not found
	SEGAN_LIB_INLINE T& get( uint index )
	{
		sx_assert(index < m_size);
		return m_slots[index];
	}

	//! return the index of an item from the array. return -1 of no item found
	SEGAN_LIB_INLINE uint index_of( const T& item )
	{
		for ( uint i = 0; i < m_size; ++i )
			if ( m_slots[i] == item )
				return i;
		return -1;
	}

	//! remove a session from the manager by index
	SEGAN_LIB_INLINE bool remove( const T& item )
	{
		uint index = index_of( item );
		if ( index == (uint)-1 ) return false;
		remove_index( index );
		return true;
	}

	//! remove a session from the manager by index
	SEGAN_LIB_INLINE void remove_index( uint index )
	{
		sx_assert(index < m_size);
		m_slots[index] = null;
		m_stack[m_numfree++] = index;
	}

public:
	T*			m_slots;			//! item slots in table
	uint		m_size;				//! capacity of the container

private:
	uint*		m_stack;			//! indices of free slots
	uint		m_numfree;			//! number of free slots in stack
};

#endif // DEFINED_Table

