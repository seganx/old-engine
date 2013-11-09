/********************************************************************
	created:	2013/11/9
	filename: 	Mesh.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of mesh for openGL
*********************************************************************/
#ifndef GUARD_Mesh_gl_HEADER_FILE
#define GUARD_Mesh_gl_HEADER_FILE


#include "Device.h"

//////////////////////////////////////////////////////////////////////////
//!	mesh
class Mesh : public d3dMesh
{
	SEGAN_STERILE_CLASS( Mesh );

public:
	Mesh( void );
	~Mesh( void );

	//! set new mesh description. this may clear current data
	void set_desc( d3dMesh& desc );

	//! lock array of positions of the mesh. return null if the function failed to lock
	float3* lock_positions( void );

	//! unlock array of positions
	void unlock_positions( void );

	//! lock array of normals of the mesh. return null if the function failed to lock
	float3* lock_normals( void );

	//! unlock array of normals
	void unlock_normals( void );

	//! lock array of tangents of the mesh. return null if the function failed to lock
	float3* lock_tangents( void );

	//! unlock array of tangents
	void unlock_tangents( void );

	//! lock array of texture coordinates in the mesh. return null if the function failed to lock
	float2* lock_texcoords( const uint index );

	//! unlock array of texture coordinates
	void unlock_texcoords( const uint index );

	//! lock array of vertex colors in the mesh. return null if the function failed to lock
	d3dColor* lock_colors( void );

	//! unlock array of texture coordinates
	void unlock_colors( void );

public:

	VertexBuffer		m_pos;
	VertexBuffer		m_norm;
	VertexBuffer		m_tang;
	VertexBuffer		m_tex0;
	VertexBuffer		m_tex1;
	VertexBuffer		m_colr;
	IndexBuffer			m_indices;

};


//////////////////////////////////////////////////////////////////////////
//	mesh manager
class MeshMan : public d3dMeshMan
{
	SEGAN_STERILE_CLASS( MeshMan );
public:
	MeshMan( void );
	~MeshMan( void );

	/*! 
	add a mesh to the scene 
	NOTE: this function must add the new mesh to the list of meshes for additional access
	*/
	void add( const d3dMesh* mesh );

	/*!
	remove mesh from the scene
	NOTE: this function must remove the mesh from the list of meshes
	*/
	void remove( const d3dMesh* mesh );

	/*!
	get meshes from an specified area
	NOTE: this function should uses a tree structure for fast search
	*/
	uint get_by_area( Array<d3dMesh*>& result, const float3& pos, const float radius );

	/*!
	get meshes from an specified frustum
	NOTE: this function should uses a tree structure for fast search
	*/
	uint get_by_frustum( Array<d3dMesh*>& result, const Frustum& frustum );

public:


};



#endif	//	GUARD_Mesh_gl_HEADER_FILE