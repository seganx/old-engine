/********************************************************************
	created:	2015/05/04
	filename: 	Partition.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple partition to store and retrieve
	data by position
	*********************************************************************/
#ifndef GUARD_Partition_HEADER_FILE
#define GUARD_Partition_HEADER_FILE


#include "Def.h"


template<typename T>
class Partition
{
	sx_sterile_class(Partition);
public:

	//! use this callback function to iterate through rows. return true to continue and false to stop iteration
	typedef bool(*CB_Partition)(void* usedata, float* pos, T& data);

	//private:

	//!	node of the partition
	struct Box;
	struct Node
	{
		T		data;
		float	pos[3];
		Box*	box;
		word	index;

		void init(const float x, const float y, const float z)
		{
			pos[0] = x;
			pos[1] = y;
			pos[2] = z;
			box = null;
			index = 0;
		}
	};

	//! node container for a partition
	struct Box
	{
		Node**	nodes;
		word	count;

#if _DEBUG
		word	pos[3];
#endif
		Box() : nodes(0), count(0) {}

		SEGAN_LIB_INLINE void add(Node* node)
		{
			node->box = this;
			node->index = count++;
			nodes = (Node**)sx_mem_realloc(nodes, count * sizeof(Node*));
			nodes[node->index] = node;
		}

		SEGAN_LIB_INLINE void remove(const Node* node)
		{
			word index = node->index;
			sx_assert(index < count);
			sx_assert(nodes[index] == node);
			nodes[index] = nodes[--count];
			nodes[index]->index = index;
			if (count)
				nodes = (Node**)sx_mem_realloc(nodes, count * sizeof(Node*));
			else
				sx_mem_free_and_null(nodes);
		}

		void clear(const bool deleteNodes)
		{
			while (deleteNodes && count)
				sx_mem_free(nodes[--count]);
			sx_mem_free_and_null(nodes);
			count = 0;
		}
	};

public:

	Partition() : m_count(0), m_boxes(0), m_scale(0), m_width(0), m_height(0), m_length(0) {}
	~Partition() { clear(); }

	void set_size(const uint boxSize, const uint width, const uint height, const uint length)
	{
		m_scale = boxSize;
		m_width = width;
		m_height = height;
		m_length = length;
		m_boxCount = m_width * m_height * m_length;
		const uint msize = m_boxCount * sizeof(Box);
		m_boxes = (Box*)sx_mem_realloc(m_boxes, msize);
		sx_mem_set(m_boxes, 0, msize);

#if _DEBUG
		for (uint i = 0; i < width; i++)
		{
			for (uint j = 0; j < height; j++)
			{
				for (uint k = 0; k < length; k++)
				{
					uint w = i * m_scale;
					uint h = j * m_scale;
					uint l = k * m_scale;
					uint index = get_box_index_by_pos(w + 0.1f, h + 0.1f, l + 0.1f);
					m_boxes[index].pos[0] = w;
					m_boxes[index].pos[1] = h;
					m_boxes[index].pos[2] = l;
				}
			}
		}
#endif
	}

	void clear()
	{
		clear_boxes(true);
		sx_mem_free_and_null(m_boxes);
		m_count = 0;
	}

	//	insert data to partition and return reference to the new partition object 
	void* insert(const T& data, const float x, const float y, const float z)
	{
		//	create new node for the new data
		Node* node = (Node*)sx_mem_alloc(sizeof(Node));
		node->init(x, y, z);
		node->data = data;

		//	add the new node the box
		const uint i = get_box_index_by_pos(x, y, z);
		Box* box = get_box_by_index(i);
		box->add(node);

		++m_count;
		return node;
	}

	//	remove the object from partition
	void remove(void*obj)
	{
		Node* node = (Node*)obj;
		Box* box = node->box;
		if (box)
		{
			box->remove(node);
			sx_mem_free(node);
			--m_count;
		}
	}

	//	update the node position
	void update_node(void* obj, const float x, const float y, const float z)
	{
		if (!verify_position(x, y, z)) return;

		Node* node = (Node*)obj;
		sx_assert(node);

		Box* box = node->box;

		//	update the position
		node->pos[0] = x;
		node->pos[1] = y;
		node->pos[2] = z;

		//	verify that the node moved and changed the box
		Box* newBox = get_box_by_pos(x, y, z);
		if (box != newBox)
		{
			//	change the box of the node
			box->remove(node);
			newBox->add(node);
		}
	}

	//	return number of objects in the axis aligned box area
	uint get_count_in_aabox(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2)
	{
		const float xmin = sx_min_f(x1, x2);
		const float ymin = sx_min_f(y1, y2);
		const float zmin = sx_min_f(z1, z2);

		const float xmax = sx_max_f(x1, x2);
		const float ymax = sx_max_f(y1, y2);
		const float zmax = sx_max_f(z1, z2);

		const int wl = (xmin < 0) ? 0 : int(xmin / m_scale);
		const int hl = (ymin < 0) ? 0 : int(ymin / m_scale);
		const int ll = (zmin < 0) ? 0 : int(zmin / m_scale);

		const int wh = (xmax < 0) ? 0 : int(xmax / m_scale);
		const int hh = (ymax < 0) ? 0 : int(ymax / m_scale);
		const int lh = (zmax < 0) ? 0 : int(zmax / m_scale);

		uint res = 0;
		for (int i = wl; i <= wh; ++i)
		{
			for (int j = hl; j <= hh; ++j)
			{
				for (int k = ll; k <= lh; ++k)
				{
#if 1
					uint index = get_box_index(i, j, k);
					Box* box = get_box_by_index(index);
					if (box != &m_tmpBox)
					{
						for (uint n = 0; n < box->count; n++)
						{
							Node* node = box->nodes[n];
							bool xa = sx_between_f(node->pos[0], xmin, xmax);
							bool ya = sx_between_f(node->pos[1], ymin, ymax);
							bool za = sx_between_f(node->pos[2], zmin, zmax);
							if (xa && ya && za)
								++res;
						}
					}
#else
					res++;
#endif
				}
			}
		}
		return res;
	}

	//	return number of objects in the spherical area
	uint get_count_in_sphere(const float x, const float y, const float z, const float radius)
	{
		return 0;
	}

	SEGAN_LIB_INLINE Box* get_box_by_index(uint index)
	{
		if (index < m_boxCount)
			return &m_boxes[index];
		else
			return &m_tmpBox;
	}

	SEGAN_LIB_INLINE uint get_box_index(const uint w, const uint h, const uint l)
	{
		uint p = l * m_width * m_height;
		uint r = h * m_width;
		return (w + r + p);
	}

	SEGAN_LIB_INLINE uint get_box_index_by_pos(const float x, const float y, const float z)
	{
		const uint w = uint(x / m_scale);
		const uint h = uint(y / m_scale);
		const uint l = uint(z / m_scale);
		return get_box_index(w, h, l);
	}

	SEGAN_LIB_INLINE Box* get_box_by_pos(const float x, const float y, const float z)
	{
		uint index = get_box_index_by_pos(x, y, z);
		return get_box_by_index(index);
	}

	SEGAN_LIB_INLINE void clear_boxes(const bool deleteNodes)
	{
		for (uint i = 0; i < m_boxCount; ++i)
		{
			m_boxes[i].clear(deleteNodes);
		}
	}

	SEGAN_LIB_INLINE bool verify_position(const float x, const float y, const float z)
	{
		uint w = m_scale * m_width;
		uint h = m_scale * m_height;
		uint l = m_scale * m_length;
		bool vx = (x >= 0) && (x < w);
		bool vy = (y >= 0) && (y < h);
		bool vz = (z >= 0) && (z < l);
		return (vx && vy && vz);
	}

public:
	uint	m_count;		//	number of data in container
	Box*	m_boxes;		//	array of boxes
	uint	m_scale;		//	scale of each box
	uint	m_width;		//	number of boxes in row
	uint	m_height;		//	number of boxes in columns
	uint	m_length;		//	number of boxes in length
	uint	m_boxCount;		//	number of all boxes
	Box		m_tmpBox;
};

#endif // GUARD_Partition_HEADER_FILE
