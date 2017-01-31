/********************************************************************
	created:	2012/02/15
	filename: 	Queue.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Queue container classes that 
				use double linked List to hold data.
*********************************************************************/
#ifndef GUARD_Queue_HEADER_FILE
#define GUARD_Queue_HEADER_FILE

#include "Def.h"

/*! 
Queue container classes that use double linked List to hold data.
*/
template<typename T>
class Queue
{
	sx_sterile_class(Queue);

public:

	struct Iterator
	{
		T			data;
		Iterator*	next;
		Iterator*	prev;
	};

	Queue( void ): m_back(null), m_front(null), m_count(0)
	{
	}

	~Queue( void )
	{
		clear();
	}

	SEGAN_LIB_INLINE void clear( void )
	{
 		while ( m_count )
 			_delete_node( m_back );
	}

	SEGAN_LIB_INLINE void push( const T& newItem )
	{
		Iterator* node = (Iterator*)sx_mem_alloc( sizeof(Iterator) );
		node->data = newItem;
		node->prev = null;
		node->next = m_back;
		if ( m_back ) m_back->prev = node;
		m_back = node;
		if ( !m_front ) m_front = node;
		m_count++;
	}

	SEGAN_LIB_INLINE void pop( void )
	{
		if ( m_front )
			_delete_node( m_front );
	}

	SEGAN_LIB_INLINE bool pop( OUT T& Item )
	{
		if ( m_front )
		{
			Item = m_front->data;
			_delete_node( m_front );
			return true;
		}
		return false;
	}

	SEGAN_LIB_INLINE T& top( void )
	{
		sx_assert( m_front );
		return m_front->data;
	}

	SEGAN_LIB_INLINE T& front( void )
	{
		sx_assert( m_front );
		return m_front->data;
	}

	SEGAN_LIB_INLINE T& back( void )
	{
		sx_assert( m_back );
		return m_back->data;
	}

private:

	SEGAN_LIB_INLINE void _delete_node( Iterator* node )
	{
		if ( node->prev )		node->prev->next = node->next;
		if ( node->next )		node->next->prev = node->prev;
		if ( node == m_front )	m_front = node->prev;
		if ( node == m_back )	m_back = node->next;
		sx_mem_free( node );
		m_count--;
	}


	Iterator*	m_back;
	Iterator*	m_front;

public:
	sint		m_count;

};


#endif	//	GUARD_Queue_HEADER_FILE
