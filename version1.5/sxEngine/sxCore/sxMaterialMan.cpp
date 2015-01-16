#include "sxMaterialMan.h"
#include "../sxRender/sxRender.h"

/*
NOTE:	in this class I use a member named 'reserved' to do some operation.
		first byte of reserved used to store validation level contain 0,1,2 ...
		second byte of reserved used to store validation count. validation count must be less that 3

		the other bytes will describe later
*/
#define VALIDATION_LEVEL	SEGAN_1TH_BYTEOF(reserved)
#define VALIDATION_COUNT	SEGAN_2TH_BYTEOF(reserved)
#define VALIDATION_NONE		0xff

static const UINT materialFileID = MAKEFOURCC('M', 'T', 'R', 'L');

namespace sx { namespace core {


	MaterialMan::MaterialMan():	m_ActiveMaterial(0), reserved(0)
	{
		m_materials.PushBack( sx_new( d3d::Material ) );

		VALIDATION_LEVEL = VALIDATION_NONE;
		VALIDATION_COUNT = 0;
	}

	MaterialMan::~MaterialMan()
	{
		Invalidate();

		for (int i=0; i<m_materials.Count(); i++)
		{
			sx_delete( m_materials[i] );
		}
		m_materials.Clear();
	}

	void MaterialMan::Clear( void )
	{
		for (int i=0; i<m_materials.Count(); i++)
		{
			sx_delete( m_materials[i] );
		}
		m_materials.Clear();

		m_materials.PushBack( sx_new( d3d::Material ) );
	}

	void MaterialMan::Validate( int level )
	{
		if ( level < VALIDATION_LEVEL )
		{
			VALIDATION_LEVEL = level;
			VALIDATION_COUNT += 1;

			for (int i=0; i<m_materials.Count(); i++)
			{
				m_materials[i]->Validate(level);
			}
		}
	}

	void MaterialMan::Invalidate( void )
	{
		if ( VALIDATION_LEVEL != VALIDATION_NONE )
		{
			VALIDATION_LEVEL = VALIDATION_NONE;
			while (VALIDATION_COUNT)
			{
				for (int i=0; i<m_materials.Count(); i++)
				{
					//sxLog::Log(L"start material [ %d ] invalidation", i);
					m_materials[i]->Invalidate();
					//sxLog::Log(L"end material [ %d ] invalidation", i);
				}

				VALIDATION_COUNT -= 1;
			}
		}
	}


	void MaterialMan::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, materialFileID);

		// write version
		int version = 1;
		SEGAN_STREAM_WRITE(stream, version);

		// write number of materials and their contents
		int n = m_materials.Count();
		SEGAN_STREAM_WRITE(stream, n);
		for (int i=0; i<m_materials.Count(); i++)
			m_materials[i]->Save(stream);

		//  finally write the active material index
		SEGAN_STREAM_WRITE(stream, m_ActiveMaterial);
	}

	void MaterialMan::Load( Stream& stream )
	{
		Clear();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != materialFileID)
		{
			sxLog::Log(L"Incompatible file format for loading material !");
			return;
		}

		// load version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			// read number of materials and their contents
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			m_materials[0]->Load(stream);
			for (int i=1; i<n; i++)
				Add()->Load(stream);
			
			//  finally read the active material index
			SEGAN_STREAM_READ(stream, m_ActiveMaterial);
		}
	}

	d3d::PMaterial MaterialMan::Add( void )
	{
		d3d::PMaterial newMat = sx_new( d3d::Material );
		m_materials.PushBack(newMat);

		if ( VALIDATION_LEVEL != VALIDATION_NONE )
		{
			for (int i=0; i<VALIDATION_COUNT; i++)
			{
				newMat->Validate();
			}
		}

		return newMat;
	}

	void MaterialMan::Remove( int index )
	{
		if (index>0 || index<m_materials.Count()) 
		{
			for (int i=0; i<VALIDATION_COUNT; i++)
			{
				m_materials[index]->Invalidate();
			}

			sx_delete_and_null( m_materials[index] );
			m_materials.RemoveByIndex(index);
		}
	}

	d3d::PMaterial MaterialMan::Get( int index )
	{
		if (index<0 || index>=m_materials.Count())
			return NULL;
		else
			return m_materials[index];
	}

	FORCEINLINE int MaterialMan::Count( void )
	{
		return m_materials.Count();
	}

	FORCEINLINE void MaterialMan::SetActiveMaterial( int index )
	{
		if (index<0 || index>=m_materials.Count())
			m_ActiveMaterial = 0;
		else
			m_ActiveMaterial = index;
	}

	FORCEINLINE int MaterialMan::GetActiveMaterialIndex( void )
	{
		return m_ActiveMaterial;
	}

	FORCEINLINE d3d::PMaterial MaterialMan::GetActiveMaterial( void )
	{
		if ( m_ActiveMaterial > -1 )
			return m_materials[m_ActiveMaterial];
		else return NULL;
	}

	FORCEINLINE void MaterialMan::SetToDevice( DWORD flag )
	{
		m_materials[m_ActiveMaterial]->SetToDevice(flag);
	}

	FORCEINLINE d3d::PMaterial MaterialMan::operator[]( int index )
	{
		if (index<0 || index>=m_materials.Count())
			return NULL;
		else
			return m_materials[index];
	}


}}  //  namespace sx { namespace core {