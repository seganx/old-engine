/********************************************************************
	created:	2016/4/23
	filename: 	Json.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a class for parse JSON scripts.
				
				Concept and code base from http://zserge.com/jsmn.html
				Copyright (c) 2010 Serge A. Zaitsev
*********************************************************************/
#ifndef DEFINED_Json
#define DEFINED_Json

#include "Def.h"

//! use this class to parse JSON
class SEGAN_LIB_API Json
{
public:
	enum Type
	{
		UNDEFINED = 0,
		OBJECT = 1,
		ARRAY = 2,
		STRING = 3,
		PRIMITIVE = 4		//! Other primitive: number, boolean (true/false) or null
	};

	enum Error
	{
		ERROR_NOMEM = -1,	//! Not enough tokens were provided
		ERROR_INVAL = -2,	//! Invalid character inside JSON string
		ERROR_PART = -3		//! The string is not a full JSON packet, more bytes expected
	};

	struct Node
	{
		Type	type;		//! (object, array, string etc.)
		int		start;		//! start position in JSON data string
		int		end;		//! end position in JSON data string
		int		childs;		//! number of children the node has
		int		parent;		//! index of the parent in node array
		Node*	right;		//! pointer to the right node
		Node*	down;		//! pointer to the down node (child)
	};

public:
	Json();
	~Json();

	//! parses a JSON data string and return the root node
	Node* parse(const char* jsondata);

	//! find and return a node by name. return zero node if can't find specified node
	Node* find(const char* name);

	//! fill out dest buffer with string value of the node
	int read_string(char* dest, const int dest_size, Node* node);

	//! return node's value as integer
	int read_int(Node* node, const int& default_value);

	//! return node's value as float
	float read_float(Node* node, const float& default_value);

	//! return node's value as boolean
	bool read_bool(Node* node, const bool& default_value);

	//! print the parsed JSON in a tree structure
	void print(void);

public:
	const char*	m_string;		//! JSON string
	uint		m_pos;			//! offset in the JSON string
	uint		m_toknext;		//! next token to allocate
	uint		m_toksuper;		//! superior token node, e.g parent object or array
	Node*		m_nodes;		//! array of node
	Node		m_tmp;			//! temp node
};


#endif // DEFINED_Json

