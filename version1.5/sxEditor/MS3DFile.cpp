#pragma warning(disable : 4786)
#include "MS3DFile.h"
#include "ImportEngine.h"

#define MAKEDWORD(a, b)      ((unsigned int)(((word)(a)) | ((word)((word)(b))) << 16))

using namespace sx;

class CMS3DFileI
{
public:
	Array<ms3d_vertex_t>		arrVertices;
	Array<ms3d_triangle_t>		arrTriangles;
	Array<ms3d_group_t>			arrGroups;
	Array<ms3d_material_t>		arrMaterials;
	Array<ms3d_joint_t>			arrJoints;

	float	fAnimationFPS;
	float	fCurrentTime;
	int		iTotalFrames;
public:
	CMS3DFileI()
	:	fAnimationFPS(24.0f),
		fCurrentTime(0.0f),
		iTotalFrames(0)
	{
	}
};

CMS3DFile::CMS3DFile()
{
	_i = sx_new( CMS3DFileI() );
}

CMS3DFile::~CMS3DFile()
{
	Clear();
	sx_delete( _i );
}

bool CMS3DFile::LoadFromFile( const WCHAR* FileName )
{
	sys::FileStream file;
	if ( !file.Open(FileName, FM_OPEN_READ | FM_SHARE_READ) ) return false;

	ms3d_header_t header;
	file.Read(&header, sizeof(ms3d_header_t));

	if (strncmp(header.id, "MS3D000000", 10) != 0)
		return false;

	if (header.version != 4)
		return false;

	// vertices
	word nNumVertices;
	file.Read(&nNumVertices, sizeof(word));
	if (nNumVertices)
	{
		_i->arrVertices.SetCount(nNumVertices);
		file.Read(&_i->arrVertices[0], nNumVertices * sizeof(ms3d_vertex_t));
	}

	// triangles
	word nNumTriangles;
	file.Read(&nNumTriangles, sizeof(word));
	if (nNumTriangles)
	{
		_i->arrTriangles.SetCount(nNumTriangles);
		file.Read(&_i->arrTriangles[0], nNumTriangles * sizeof(ms3d_triangle_t));
	}

	// groups
	word nNumGroups;
	file.Read(&nNumGroups, sizeof(word));
	if (nNumGroups)
	{
		_i->arrGroups.SetCount(nNumGroups);
		for (int i=0; i < nNumGroups; i++)
		{
			file.Read(&_i->arrGroups[i].flags, sizeof(byte));
			file.Read(&_i->arrGroups[i].name, 32 * sizeof(char));
			file.Read(&_i->arrGroups[i].numtriangles, sizeof(word));

			_i->arrGroups[i].triangleIndices = (word*)sx_mem_alloc( sizeof(word) * _i->arrGroups[i].numtriangles );
			file.Read(_i->arrGroups[i].triangleIndices, _i->arrGroups[i].numtriangles * sizeof(word));
			file.Read(&_i->arrGroups[i].materialIndex, sizeof(char) );
		}
	}

	// materials
	word nNumMaterials;
	file.Read(&nNumMaterials, sizeof(word));
	if (nNumMaterials)
	{
		_i->arrMaterials.SetCount(nNumMaterials);
		file.Read(&_i->arrMaterials[0], nNumMaterials * sizeof(ms3d_material_t));
	}

	file.Read(&_i->fAnimationFPS, sizeof(float));
	file.Read(&_i->fCurrentTime, sizeof(float));
	file.Read(&_i->iTotalFrames, sizeof(int));

	// joints
	word nNumJoints;
	file.Read(&nNumJoints, sizeof(word));
	if (nNumJoints)
	{
		_i->arrJoints.SetCount(nNumJoints);
		for (int i=0; i < nNumJoints; i++)
		{
			file.Read(&_i->arrJoints[i].flags, sizeof(byte));
			file.Read(&_i->arrJoints[i].name, 32 * sizeof(char));
			file.Read(&_i->arrJoints[i].parentName, 32 * sizeof(char));
			file.Read(&_i->arrJoints[i].rotation, 3 * sizeof(float));
			file.Read(&_i->arrJoints[i].position, 3 * sizeof(float));
			file.Read(&_i->arrJoints[i].numKeyFramesRot, sizeof(word));
			file.Read(&_i->arrJoints[i].numKeyFramesTrans, sizeof(word));

			_i->arrJoints[i].keyFramesRot = (ms3d_keyframe_rot_t*)sx_mem_alloc( _i->arrJoints[i].numKeyFramesRot * sizeof(ms3d_keyframe_rot_t) );
			_i->arrJoints[i].keyFramesTrans = (ms3d_keyframe_pos_t*)sx_mem_alloc( _i->arrJoints[i].numKeyFramesTrans * sizeof(ms3d_keyframe_pos_t) );
			file.Read(_i->arrJoints[i].keyFramesRot, _i->arrJoints[i].numKeyFramesRot * sizeof(ms3d_keyframe_rot_t));
			file.Read(_i->arrJoints[i].keyFramesTrans, _i->arrJoints[i].numKeyFramesTrans * sizeof(ms3d_keyframe_pos_t));
		}
	}

	file.Close();

	return true;
}


void CMS3DFile::Clear()
{
	for (int i=0; i<_i->arrGroups.Count(); i++)
		sx_mem_free(_i->arrGroups[i].triangleIndices);

	for (int i=0; i<_i->arrJoints.Count(); i++)
	{
		sx_mem_free(_i->arrJoints[i].keyFramesRot);
		sx_mem_free(_i->arrJoints[i].keyFramesTrans);
	}


	_i->arrVertices.Clear();
	_i->arrTriangles.Clear();
	_i->arrGroups.Clear();
	_i->arrMaterials.Clear();
	_i->arrJoints.Clear();
}

int CMS3DFile::GetNumVertices()
{
	return (int) _i->arrVertices.Count();
}

void CMS3DFile::GetVertexAt(int nIndex, ms3d_vertex_t **ppVertex)
{
	if (nIndex >= 0 && nIndex < (int) _i->arrVertices.Count())
		*ppVertex = &_i->arrVertices[nIndex];
}

int CMS3DFile::GetNumTriangles()
{
	return (int) _i->arrTriangles.Count();
}

void CMS3DFile::GetTriangleAt(int nIndex, ms3d_triangle_t **ppTriangle)
{
	if (nIndex >= 0 && nIndex < (int) _i->arrTriangles.Count())
		*ppTriangle = &_i->arrTriangles[nIndex];
}

int CMS3DFile::GetNumGroups()
{
	return (int) _i->arrGroups.Count();
}

void CMS3DFile::GetGroupAt(int nIndex, ms3d_group_t **ppGroup)
{
	if (nIndex >= 0 && nIndex < (int) _i->arrGroups.Count())
		*ppGroup = &_i->arrGroups[nIndex];
}

int CMS3DFile::GetNumMaterials()
{
	return (int) _i->arrMaterials.Count();
}

void CMS3DFile::GetMaterialAt(int nIndex, ms3d_material_t **ppMaterial)
{
	if (nIndex >= 0 && nIndex < (int) _i->arrMaterials.Count())
		*ppMaterial = &_i->arrMaterials[nIndex];
}

int CMS3DFile::GetNumJoints()
{
	return (int) _i->arrJoints.Count();
}

void CMS3DFile::GetJointAt(int nIndex, ms3d_joint_t **ppJoint)
{
	if (nIndex >= 0 && nIndex < (int) _i->arrJoints.Count())
		*ppJoint = &_i->arrJoints[nIndex];
}

int CMS3DFile::FindJointByName(const char* lpszName)
{
	for (int i=0; i < _i->arrJoints.Count(); i++)
	{
		if (!strcmp(_i->arrJoints[i].name, lpszName))
			return i;
	}

	return -1;
}

float CMS3DFile::GetAnimationFPS()
{
	return _i->fAnimationFPS;
}

float CMS3DFile::GetCurrTime()
{
	return _i->fCurrentTime;
}

int CMS3DFile::GetTotalFrames()
{
	return _i->iTotalFrames;
}
