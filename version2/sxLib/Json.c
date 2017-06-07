#include "Json.h"

//////////////////////////////////////////////////////////////////////////
//	JSMN functions
//	Concept and code base from http://zserge.com/jsmn.html
//	Copyright(c) 2010 Serge A.Zaitsev
//////////////////////////////////////////////////////////////////////////
static int jsmn_name_comp(sx_json_node* node, const char* js, const char* name)
{
	const char* d = &js[node->start];
	const char* e = &js[node->end - 1];
	const char* c = name;

	int r = (int)(*d - *c);
	while (!r && *c++ && d++ != e)
		r = (int)(*d - *c);

	return r == 0 ? 1 : 0;
}

/*! Allocates a fresh unused token from the token pull. */
static sx_json_node *jsmn_alloc_token(sx_json *parser, sx_json_node *tokens, size_t num_tokens)
{
	sx_json_node *tok;
	if (parser->toknext >= num_tokens)
		return null;

	tok = &tokens[parser->toknext++];
	tok->start = tok->end = -1;
	tok->childs = 0;
	tok->parent = -1;

	return tok;
}

/*! Fills token type and boundaries.*/
static void jsmn_fill_token(sx_json_node *token, json_type type, int start, int end)
{
	token->type = type;
	token->start = start;
	token->end = end;
	token->childs = 0;
}

/*! Fills next available token with JSON primitive. */
static int jsmn_parse_primitive(sx_json *parser, const char *js, uint len, sx_json_node *tokens, uint num_tokens)
{
	int start = parser->pos;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
	{
		switch (js[parser->pos])
		{
			case ':': case '\t': case '\r': case '\n':
			case ',': case ']': case '}': case ' ':
				goto found;
		}

		if (js[parser->pos] < 32 || js[parser->pos] >= 127)
		{
			parser->pos = start;
			return ERROR_INVAL;
		}
	}

found:
	if (tokens == null)
	{
		parser->pos--;
		return 0;
	}

	sx_json_node *token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == null)
	{
		parser->pos = start;
		return ERROR_NOMEM;
	}

	jsmn_fill_token(token, PRIMITIVE, start, parser->pos);
	token->parent = parser->toksuper;
	parser->pos--;
	return 0;
}

/*! Fills next token with JSON string. */
static int jsmn_parse_string(sx_json *parser, const char *js, uint len, sx_json_node *tokens, uint num_tokens)
{
	int start = parser->pos;
	parser->pos++;

	/* Skip starting quote */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
	{
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"')
		{
			if (tokens == null)
				return 0;

			sx_json_node *token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == null)
			{
				parser->pos = start;
				return ERROR_NOMEM;
			}

			jsmn_fill_token(token, STRING, start + 1, parser->pos);
			token->parent = parser->toksuper;
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len)
		{
			parser->pos++;
			switch (js[parser->pos])
			{
				/* Allowed escaped symbols */
				case '\"': case '/': case '\\': case 'b':
				case 'f': case 'r': case 'n': case 't':
					break;

					/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;
					for (int i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++)
					{
						/* If it isn't a hex character we have an error */
						if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
							(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
							(js[parser->pos] >= 97 && js[parser->pos] <= 102))) /* a-f */
						{
							parser->pos = start;
							return ERROR_INVAL;
						}
						parser->pos++;
					}
					parser->pos--;
					break;

					/* Unexpected symbol */
				default:
					parser->pos = start;
					return ERROR_INVAL;
			}
		}
	}

	parser->pos = start;
	return ERROR_PART;
}

/*! Parse JSON string and fill tokens. */
static int jsmn_parse(sx_json *parser, const char *js, uint len, sx_json_node *tokens, uint num_tokens)
{
	int count = parser->toknext;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
	{
		char c = js[parser->pos];
		switch (c)
		{
			case '{': case '[':
			{
				count++;
				if (tokens == null)
					break;

				sx_json_node *token = jsmn_alloc_token(parser, tokens, num_tokens);
				if (token == null)
					return ERROR_NOMEM;
				if (parser->toksuper != (uint)-1)
				{
					tokens[parser->toksuper].childs++;
					token->parent = parser->toksuper;
				}
				token->type = (c == '{' ? OBJECT : ARRAY);
				token->start = parser->pos;
				parser->toksuper = parser->toknext - 1;
			}
			break;

			case '}': case ']':
			{
				if (tokens == null)
					break;

				json_type type = (c == '}' ? OBJECT : ARRAY);
				if (parser->toknext < 1)
					return ERROR_INVAL;

				sx_json_node *token = &tokens[parser->toknext - 1];
				for (;;)
				{
					if (token->start != -1 && token->end == -1)
					{
						if (token->type != type)
							return ERROR_INVAL;

						token->end = parser->pos + 1;
						parser->toksuper = token->parent;
						break;
					}
					if (token->parent == -1)
						break;

					token = &tokens[token->parent];
				}
			}
			break;

			case '\"':
			{
				int r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				if (parser->toksuper != (uint)-1 && tokens != null)
					tokens[parser->toksuper].childs++;
			}
			break;

			case '\t': case '\r': case '\n': case ' ': break;
			case ':': parser->toksuper = parser->toknext - 1; break;
			case ',':
				if (tokens != null && parser->toksuper != (uint)-1 && tokens[parser->toksuper].type != ARRAY && tokens[parser->toksuper].type != OBJECT)
					parser->toksuper = tokens[parser->toksuper].parent;
				break;

			default:
			{
				int r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				if (parser->toksuper != (uint)-1 && tokens != null)
					tokens[parser->toksuper].childs++;
			}
			break;
		}
	}

	if (tokens != null)
	{
		for (int i = parser->toknext - 1; i >= 0; i--)
		{
			/* Unmatched opened object or array */
			if (tokens[i].start != -1 && tokens[i].end == -1)
				return ERROR_PART;
		}
	}

	return count;
}

static sx_json_node* jsmn_find(sx_json_node* root, const char* js, const char* name)
{
	if (!root->down) return null;

	if (jsmn_name_comp(root, js, name))
		return root;

	sx_json_node* res = null;

	if (root->down)
		res = jsmn_find(root->down, js, name);

	if (!res && root->right)
		res = jsmn_find(root->right, js, name);

	return res;
}

static void jsmn_print(const char* js, sx_json_node* root, int level, bool right, bool down)
{
	if (!root) return;

	for (int i = 0; i < level; ++i)
		printf("\t");

	printf("%.*s\n", (root->end - root->start), (js + root->start));

	if (down && root->down)
		jsmn_print(js, root->down, level + 1, right, down);

	if (right && root->right)
		jsmn_print(js, root->right, level, right, down);
}

//////////////////////////////////////////////////////////////////////////
//	JSON class implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API uint sx_json_node_count(sx_json* obj, const char* jsondata, const int jsonlen)
{
    if (!jsondata) return 0;

    //	reset parser parameters
    obj->pos = obj->toknext = 0;
    obj->toksuper = -1;

    //	get number of nodes needed
    return jsmn_parse(obj, jsondata, jsonlen, null, 0);
}

SEGAN_LIB_API sx_json_node* sx_json_parse(sx_json* obj, const char* jsondata, const int jsonlen)
{
	if (!jsondata) return 0;
	
	//	reset parser parameters
	obj->pos = obj->toknext = 0;
	obj->toksuper = -1;
	obj->text = jsondata;

	//	parse the JSON string
	jsmn_parse(obj, jsondata, jsonlen, obj->nodes, obj->nodescount);

	//	build nodes in hierarchy shape
	for (int i = 0; i < obj->nodescount; ++i)
	{
		sx_json_node* node = &obj->nodes[i];
        node->text = obj->text;
		if (node->parent >= 0)
		{
			sx_json_node* parent = &obj->nodes[node->parent];
			if (parent->down)
			{
				sx_json_node* down = parent->down;
				while (down->right) down = down->right;
				down->right = node;
			}
			else parent->down = node;
		}
	}

	return obj->nodes;
}

SEGAN_LIB_API sx_json_node* sx_json_find(sx_json* obj, const char* name)
{
	sx_json_node* res = jsmn_find(obj->nodes, obj->text, name);
	return res ? res : &obj->tmp;
}

SEGAN_LIB_API int sx_json_read_value(sx_json_node* node, char* dest, const int dest_size)
{
	return _snprintf_s(dest, dest_size, _TRUNCATE, "%.*s", (node->down->end - node->down->start), (node->text + node->down->start));
}

SEGAN_LIB_API int sx_json_read_string(sx_json_node* node, const char* name, char* dest, const int dest_size)
{
    sx_json_node* found = jsmn_find(node, node->text, name);
    return found ? sx_json_read_value(found, dest, dest_size) : 0;
}

SEGAN_LIB_API int sx_json_read_int(sx_json_node* node, const char* name, const int default_value)
{
    char tmp[16] = init;
    sx_json_read_string(node, name, tmp, 16);
   
    int res = default_value;
    sscanf_s(tmp, "%d", &res, sizeof(res));
    return res;
}

SEGAN_LIB_API float sx_json_read_float(sx_json_node* node, const char* name, const float default_value)
{
	return 0;
}

SEGAN_LIB_API bool sx_json_read_bool(sx_json_node* node, const char* name, const bool default_value)
{
	return false;
}

SEGAN_LIB_API void sx_json_print(sx_json_node* node)
{
	jsmn_print(node->text, node, 0, true, true);
}
