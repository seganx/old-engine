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

#include "Memory.h"
#include "Assert.h"

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

	SEGAN_LIB_INLINE void Clear( void )
	{
		_Realloc( 0 );
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

	SEGAN_LIB_INLINE void PushBack( const T& newItem )
	{
		_Realloc( ++m_count );
		m_item[m_count-1] = newItem;
	}

	SEGAN_LIB_INLINE void PushFront( const T& newItem )
	{
		_Realloc( ++m_count );
		for ( sint i = m_count-1; i>0; i-- )
			m_item[i] = m_item[i-1];
		m_item[0] = newItem;
	}

	SEGAN_LIB_INLINE void Insert( sint index, const T& newItem )
	{
		sx_assert(index>=0 && index<=m_count);
		_Realloc( ++m_count );
		for ( sint i = m_count-1; i>index; i-- )
			m_item[i] = m_item[i-1];
		m_item[index] = newItem;
	}

	SEGAN_LIB_INLINE bool Remove( const T& item )
	{
		sint i = IndexOf( item );
		if ( i < 0 ) return false;
		RemoveByIndex( i );
		return true;
	}

	SEGAN_LIB_INLINE void RemoveByIndex( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		for ( sint i=index; i<m_count-1; i++ )
			m_item[i] = m_item[i+1];
		_Realloc( --m_count );
	}

	SEGAN_LIB_INLINE void Swap( sint index1, sint index2 )
	{
		sx_assert(index1>=0 && index1<m_count && index2>=0 && index2<m_count);
		if ( index1 == index2 ) return;
		T _tmp = m_item[index1];
		m_item[index1] = m_item[index2];
		m_item[index2] = _tmp;
	}

	SEGAN_LIB_INLINE void Sort( compFunc cmpFunc = null )
	{
		if ( m_count > 1 ) QuickSort( cmpFunc, 0, m_count-1 );
	}

	SEGAN_LIB_INLINE sint IndexOf( const T& item )
	{
		for ( sint i=0; i<m_count; i++ ) {
			if ( m_item[i] == item ) {
				return i;
			}
		}
		return -1;
	}

	SEGAN_LIB_INLINE T& At( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE T& operator[]( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE void QuickSort( compFunc cmpFunc, sint leftarg, sint rightarg )
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

			QuickSort( cmpFunc, leftarg, right );
			QuickSort( cmpFunc, right + 1, rightarg );
		}
	}
#endif

private:

	SEGAN_LIB_INLINE void _Realloc( sint newSize )
	{
		if ( m_sampler )
		{
			if ( newSize > m_size || ( m_size - newSize ) > m_sampler ) {
				m_size = sint( newSize / m_sampler  + 1 ) * m_sampler;
				mem_realloc( (void*&)m_item, m_size * sizeof(T) );
			}
		}
		else
		{
			m_size = newSize;
			mem_realloc( (void*&)m_item, m_size * sizeof(T) );
		}
		
	}

private:

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

	SEGAN_LIB_INLINE sint Count( void )
	{
		return m_count;
	}

	SEGAN_LIB_INLINE bool IsEmpty( void ) const
	{
		return ( m_count == 0 );
	}

	SEGAN_LIB_INLINE void PushBack( const T& newItem )
	{
		if ( m_count >= count ) return;
		m_item[m_count++] = newItem;
	}

	SEGAN_LIB_INLINE void PushFront( const T& newItem )
	{
		if ( m_count >= count ) return;
		for ( sint i = ++m_count; i>0; i-- )
			m_item[i] = m_item[i-1];
		m_item[0] = newItem;
	}

	SEGAN_LIB_INLINE void Insert( sint index, const T& newItem )
	{
		sx_assert(index>=0 && index<=m_count);
		if ( m_count >= count ) return;
		for ( sint i = ++m_count; i>index; i-- )
			m_item[i] = m_item[i-1];
		m_item[index] = newItem;
	}

	SEGAN_LIB_INLINE bool Remove( const T& item )
	{
		sint i = IndexOf( item );
		if ( i < 0 ) return false;
		RemoveByIndex( i );
		return true;
	}

	SEGAN_LIB_INLINE void RemoveByIndex( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		m_count--;
		for ( sint i=index; i<m_count; i++ )
			m_item[i] = m_item[i+1];
	}

	SEGAN_LIB_INLINE void Swap( sint index1, sint index2 )
	{
		sx_assert(index1>=0 && index1<m_count && index2>=0 && index2<m_count);
		if ( index1 == index2 ) return;
		T _tmp = m_item[index1];
		m_item[index1] = m_item[index2];
		m_item[index2] = _tmp;
	}

	SEGAN_LIB_INLINE void Sort( compFunc cmpFunc = null )
	{
		if ( m_count > 1 ) QuickSort( cmpFunc, 0, m_count-1 );
	}

	SEGAN_LIB_INLINE sint IndexOf( const T& item )
	{
		for ( sint i=0; i<m_count; i++ ) {
			if ( m_item[i] == item ) {
				return i;
			}
		}
		return -1;
	}

	SEGAN_LIB_INLINE T& At( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

	SEGAN_LIB_INLINE T& operator[]( sint index )
	{
		sx_assert(index>=0 && index<m_count);
		return m_item[index];
	}

#if 1

	void QuickSort(compFunc cmpFunc, int leftarg, int rightarg)
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

			QuickSort( cmpFunc, leftarg, right );
			QuickSort( cmpFunc, right + 1, rightarg );
		}
	}

private:

	T		m_item[count];	//  items in array
	int		m_count;		//  number of items in array
};



#endif	//	GUARD_Array_HEADER_FILE
