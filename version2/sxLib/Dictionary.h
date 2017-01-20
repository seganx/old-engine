/********************************************************************
	created:	2013/11/05
	filename: 	Dictionary.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple dictionary to store and 
				retrieve data by name.
*********************************************************************/
#ifndef GUARD_Dictionary_HEADER_FILE
#define GUARD_Dictionary_HEADER_FILE

#include "Def.h"


#define	TABLE_CASE_INSENSITIVE	0		//!	make the table case insensitive in names

template<typename T_data>
class Dictionary
{
	SEGAN_STERILE_CLASS( Dictionary );
public:

	//! use this callback function to iterate through rows. return true to continue and false to stop iteration
	typedef bool (*CB_Table)(void* usedata, const wchar* name, T_data& data);

	//! since we don't know the size of data type, so maybe it's better to allocate the memory block separately
	struct Row
	{
		T_data	data;
	};

	//!	leaf of the string tree
	struct Leaf
	{
		wchar	label;
		Leaf*	right;
		Leaf*	down;
		Row*	row;

		void init( const wchar _label )
		{
#if TABLE_CASE_INSENSITIVE
			label	= sx_str_lower( _label );
#else
			label	= _label;
#endif
			down	= null;
			right	= null;
			row		= null;
		}

		bool can_remove( void )
		{
			return ( !right && !row );
		}
	};

	Dictionary( void ): m_root(0), m_count(0) {}
	~Dictionary( void ) { clear(); }

	void clear( void )
	{
		_free( m_root );
		m_root = 0;
		m_count = 0;
	}

	bool insert( const wchar* name, const T_data& data )
	{
		sx_assert( name );
		if ( !name ) return false;
		if ( !m_root )
		{
			m_root = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			m_root->init( name[0] );
		}
		return _put( m_root, 0, name, data );
	}

	bool insert_sort( const wchar* name, const T_data& data )
	{
		sx_assert( name );
		if ( !name ) return false;
		if ( !m_root )
		{
			m_root = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			m_root->init( name[0] );
		}
		return _put_sort( null, m_root, 0, name, data );
	}

	bool remove( const wchar* name )
	{
		sx_assert( name );
		if ( !name || !m_root ) return false;
		return _putaway( m_root, 0, name );
	}

	SEGAN_LIB_INLINE bool find( const wchar* name, T_data& result )
	{
		sx_assert( name );
		if ( !name || !m_root ) return false;
		return _pickup( m_root, 0, name, result );
	}

	SEGAN_LIB_INLINE T_data get( const wchar* name, const T_data& failed )
	{
		sx_assert( name );
		if ( !name || !m_root ) return failed;
		T_data res;
		if ( _pickup( m_root, 0, name, res ) )
			return res;
		else
			return failed;
	}

	void iterate( void* userdata, CB_Table callback )
	{
		if ( !m_root ) return;
		wchar name[1024];
		_traverse( m_root, userdata, callback, name, 0 );
	}

#if _DEBUG
	void print( Leaf* leaf, int index )
	{
		if ( !leaf ) return;

		if ( leaf->row )
			printf( "%c", toupper(leaf->label) );
		else
			printf( "%c", tolower(leaf->label) );

		if ( leaf->right )
			print( leaf->right, index + 1 );

		if ( leaf->down )
		{
			printf( "\n" );
			for ( int i=0; i<index; ++i )
				printf( " " );
			print( leaf->down, index );
		}
	}
#endif

private:

	void _free( Leaf* leaf )
	{
		if ( !leaf ) return;
		if ( leaf->row )
			sx_mem_free( leaf->row );
		_free( leaf->right );
		_free( leaf->down );
		sx_mem_free( leaf );
	}

	SEGAN_LIB_INLINE bool _put( Leaf* leaf, uint index, const wchar* name, const T_data& data )
	{
#if TABLE_CASE_INSENSITIVE
		wchar label = sx_str_lower( name[index] );
#else
		wchar label = name[index];
#endif
		//	search through other leafs
		while ( leaf->label != label && leaf->down )
		{
			leaf = leaf->down;
		}

		//	no leaf has been found, so just create new one
		if ( leaf->label != label )
		{
			leaf->down = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			leaf->down->init( label );
			leaf = leaf->down;
		}

		//	just put the data or go to the next index
		label = name[index+1];
		if ( label == 0 )
		{
			//	we are in position. verify the row and put the data
			if ( !leaf->row )
			{
				leaf->row = (Row*)sx_mem_alloc( sizeof(Row) );
				leaf->row->data = data;
				++m_count;
				return true;
			}
			else return false;
		}
		else
		{
			//	just go deeper leaf
			if ( !leaf->right )
			{
				leaf->right =  (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				leaf->right->init( label );
			}
			return _put( leaf->right, index+1, name, data );
		}
	}

	SEGAN_LIB_INLINE bool _put_sort( Leaf* left, Leaf* leaf, uint index, const wchar* name, const T_data& data )
	{
#if TABLE_CASE_INSENSITIVE
		wchar label = tolower( name[index] );
#else
		wchar label = name[index];
#endif
		//	search through other leafs
		Leaf* parent = null;
		while ( label > leaf->label && leaf->down )
		{
			parent = leaf;
			leaf = leaf->down;
		}

		//	no leaf has been found, so just create new one
		if ( leaf->label != label )
		{
			Leaf* newone = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			newone->init( label );
			
			if ( label < leaf->label )
			{
				if ( left && left->right == leaf )
				{
					left->right = newone;
					newone->down = leaf;
				}
				else
				{
					if ( parent )
					{
						parent->down = newone;
						newone->down = leaf;
					}
					else
					{
						m_root = newone;
						newone->down = leaf;
					}
				}
			}
			else leaf->down = newone;

			leaf = newone;
		}

		//	just put the data or go to the next index
		label = name[index+1];
		if ( label == 0 )
		{
			//	we are in position. verify the row and put the data
			if ( !leaf->row )
			{
				leaf->row = (Row*)sx_mem_alloc( sizeof(Row) );
				leaf->row->data = data;
				++m_count;
				return true;
			}
			else return false;
		}
		else
		{
			//	just go deeper leaf
			if ( !leaf->right )
			{
				leaf->right =  (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				leaf->right->init( label );
			}
			return _put_sort( leaf, leaf->right, index+1, name, data );
		}
	}

	SEGAN_LIB_INLINE bool _pickup( Leaf* leaf, uint index, const wchar* name, T_data& data )
	{
#if TABLE_CASE_INSENSITIVE
		wchar label = sx_str_lower( name[index] );
#else
		wchar label = name[index];
#endif
		//	search through other leafs
		while ( leaf && leaf->label != label )
		{
			leaf = leaf->down;
		}
		if ( !leaf ) return false;

		//	just look for data or go to the next index
		label = name[index+1];
		if ( label == 0 )
		{
			//	we are in position. verify the row and pick the data up
			if ( leaf->row )
			{
				data = leaf->row->data;
				return true;
			}
			else return false;
		}
		else
		{
			//	just go deeper leaf
			if ( leaf->right )
				return _pickup( leaf->right, index+1, name, data );
			else
				return false;
		}
	}

	SEGAN_LIB_INLINE bool _putaway( Leaf* leaf, uint index, const wchar* name )
	{
		if ( !leaf ) return false;

#if TABLE_CASE_INSENSITIVE
		wchar label = tolower( name[index] );
#else
		wchar label = name[index];
#endif
		//	search through other leafs
		if ( leaf->label != label )
		{
			bool res = _putaway( leaf->down, index, name );
			if ( res && leaf->down->can_remove() )
			{
				Leaf* tmp = leaf->down->down;
				sx_mem_free( leaf->down );
				leaf->down = tmp;
			}
			return res;
		}

		//	just look for data or go to the next index
		label = name[index+1];
		if ( label == 0 )
		{
			//	we are in position. verify the row and remove the row
			if ( leaf->row )
			{
				sx_mem_free( leaf->row );
				leaf->row = null;
				--m_count;
				return true;
			}
			else return false;
		}
		else
		{
			//	just go deeper leaf
			if ( leaf->right )
			{
				bool res = _putaway( leaf->right, index+1, name );
				if ( res && leaf->right->can_remove() )
				{
					Leaf* tmp = leaf->right->down;
					sx_mem_free( leaf->right );
					leaf->right = tmp;
				}
				return res;
			}
			else
				return false;
		}
	}

	SEGAN_LIB_INLINE bool _traverse( const Leaf* leaf, void* userdata, CB_Table callback, wchar* name, uint index )
	{
		if ( !leaf ) return true;

		sx_assert( index < 1023 );
		name[index] = leaf->label;

		bool res = true;
		if ( leaf->row )
		{
			name[index+1] = 0;
			res = callback( userdata, name, leaf->row->data );
		}

		if ( res && leaf->right )
			res = _traverse( leaf->right, userdata, callback, name, index+1 );

		if ( res && leaf->down )
			res = _traverse( leaf->down, userdata, callback, name, index );

		return res;
	}

public:
	Leaf*	m_root;
	uint	m_count;
};

#endif // GUARD_Dictionary_HEADER_FILE
