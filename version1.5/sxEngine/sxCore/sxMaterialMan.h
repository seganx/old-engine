/********************************************************************
	created:	2011/06/16
	filename: 	sxMaterialMan.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple material manager that will
				use in some renderable members like meshes, particles ...
*********************************************************************/
#ifndef GUARD_sxMaterialMan_HEADER_FILE
#define GUARD_sxMaterialMan_HEADER_FILE

#include "../sxRender/sxMaterial.h"

namespace sx { namespace core {

	//! this class manage materials for renderable objects
	class SEGAN_API MaterialMan
	{
		SEGAN_STERILE_CLASS(MaterialMan);

	public:
		MaterialMan();
		virtual ~MaterialMan();

		//! clear current content
		void Clear(void);

		//! return the number of materials
		int Count(void);

		//! this will call when the owner's "Validate" message received
		void Validate(int level);

		//! this will call when the owner's "Invalidate" message received
		void Invalidate(void);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! add new material
		d3d::PMaterial Add(void);

		//! remove a material
		void Remove(int index);

		//! get a material by index. return null if no material found
		d3d::PMaterial Get(int index);

		//! active the render material by index. active material 0 if index be in invalid range
		void SetActiveMaterial(int index);

		//! return the index of material which is currently activated
		int	GetActiveMaterialIndex(void);

		//! return pointer to the activated material
		d3d::PMaterial GetActiveMaterial(void);

		//! set activated material to the device
		void SetToDevice(DWORD flag);

		// simple operator
		d3d::PMaterial operator[] (int index);

	private:
		Array<d3d::PMaterial>	m_materials;			//	array of materials
		int							m_ActiveMaterial;	//  index of rendering material

		DWORD			reserved;		//  use to some internal options and operation
	};
	typedef MaterialMan *PMaterialMan;

}}	//  namespace sx { namespace core {

#endif	//	GUARD_sxMaterialMan_HEADER_FILE
