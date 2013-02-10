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

#include "Memory.h"

/*! 
Queue container classes that use double linked List to hold data.
*/
template<typename T>
class Queue
{
	SEGAN_STERILE_CLASS(Queue);

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
		Clear();
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
 		while ( m_count )
 			_DeleteNode( m_back );
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
		Iterator* node = (Iterator*)mem_alloc( sizeof(Iterator) );
		node->data = newItem;
		node->prev = null;
		node->next = m_back;
		if ( m_back ) m_back->prev = node;
		m_back = node;
		if ( !m_front ) m_front = node;
		m_count++;
	}

	SEGAN_LIB_INLINE void Pop( void )
	{
		if ( m_front )
			_DeleteNode( m_front );
	}

	SEGAN_LIB_INLINE bool Pop( OUT T& Item )
	{
		if ( m_front )
		{
			Item = m_front->data;
			_DeleteNode( m_front );
			return true;
		}
		return false;
	}

	SEGAN_LIB_INLINE T& Top( void )
	{
		sx_assert( m_front );
		return m_front->data;
	}

	SEGAN_LIB_INLINE T& Front( void )
	{
		sx_assert( m_front );
		return m_front->data;
	}

	SEGAN_LIB_INLINE T& Back( void )
	{
		sx_assert( m_back );
		return m_back->data;
	}

private:

	SEGAN_LIB_INLINE void _DeleteNode( Iterator* node )
	{
		if ( node->prev )		node->prev->next = node->next;
		if ( node->next )		node->next->prev = node->prev;
		if ( node == m_front )	m_front = node->prev;
		if ( node == m_back )	m_back = node->next;
		mem_free( node );
		m_count--;
	}


	Iterator*	m_back;
	Iterator*	m_front;
	sint		m_count;

};


#endif	//	GUARD_Queue_HEADER_FILE
