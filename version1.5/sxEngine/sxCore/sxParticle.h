/********************************************************************
	created:	2011/06/15
	filename: 	sxParticle.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic class of particle. the main
				reason to make this class is creating and rendering smokes
				in the scene.
*********************************************************************/
#ifndef GUARD_sxParticle_HEADER_FILE
#define GUARD_sxParticle_HEADER_FILE

#include "sxNodeMember.h"
#include "sxMaterialMan.h"
#include "../sxRender/sxRender.h"

namespace sx { namespace core {

	//! the basic particle class of the engine. in this time I need a simple smoke in the scene. 
	class SEGAN_API Particle : public NodeMember
	{
		SEGAN_STERILE_CLASS(Particle);

	public:
		Particle();
		virtual ~Particle();

		//! clear current content
		void Clear(void);

		//! this will call when the owner's "Validate" message received
		void Validate(int level);

		//! this will call when the owner's "Invalidate" message received
		void Invalidate(void);

		//!	this will call when the owner's "Update" function called
		void Update(float elpsTime);

		//! draw this particle depend on entry flag
		void Draw(DWORD flag);

		//! draw with LOD depend on view parameter. ignore draw for so far objects
		void DrawByViewParam(DWORD flag, float viewParam);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! set particle description
		void SetDesc(ParticleDesc *pDesc);

		//! return pointer of particle description
		const ParticleDesc *GetDesc(void);

		//! clear current buffer and restart spraying particle
		void Reset(void);

		//! return reference to material manager
		MaterialMan& Material(void);

		/*! return level of detail value depend on view parameter. 	*/
		int ComputeLodValue(float viewParam);

	private:

		//! setup a simple quad
		void SetupQuad(ParticleQuad& pQuad, const float fAge);

		//! update a simple quad
		void UpdateQuad(ParticleQuad* pQuad, const float etime, const float* force, const float* ownerSpeed);

		//! create suit buffer
		void CreateBuffer(void);

		//! send particle quads to buffer
		void BurnBuffers(void);

		AABox						m_box;				//  should update on Burn Buffer functions
		Sphere						m_sphere;			//  bounding sphere of particle
		ParticleDesc				m_desc;				//  description of this particle
		Array<ParticleQuad>	m_quads;			//  array of quads
		MaterialMan					m_material;			//  materials of this particle
		int							m_numQuads;			//  number of quads per play
		float						m_flowTime;			//	time counter to control flow
		float						m_flowTimeMax;		//	time counter to control flow
		float						m_delayTime;		//	delay time to spray particles
		int							m_updateBox;		//  use to update bonding box of parent
		float3						m_ownerPos;			//  position of owner used to compute speed
		DWORD						reserved;			//  use to some internal options and operation
		
	public:
		//////////////////////////////////////////////////////////////////////////
		//	MESH MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:
			//! use begin/end patch to patch particle in the scene
			static void BeginPatch(DWORD flag);

			//! use begin/end patch to patch particle in the scene
			static void EndPatch(DWORD flag);

			//! coefficient of minimum volume of particle in screen space which can display
			static float& MinimumVolume(void);
		};

	};
	typedef Particle *PParticle;


}}	//  namespace sx { namespace core {

#endif	//	GUARD_sxParticle_HEADER_FILE
