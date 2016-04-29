/********************************************************************
	created:	2012/04/03
	modified:	2013/01/15
	filename: 	Array.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain array class
*********************************************************************/
#ifndef GUARD_Array_HEADER_FILE
#define GUARD_Array_HEADER_FILE

#include "Def.h"

/*! 
this array class doesn't use the copy constructor of class objects when it pushes them.
so it's suitable for pointers which has no copy constructor to decrease function calls and increase speed
NOTE : Compare function should return -1:less  0:equal  +1:more
*/
template<typename T>
class Array
{
	SEGAN_STERILE_CLASS(Array);

public:
	typedef sint (*compFunc)( const T& item1, const T& item2 );


	Array( sint sampler = 0 )
		: m_item(null)
		, m_count(0)
		, m_size(0)
		, m_sampler(sampler)
	{
	}

	~Array( void )
	{
		mem_free( m_item );
	}

	SEGAN_LIB_INLINE void clear( void )
	{
		_realloc( 0 );
		m_count = 0;
	}

	SEGAN_LIB_INLINE bool is_empty( void )
	{
		return ( m_count == 0 );
	}

	SEGAN_LIB_INLINE void set_count( const sint newCount )
	{
		m_count = newCount;
		_realloc( m_count );
	}

	SEGAN_LIB_INLINE void set_size( const sint newSize )
	{
		m_sampler = newSize;
		_realloc( newSize-1 );
	}

	SEGAN_LIB_INLINE void push_back( const T& newItem )
	{
		_realloc( ++m_count );
		m_item[m_count-1] = newItem;
	}

	SEGAN_LIB_INLINE void push_front( const T& newItem )
	{
		_realloc( ++m_count );
		for ( sint i = m_count-1; i>0; i-- )
			m_item[i] = m_item[i-1];
		m_item[0] = newItem;
	}

	SEGAN_LIB_INLINE void insert( const sint index, const T& newItem )
	{
		sx_assert(index>=0 && index<=m_count);
		_realloc( ++m_count );
		for ( sint i = m_count-1; i>index; i-- )
			m_item[i] = m_item[i-1];
		m_item[index] = newItem;
	}

	SEGAN_LIB_INLINE bool remove( const T& item, const bool keep_order = true )
	{
		sint i = index_of( item );
		if ( i < 0 ) return false;
		remove_index( i, keep_order );
		return true;
	}

	SEGAN_LIB_INLINE void remove_index( const sint index, const bool keep_order = true )
	{
		sx_assert(index>=0 && index<m_count);
		if (keep_order)
		{
			for (sint i = index; i < m_count - 1; ++i)
				m_item[i] = m_item[i + 1];
		}
		else m_item[index] = m_item[m_count - 1];
		_realloc( --m_count );
	}

	SEGAN_LIB_INLINE void swap( const sint index1, const sint index2 )
	{
		sx_assert(index1>=0 && index1<m_count && index2>=0 && index2<m_count);
		if ( index1 == index2 ) return;
		T _tmp = m_item[index1];
		m_item[index1] = m_item[index2];
		m_item[index2] = _tmp;
	}

	SEGAN_LIB_INLINE void sort( compFunc cmpFunc = null )
	{
		if ( m_count > 1 ) quick_sort( cmpFunc, 0, m_count-1 );
	}

	//! return the index of an item from the array. return -1 of no item found
	SEGAN_LIB_INLINE sint index_of( const T& item )
	{
		for ( sint i=0; i<m_count; i++ ) {
			if ( m_item[i] == item ) {
				return i;
			}
		}
		return -1;
	}

	SEGAN_LIB_INLINE T& at( const sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE T& operator[]( const sint index ) const
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE void quick_sort( compFunc cmpFunc, sint leftarg, sint rightarg )
	{	//	base code from IBM with some changes : http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Ffunction_templates.htm&topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Ffunction_templates.htm
		if ( leftarg < rightarg )
		{
			sx_assert( leftarg >= 0 );
			sx_assert( leftarg < m_count );

			sx_assert( rightarg >= 0 );
			sx_assert( rightarg < m_count );

			int pivotvalue = ( leftarg + rightarg ) / 2;
			int left = leftarg - 1;
			int right = rightarg + 1;

			for(;;) {
				sx_assert( pivotvalue >= 0 );
				sx_assert( pivotvalue < m_count );

				sx_assert( right-1 >= 0 );
				sx_assert( right-1 < m_count );
				while ( --right >= 0 && cmpFunc( m_item[right], m_item[pivotvalue] ) > 0 );

				sx_assert( left+1 >= 0 );
				sx_assert( left+1 < m_count );
				while ( ++left < m_count && cmpFunc( m_item[left], m_item[pivotvalue] ) < 0 );

				if ( left >= right ) break;

				sx_assert( left >= 0 );
				sx_assert( left < m_count );
				sx_assert( right >= 0 );
				sx_assert( right < m_count );
				T temp = m_item[right];
				m_item[right] = m_item[left];
				m_item[left] = temp;

				if ( pivotvalue == right )		pivotvalue = left;
				else if ( pivotvalue == left )	pivotvalue = right;
			}

			quick_sort( cmpFunc, leftarg, right );
			quick_sort( cmpFunc, right + 1, rightarg );
		}
	}

private:

	SEGAN_LIB_INLINE void _realloc( const sint newsize )
	{
		if ( m_sampler )
		{
			if ( newsize > m_size || ( m_size - newsize ) > m_sampler ) {
				m_size = sint( newsize / m_sampler  + 1 ) * m_sampler;
				m_item = (T*)mem_realloc( m_item, m_size * sizeof(T) );
			}
		}
		else
		{
			m_size = newsize;
			m_item = (T*)mem_realloc( m_item, m_size * sizeof(T) );
		}
		
	}

public:

	T*			m_item;			//  items in array
	sint		m_count;		//  number of items in array
	sint		m_size;			//	size of the whole array
	sint		m_sampler;		//  use to sample memory to reduce allocation
};



/*! 
this array class doesn't use the copy constructor of class objects when it pushes them.
so it's suitable for pointers which has no copy constructor to decrease function calls and increase speed
NOTE : Compare function should return -1:less  0:equal  +1:more
*/
template<typename T, uint count>
class Array_fix
{
public:
	typedef sint (*compFunc)( const T& item1, const T& item2 );

	Array_fix(): m_count(0)
	{
		sx_assert( count > 0 );
#if _DEBUG
		memset( m_item, 0, sizeof(m_item) );
#endif
	}

	~Array_fix( void )
	{
	}

	SEGAN_LIB_INLINE void clear( void )
	{
		m_count = 0;
#if _DEBUG
		memset( m_item, 0, sizeof(m_item) );
#endif
	}

	SEGAN_LIB_INLINE void set_count( const sint newCount )
	{
		if ( newCount >= count ) return;
		m_count = newCount;
	}

	SEGAN_LIB_INLINE void push_back( const T& newItem )
	{
		if ( m_count >= count ) return;
		m_item[m_count++] = newItem;
	}

	SEGAN_LIB_INLINE void push_front( const T& newItem )
	{
		if ( m_count >= count ) return;
		for ( sint i = ++m_count; i>0; i-- )
			m_item[i] = m_item[i-1];
		m_item[0] = newItem;
	}

	SEGAN_LIB_INLINE void insert( const sint index, const T& newItem )
	{
		sx_assert(index>=0 && index<=m_count);
		if ( m_count >= count ) return;
		for ( sint i = ++m_count; i>index; i-- )
			m_item[i] = m_item[i-1];
		m_item[index] = newItem;
	}

	SEGAN_LIB_INLINE bool remove( const T& item )
	{
		sint i = index_of( item );
		if ( i < 0 ) return false;
		remove_index( i );
		return true;
	}

	SEGAN_LIB_INLINE void remove_index( const sint index, const bool keep_order = true )
	{
		sx_assert(index>=0 && index<m_count);
		m_count--;
		if (keep_order)
		{
			for (sint i = index; i < m_count - 1; ++i)
				m_item[i] = m_item[i + 1];
		}
		else m_item[index] = m_item[m_count - 1];
	}

	SEGAN_LIB_INLINE void swap( const sint index1, const sint index2 )
	{
		sx_assert(index1>=0 && index1<m_count && index2>=0 && index2<m_count);
		if ( index1 == index2 ) return;
		T _tmp = m_item[index1];
		m_item[index1] = m_item[index2];
		m_item[index2] = _tmp;
	}

	SEGAN_LIB_INLINE void sort( compFunc cmpFunc = null )
	{
		if ( m_count > 1 ) quick_sort( cmpFunc, 0, m_count-1 );
	}

	SEGAN_LIB_INLINE sint index_of( const T& item )
	{
		for ( sint i=0; i<m_count; i++ ) {
			if ( m_item[i] == item ) {
				return i;
			}
		}
		return -1;
	}

	SEGAN_LIB_INLINE T& at( const sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE T& operator[]( const sint index ) const
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

#if 1

	void quick_sort(compFunc cmpFunc, int leftarg, int rightarg)
	{	//	base code from IBM with some changes : http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Ffunction_templates.htm&topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Ffunction_templates.htm
		if ( leftarg < rightarg )
		{
			sx_assert( leftarg >= 0 );
			sx_assert( leftarg < m_count );

			sx_assert( rightarg >= 0 );
			sx_assert( rightarg < m_count );

			int pivotvalue = ( leftarg + rightarg ) / 2;
			int left = leftarg - 1;
			int right = rightarg + 1;

			for(;;) {
				sx_assert( pivotvalue >= 0 );
				sx_assert( pivotvalue < m_count );

				sx_assert( right-1 >= 0 );
				sx_assert( right-1 < m_count );
				while ( --right >= 0 && cmpFunc( m_item[right], m_item[pivotvalue] ) > 0 );

				sx_assert( left+1 >= 0 );
				sx_assert( left+1 < m_count );
				while ( ++left < m_count && cmpFunc( m_item[left], m_item[pivotvalue] ) < 0 );

				if ( left >= right ) break;

				sx_assert( left >= 0 );
				sx_assert( left < m_count );
				sx_assert( right >= 0 );
				sx_assert( right < m_count );
				T temp = m_item[right];
				m_item[right] = m_item[left];
				m_item[left] = temp;

				if ( pivotvalue == right )		pivotvalue = left;
				else if ( pivotvalue == left )	pivotvalue = right;
			}

			quick_sort( cmpFunc, leftarg, right );
			quick_sort( cmpFunc, right + 1, rightarg );
		}
	}
#endif

public:

	T		m_item[count];	//  items in array
	sint	m_count;		//  number of items in array
};



#endif	//	GUARD_Array_HEADER_FILE
