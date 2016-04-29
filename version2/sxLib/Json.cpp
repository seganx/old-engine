#include "Json.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
//	JSMN functions
//	Concept and code base from http://zserge.com/jsmn.html
//	Copyright(c) 2010 Serge A.Zaitsev
//////////////////////////////////////////////////////////////////////////
static int jsmn_name_comp(Json::Node* node, const char* js, const char* name)
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
static Json::Node *jsmn_alloc_token(Json *parser, Json::Node *tokens, size_t num_tokens)
{
	Json::Node *tok;
	if (parser->m_toknext >= num_tokens)
		return null;

	tok = &tokens[parser->m_toknext++];
	tok->start = tok->end = -1;
	tok->childs = 0;
	tok->parent = -1;

	return tok;
}

/*! Fills token type and boundaries.*/
static void jsmn_fill_token(Json::Node *token, Json::Type type, int start, int end)
{
	token->type = type;
	token->start = start;
	token->end = end;
	token->childs = 0;
}

/*! Fills next available token with JSON primitive. */
static int jsmn_parse_primitive(Json *parser, const char *js, uint len, Json::Node *tokens, uint num_tokens)
{
	int start = parser->m_pos;

	for (; parser->m_pos < len && js[parser->m_pos] != '\0'; parser->m_pos++)
	{
		switch (js[parser->m_pos])
		{
			case ':': case '\t': case '\r': case '\n':
			case ',': case ']': case '}': case ' ':
				goto found;
		}

		if (js[parser->m_pos] < 32 || js[parser->m_pos] >= 127)
		{
			parser->m_pos = start;
			return Json::ERROR_INVAL;
		}
	}

found:
	if (tokens == null)
	{
		parser->m_pos--;
		return 0;
	}

	Json::Node *token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == null)
	{
		parser->m_pos = start;
		return Json::ERROR_NOMEM;
	}

	jsmn_fill_token(token, Json::PRIMITIVE, start, parser->m_pos);
	token->parent = parser->m_toksuper;
	parser->m_pos--;
	return 0;
}

/*! Fills next token with JSON string. */
static int jsmn_parse_string(Json *parser, const char *js, uint len, Json::Node *tokens, uint num_tokens)
{
	int start = parser->m_pos;
	parser->m_pos++;

	/* Skip starting quote */
	for (; parser->m_pos < len && js[parser->m_pos] != '\0'; parser->m_pos++)
	{
		char c = js[parser->m_pos];

		/* Quote: end of string */
		if (c == '\"')
		{
			if (tokens == null)
				return 0;

			Json::Node *token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == null)
			{
				parser->m_pos = start;
				return Json::ERROR_NOMEM;
			}

			jsmn_fill_token(token, Json::STRING, start + 1, parser->m_pos);
			token->parent = parser->m_toksuper;
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->m_pos + 1 < len)
		{
			parser->m_pos++;
			switch (js[parser->m_pos])
			{
				/* Allowed escaped symbols */
				case '\"': case '/': case '\\': case 'b':
				case 'f': case 'r': case 'n': case 't':
					break;

					/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->m_pos++;
					for (int i = 0; i < 4 && parser->m_pos < len && js[parser->m_pos] != '\0'; i++)
					{
						/* If it isn't a hex character we have an error */
						if (!((js[parser->m_pos] >= 48 && js[parser->m_pos] <= 57) || /* 0-9 */
							(js[parser->m_pos] >= 65 && js[parser->m_pos] <= 70) || /* A-F */
							(js[parser->m_pos] >= 97 && js[parser->m_pos] <= 102))) /* a-f */
						{
							parser->m_pos = start;
							return Json::ERROR_INVAL;
						}
						parser->m_pos++;
					}
					parser->m_pos--;
					break;

					/* Unexpected symbol */
				default:
					parser->m_pos = start;
					return Json::ERROR_INVAL;
			}
		}
	}

	parser->m_pos = start;
	return Json::ERROR_PART;
}

/*! Parse JSON string and fill tokens. */
int jsmn_parse(Json *parser, const char *js, uint len, Json::Node *tokens, uint num_tokens)
{
	int count = parser->m_toknext;

	for (; parser->m_pos < len && js[parser->m_pos] != '\0'; parser->m_pos++)
	{
		char c = js[parser->m_pos];
		switch (c)
		{
			case '{': case '[':
			{
				count++;
				if (tokens == null)
					break;

				Json::Node *token = jsmn_alloc_token(parser, tokens, num_tokens);
				if (token == null)
					return Json::ERROR_NOMEM;
				if (parser->m_toksuper != -1)
				{
					tokens[parser->m_toksuper].childs++;
					token->parent = parser->m_toksuper;
				}
				token->type = (c == '{' ? Json::OBJECT : Json::ARRAY);
				token->start = parser->m_pos;
				parser->m_toksuper = parser->m_toknext - 1;
			}
			break;

			case '}': case ']':
			{
				if (tokens == null)
					break;

				Json::Type type = (c == '}' ? Json::OBJECT : Json::ARRAY);
				if (parser->m_toknext < 1)
					return Json::ERROR_INVAL;

				Json::Node *token = &tokens[parser->m_toknext - 1];
				for (;;)
				{
					if (token->start != -1 && token->end == -1)
					{
						if (token->type != type)
							return Json::ERROR_INVAL;

						token->end = parser->m_pos + 1;
						parser->m_toksuper = token->parent;
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
				if (parser->m_toksuper != -1 && tokens != null)
					tokens[parser->m_toksuper].childs++;
			}
			break;

			case '\t': case '\r': case '\n': case ' ': break;
			case ':': parser->m_toksuper = parser->m_toknext - 1; break;
			case ',':
				if (tokens != null && parser->m_toksuper != -1 && tokens[parser->m_toksuper].type != Json::ARRAY && tokens[parser->m_toksuper].type != Json::OBJECT)
					parser->m_toksuper = tokens[parser->m_toksuper].parent;
				break;

			default:
			{
				int r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				if (parser->m_toksuper != -1 && tokens != null)
					tokens[parser->m_toksuper].childs++;
			}
			break;
		}
	}

	if (tokens != null)
	{
		for (int i = parser->m_toknext - 1; i >= 0; i--)
		{
			/* Unmatched opened object or array */
			if (tokens[i].start != -1 && tokens[i].end == -1)
				return Json::ERROR_PART;
		}
	}

	return count;
}

Json::Node* jsmn_find(Json::Node* root, const char* js, const char* name)
{
	if (!root->down) return null;

	if (jsmn_name_comp(root, js, name))
		return root;

	Json::Node* res = null;

	if (root->down)
		res = jsmn_find(root->down, js, name);

	if (!res && root->right)
		res = jsmn_find(root->right, js, name);

	return res;
}

void jsmn_print(const char* js, Json::Node* root, int level, bool right, bool down)
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
Json::Json(): m_string(null), m_pos(0), m_toknext(0), m_toksuper(-1), m_nodes(null)
{
	memset(&m_tmp, 0, sizeof(m_tmp));
}

Json::~Json()
{
	if (m_nodes)
		free(m_nodes);
}

Json::Node* Json::parse(const char* jsondata)
{
	if (!jsondata) return 0;
	
	//	reset parser parameters
	m_pos = m_toknext = 0;
	m_toksuper = -1;
	int jsonlen = strlen(jsondata);

	//	get number of nodes needed
	int cnodes = jsmn_parse(this, jsondata, jsonlen, null, 0);
	if (cnodes < 1) return 0;

	//	create needed nodes
	if (m_nodes) free(m_nodes);
	m_nodes = (Node*)calloc(cnodes, sizeof(Node));

	//	reset parser parameters
	m_pos = m_toknext = 0;
	m_toksuper = -1;
	m_string = jsondata;

	//	parse the JSON string
	jsmn_parse(this, jsondata, jsonlen, m_nodes, cnodes);

	//	build nodes in hierarchy shape
	for (int i = 0; i < cnodes; ++i)
	{
		Node* node = &m_nodes[i];
		if (node->parent >= 0)
		{
			Node* parent = &m_nodes[node->parent];
			if (parent->down)
			{
				Node* down = parent->down;
				while (down->right) down = down->right;
				down->right = node;
			}
			else parent->down = node;
		}
	}

	return &m_nodes[0];
}

Json::Node* Json::find(const char* name)
{
	Node* res = jsmn_find(m_nodes, m_string, name);
	return res ? res : &m_tmp;
}

int Json::read_string(char* dest, const int dest_size, Node* node)
{
	return sprintf_s(dest, dest_size, "%.*s", (node->down->end - node->down->start), (m_string + node->down->start));
}

int Json::read_int(Node* node, const int& default_value)
{
	return 0;
}

float Json::read_float(Node* node, const float& default_value)
{
	return 0;
}

bool Json::read_bool(Node* node, const bool& default_value)
{
	return false;
}

void Json::print(void)
{
	jsmn_print(m_string, m_nodes, 0, true, true);
}
