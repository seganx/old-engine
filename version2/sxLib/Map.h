/********************************************************************
	created:	2012/04/04
	filename: 	Map.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain map class. in this version I decide to use
				AVL-Tree because of fastest search between B-Trees, although the insertion 
				and delete operation is slower than Red-Black-Tree but AVL is more balanced and
				have faster search.

				The original AVL-Tree c++ implementation from 
				http://meshula.net/articles/AVL.html

*********************************************************************/
#ifndef GUARD_Map_HEADER_FILE
#define GUARD_Map_HEADER_FILE

#include "Def.h"


/*! 
this map use AVL tree structure to store data
NOTE : Compare function MUST return -1:less  0:equal  +1:more
*/
template<typename T_key, typename T_data>
class Map
{
	SEGAN_STERILE_CLASS(Map);

	static sint map_cmpfnc_default( const T_key& v1, const T_key& v2 )
	{
		return ( v1 > v2 ? 1 : ( v1 < v2 ? -1 : 0) );
	}

public:
	typedef sint (*compFunc)( const T_key& key1, const T_key& key2 );

	struct Leaf
	{
		T_key		key;
		T_data		data;
		Leaf*		left;
		Leaf*		right;
		sint		balance;
		sint		depth;

		void init( const T_key& _key, const T_data& _data )
		{
			key		= _key;
			data	= _data;
			left	= null;
			right	= null;
			balance	= 0;
			depth	= 0;
		}
	};

	struct Iterator
	{
		Leaf*		m_stack[32];
		Leaf*		m_first;
		Leaf*		m_root;
		Leaf*		m_curr;
		sint		m_index;

		Iterator( Leaf* pmn ): m_root(pmn), m_first(pmn), m_curr(pmn), m_index(0)
		{
			if ( !m_root ) return;
			while ( m_curr ) {
				m_first = m_curr;
				if ( m_curr->left )
					m_stack[m_index++] = m_curr;
				m_curr = m_curr->left;
			}
			m_curr = m_first;
		}
		~Iterator( void ) {}

		SEGAN_LIB_INLINE bool is_first( void )		{ return m_curr == m_first; }
		SEGAN_LIB_INLINE bool is_last( void )		{ return m_curr == null; }
		SEGAN_LIB_INLINE T_key& operator&( void )	{ return m_curr->key;  }
		SEGAN_LIB_INLINE T_data& operator*( void )	{ return m_curr->data; }
		SEGAN_LIB_INLINE void operator++(sint)		{
			if ( !m_curr ) return;
			Leaf* pCurr = m_curr->right;
			while ( true ) {
				if ( pCurr ) {
					m_stack[m_index++] = pCurr;
					pCurr = pCurr->left;
				} else {
					if ( m_index > 0 ) {
						Leaf* pCandidate = m_stack[--m_index];
						if ( m_curr == pCandidate->right ) {
							if ( m_index > 0 ) {
								m_curr = m_stack[--m_index];
							}
						} else m_curr = pCandidate;
					} else m_curr = null;
					return;
				}
			}
		}
	};

public:

	Map( compFunc _compFunc = &map_cmpfnc_default )
		: m_root(null)
		, m_count(0)
		, m_compFunc(_compFunc)
	{
	}

	~Map( void )
	{ 
		clear();
	}

	SEGAN_LIB_INLINE void clear( void )
	{
		_clear( m_root );
		m_count = 0;
	}

	SEGAN_LIB_INLINE Iterator first( void ) const 
	{
		return m_root;		
	}

	SEGAN_LIB_INLINE bool insert( const T_key& key, const T_data& item, bool overwrite = false )
	{
		if ( m_root == 0 ) {
			m_root = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			m_root->init( key, item );
			m_count++;
			return true;
		}
		if ( _insert( key, item, m_root, overwrite ) ) {
			m_count++;
			return true;
		}
		return false;
	}

	SEGAN_LIB_INLINE bool insert_multi( const T_key& key, const T_data& item )
	{
		if ( m_root == 0 ) {
			m_root = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
			m_root->init( key, item );
			m_count++;
			return true;
		}
		if ( _insert_multi( key, item, m_root ) ) {
			m_count++;
			return true;
		}
		return false;
	}

	SEGAN_LIB_INLINE bool remove( const T_key& key )
	{
		bool delOK = false;
		_remove( m_root, key, delOK );
		if ( delOK ) m_count--;
		return delOK;
	}

	SEGAN_LIB_INLINE bool find( const T_key& key, T_data& item )
	{
		return _find( key, item, m_root );
	}

private:

	SEGAN_LIB_INLINE void _clear( Leaf*& leaf )
	{
		if ( !leaf ) return;
		_clear( leaf->left );
		_clear( leaf->right );
		sx_mem_free( leaf );
		leaf = null;
	}

	SEGAN_LIB_INLINE bool _insert( const T_key& key, const T_data& item, Leaf*& root, bool overwrite )
	{
		switch ( m_compFunc( key, root->key ) ) {
		case +1 :
			if ( root->right )
			{
				if ( !_insert( key, item, root->right, overwrite ) ) return false; 
			}
			else
			{
				root->right = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				root->right->init( key, item );
			}
			break;
		case -1 :
			if ( root->left )
			{
				if ( !_insert( key, item, root->left, overwrite ) ) return false;
			}
			else
			{
				root->left = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				root->left->init( key, item );
			}
			break;
		default :
			{
				if ( overwrite )
				{
					root->data = item;
					return true;
				}
			}
			return false;
		}
		_compute_balance( root );
		_balance( root );
		return true;
	}

	SEGAN_LIB_INLINE bool _insert_multi( const T_key& key, const T_data& item, Leaf*& root )
	{
		switch ( m_compFunc( key, root->key ) ) {
		case 0 :
		case +1 :
			if ( root->right )
			{
				_insert_multi( key, item, root->right );
			}
			else
			{
				root->right = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				root->right->init( key, item );
			}
			break;
		case -1 :
			if ( root->left )
			{
				_insert_multi( key, item, root->left );
			}
			else
			{
				root->left = (Leaf*)sx_mem_alloc( sizeof(Leaf) );
				root->left->init( key, item );
			}
			break;
		default :
			sx_assert("map:compareFunction must return -1 or 0 or 1 ! "<0);
			return false;
		}
		_compute_balance( root );
		_balance( root );
		return true;
	}

	SEGAN_LIB_INLINE void _remove ( Leaf*& root, const T_key& key, bool& delOK )
	{
		if ( !root ) {
			delOK = false;
			return;
		}
		switch ( m_compFunc( root->key, key ) )
		{
		case +1 :		// go to left subtree
			_remove( root->left, key, delOK );
			if ( delOK ) {
				_compute_balance( root );
				_balance_right( root );
			}
			return;
		case -1 :		// go to right subtree
			_remove( root->right, key, delOK );
			if ( delOK ) {
				_compute_balance( root );
				_balance_left( root );
			}
			return;
		default:		// leaf found!
			Leaf* leaf = root;
			if ( !root->right ) {
				root = root->left;
				delOK = true;
				sx_mem_free( leaf );
			} else if ( !root->left ) {
				root = root->right;
				delOK = true;
				sx_mem_free( leaf );
			} else {
				_remove_both_children( root, root->left, delOK );
				if ( delOK ) {
					_compute_balance( root );
					_balance( root );
				}
				delOK = true;
			}
		}
	}

	SEGAN_LIB_INLINE void _remove_both_children( Leaf*& root, Leaf*& curr, bool& delOK )
	{
		if ( !curr->right )
		{
			root->key = curr->key;
			root->data = curr->data;
			Leaf* leaf = curr;
			curr = curr->left;
			sx_mem_free( leaf );
			delOK = true;
		} else {
			_remove_both_children( root, curr->right, delOK );
			if ( delOK ) {
				_compute_balance( root );
				_balance( root );
			}
		}
	}

	SEGAN_LIB_INLINE bool _find( const T_key& key, T_data& item, Leaf* root )
	{
		if ( !root ) return false;
		switch ( m_compFunc( key, root->key ) )
		{
		case +1 : return _find( key, item, root->right );
		case -1 : return _find( key, item, root->left );
		default : item = root->data; return true;
		}	
	}

	SEGAN_LIB_INLINE void _compute_balance( Leaf*  root )
	{
		if ( !root ) return;
		sint leftDepth  = root->left  ? root->left->depth  : 0;
		sint rightDepth = root->right ? root->right->depth : 0;
		root->depth = 1 + ( (leftDepth > rightDepth) ? leftDepth : rightDepth );
		root->balance = rightDepth - leftDepth;
	}

	SEGAN_LIB_INLINE void _balance( Leaf*& root )
	{
		// AVL trees have the property that no branch is more than 1 longer than its sibling
		if ( !root ) return;
		if ( root->balance > 1 )	_balance_right( root );
		if ( root->balance < -1 )	_balance_left( root );
	}

	SEGAN_LIB_INLINE void _balance_right( Leaf*& root )
	{
		if ( root && root->right ) {
			if ( root->right->balance > 0 )
				_rotate_left( root );
			else if ( root->right->balance < 0 ) {
				_rotate_right( root->right );
				_rotate_left( root );
			}
		}
	}

	SEGAN_LIB_INLINE void _balance_left( Leaf*& root )
	{
		if ( root && root->left ) {
			if ( root->left->balance < 0 )
				_rotate_right( root );
			else if ( root->left->balance > 0 ) {
				_rotate_left( root->left );
				_rotate_right( root );
			}
		}
	}

	SEGAN_LIB_INLINE void _rotate_left( Leaf*& root )
	{
		if ( root->right ) {
			Leaf* tmp = root;
			root = root->right;
			tmp->right = root->left;
			root->left = tmp;
			_compute_balance( root->left );
			_compute_balance( root->right );
			_compute_balance( root );
		}
	}

	SEGAN_LIB_INLINE void _rotate_right( Leaf*& root )
	{
		if ( root->left ) {
			Leaf* tmp = root;
			root = root->left;
			tmp->left = root->right;
			root->right = tmp;
			_compute_balance( root->left );
			_compute_balance( root->right );
			_compute_balance( root );
		}
	}

public:

	Leaf*		m_root;		//  root of the map
	sint		m_count;	//  number of items in map
	compFunc	m_compFunc;	//	use to compare keys

};


#endif	//	GUARD_Map_HEADER_FILE
