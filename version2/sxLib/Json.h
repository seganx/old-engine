/********************************************************************
    created:	2016/4/23
    filename: 	Json.h
    author:		Sajad Beigjani
    email:		sajad.b@gmail.com
    Site:		www.SeganX.com
    Desc:		This file contains structures and function for parse JSON scripts.

    Concept and code base from http://zserge.com/jsmn.html
    Copyright (c) 2010 Serge A. Zaitsev
    *********************************************************************/
#ifndef DEFINED_Json
#define DEFINED_Json

#include "Def.h"

typedef enum json_type
{
    UNDEFINED = 0,
    OBJECT = 1,
    ARRAY = 2,
    STRING = 3,
    PRIMITIVE = 4		//! Other primitive: number, boolean (true/false) or null
}
json_type;

typedef enum json_error
{
    ERROR_NOMEM = -1,	//! Not enough tokens were provided
    ERROR_INVAL = -2,	//! Invalid character inside JSON string
    ERROR_PART = -3		//! The string is not a full JSON packet, more bytes expected
}
json_error;

typedef struct sx_json_node
{
    const char*             text;       //! JSON string
    json_type	            type;		//! (object, array, string etc.)
    int		                start;		//! start position in JSON data string
    int		                end;		//! end position in JSON data string
    int		                childs;		//! number of children the node has
    int		                parent;		//! index of the parent in node array
    struct sx_json_node*	right;		//! pointer to the right node
    struct sx_json_node*	down;		//! pointer to the down node (child)
} 
sx_json_node;

//! use this object to hold JSON data
typedef struct sx_json
{
    const char*             text;           //! JSON string
    uint                    pos;            //! offset in the JSON string
    uint                    toknext;        //! next token to allocate
    uint                    toksuper;       //! superior token node, e.g parent object or array
    int                     nodescount;     //! number of nodes
    struct sx_json_node*    nodes;          //! array of node
    struct sx_json_node     tmp;            //! temp node used in return values
}
sx_json;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


//! compute number of nodes needed to parse data
SEGAN_LIB_API uint sx_json_node_count(sx_json* obj, const char* jsondata, const int jsonlen);

/*! 
parses a JSON data string and return a pointer to the root
NOTE: use sx_json_node_count to find out number of nodes needed
*/
SEGAN_LIB_API sx_json_node* sx_json_parse(sx_json* obj, const char* jsondata, const int jsonlen);

//! find and return a node by name. return zero node if can't find specified node
SEGAN_LIB_API sx_json_node* sx_json_find(sx_json* obj, const char* name);

//! fill out dest buffer with string value of the node and return the length of string
SEGAN_LIB_API int sx_json_read_value(sx_json_node* node, char* dest, const int dest_size);

//! fill out dest buffer with string value of the node and return the length of string
SEGAN_LIB_API int sx_json_read_string(sx_json_node* node, const char* name, char* dest, const int dest_size);

//! return node's value as integer
SEGAN_LIB_API int sx_json_read_int(sx_json_node* node, const char* name, const int default_value);

//! return node's value as float
SEGAN_LIB_API float sx_json_read_float(sx_json_node* node, const char* name, const float default_value);

//! return node's value as boolean
SEGAN_LIB_API bool sx_json_read_bool(sx_json_node* node, const char* name, const bool default_value);

//! print the parsed JSON in a tree structure
SEGAN_LIB_API void sx_json_print(sx_json_node* node);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEFINED_Json

