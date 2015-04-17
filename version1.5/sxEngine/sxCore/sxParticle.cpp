#include "sxParticle.h"
#include "sxNode.h"
#include "sxSettings.h"
#include "../sxCommon/sxCommon.h"

/*
NOTE:	in this class I use a member named 'reserved' to do some operation.
		first byte of reserved used to store validation level contain 0,1,2 ...

		second byte use to loading verification. this will use to load and place particles in first initialize steps
		and avoid to sparse particles in first steps

		the other bytes will describe later
*/
#define VALIDATION_LEVEL	SEGAN_1TH_BYTEOF(reserved)
#define VALIDATION_NONE		0xff
#define LOADED_LEVEL		SEGAN_2TH_BYTEOF(reserved)
#define QUAD_NUM_VRTX		6

static const UINT particleFileID = MAKEFOURCC('P', 'R', 'T', 'L');

//  some static variables
static float				s_minimumVolume = 0.075f;			//  minimum volumes to display particles
static float3				s_camDir(0,0,0);				//  use to sort particles
static sx::d3d::Geometry3D	s_geometry;						//  use geometry to display particle quads
static AABox				s_particleBox( float3(-0.05f, -0.05f, -0.05f), float3(0.05f, 0.05f, 0.05f) );
static Sphere				s_particleSphere( float3(0.0f, 0.0f, 0.0f), 0.1f );

namespace sx { namespace core {

	int CompareQuadDistance(const ParticleQuad& q1, const ParticleQuad& q2)
	{
		const float d1 = s_camDir.x*q1.position.x + s_camDir.y*q1.position.y + s_camDir.z*q1.position.z;
		const float d2 = s_camDir.x*q2.position.x + s_camDir.y*q2.position.y + s_camDir.z*q2.position.z;
		return (d1 < d2) ? 1 : (d1 > d2) ? -1 : 0;
	}

	Particle::Particle() 
		: NodeMember()
		, m_box(s_particleBox)
		, m_sphere(s_particleSphere)
		, m_flowTime(0)
		, m_numQuads(0)
		, m_delayTime(0)
		, m_updateBox(0)
		, m_ownerPos(0,0,0)
		, reserved(0)
	{
		m_Type = NMT_PARTICLE;

		VALIDATION_LEVEL = VALIDATION_NONE;
	}

	Particle::~Particle()
	{
		sx_callstack();

		Clear();
	}

	void Particle::Clear( void )
	{
		m_quads.Clear();
		m_material.Clear();
		s_geometry.Cleanup();

		m_flowTime = m_flowTimeMax;

		VALIDATION_LEVEL = VALIDATION_NONE;
	}

	void Particle::Validate( int level )
	{
		if ( level >= VALIDATION_LEVEL) return;
		VALIDATION_LEVEL = level;

		m_material.Validate(level);
	}

	void Particle::Invalidate( void )
	{
		if (VALIDATION_LEVEL == VALIDATION_NONE) return;

		m_material.Invalidate();
		
		VALIDATION_LEVEL = VALIDATION_NONE;
	}

	void Particle::Update( float elpsTime )
	{
		if ( elpsTime > 500.0f ) return;

		bool canSpray = SEGAN_SET_HAS( m_Option, SX_PARTICLE_SPRAY );

		if ( canSpray && m_desc.delayTime > 0 )
		{
			m_delayTime += elpsTime * 0.001f;
			canSpray = ( m_delayTime > m_desc.delayTime );
		}

		if ( !canSpray && !m_quads.Count() )
		{
			m_numQuads = 0;
			m_flowTime = m_flowTimeMax;
			m_updateBox++;

			if ( m_updateBox > 30 )
			{
				m_updateBox = sx::cmn::Random( 10 );

				m_ownerPos = m_Owner->GetPosition_world();
				m_box.Min.Set( m_ownerPos.x - 0.05f, m_ownerPos.y - 0.05f, m_ownerPos.z - 0.05f );
				m_box.Max.Set( m_ownerPos.x + 0.05f, m_ownerPos.y + 0.05f, m_ownerPos.z + 0.05f );
				m_sphere.Zero();
				m_sphere.ComputeByAABox( m_box );

				m_Owner->UpdateBoundingVolumes();
			}
			return;
		}

		const float etime = elpsTime * 0.001f;

		//////////////////////////////////////////////////////////////////////////
		//	remove dead quads
		if ( m_desc.lifeTime > 0 )
		{
			for (int i=0; i<m_quads.Count(); i++)
			{
				if ( m_quads[i].age >= m_desc.lifeTime )
				{
					m_quads.RemoveByIndex(i);
					i--;
					continue;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//  create quads
		const bool valideMaxFlowTime = m_flowTimeMax > 0;
		if ( canSpray && valideMaxFlowTime )
		{
			if ( m_flowTime >= m_flowTimeMax )
			{
				const int numCreateQuad = int( m_flowTime / m_flowTimeMax );
				const float quantime = 0.001f * ( m_flowTime - m_flowTimeMax ) / (float)numCreateQuad;
				float3 ownerSpeed = ( elpsTime > EPSILON ) ? 500.0f * ( m_Owner->GetPosition_world() - m_ownerPos ) / elpsTime : float3(0,0,0);
				for ( int i=0; i<numCreateQuad; i++ )
				{
					if ( m_numQuads >= m_desc.quadCount )
					{
						if ( m_Option & SX_PARTICLE_LOOP )
						{
							if ( m_desc.delayTime > 0 )
							{
								m_delayTime = ( m_Option & SX_PARTICLE_RNDTIME ) ? sx::cmn::Random( m_desc.delayTime ) : 0.0f;
								m_numQuads = 0;
								break;
							}
						}
						else
						{
							SEGAN_SET_REM( m_Option, SX_PARTICLE_SPRAY );
							m_delayTime = ( m_Option & SX_PARTICLE_RNDTIME ) ? sx::cmn::Random( m_desc.delayTime ) : 0.0f;
							m_numQuads = 0;
							break;
						}
					}

					ParticleQuad quad;
					SetupQuad( quad, 0.0f );

					const float qetime = i * quantime;

					float force[3];
					const float mtime = qetime * 0.06f;
					force[0] = ( m_desc.gravity.x + m_desc.wind.x ) * mtime;
					force[1] = ( m_desc.gravity.y + m_desc.wind.y ) * mtime;
					force[2] = ( m_desc.gravity.z + m_desc.wind.z ) * mtime;

					UpdateQuad( &quad, qetime, force, ownerSpeed );

					m_quads.PushBack( quad );

					m_numQuads++;
				}

				m_flowTime = 0;
			}

			m_flowTime += elpsTime;
		} 
		else m_flowTime = m_flowTimeMax;


		//////////////////////////////////////////////////////////////////////////
		//  gather information
		if ( LOADED_LEVEL )	LOADED_LEVEL -= 1;
		m_box.Min.Set( math::MAX, math::MAX, math::MAX );
		m_box.Max.Set( -math::MAX, -math::MAX, -math::MAX );

		float3 ownerSpeed = ( elpsTime > EPSILON ) ? ( m_Owner->GetPosition_world() - m_ownerPos ) / etime : float3(0,0,0);
		m_ownerPos = m_Owner->GetPosition_world();

		float force[3];
		const float mtime = etime * 0.06f;
		force[0] = ( m_desc.gravity.x + m_desc.wind.x ) * mtime;
		force[1] = ( m_desc.gravity.y + m_desc.wind.y ) * mtime;
		force[2] = ( m_desc.gravity.z + m_desc.wind.z ) * mtime;

		//////////////////////////////////////////////////////////////////////////
		//	update each quad
		for (int i=0; i<m_quads.Count(); i++)
		{
			ParticleQuad *pQuad = &m_quads[i];

			UpdateQuad( pQuad, etime, force, ownerSpeed );

			float2 s = pQuad->scale * 0.5f;
			if ( m_box.Max.x < (pQuad->position.x + s.x) )	m_box.Max.x = pQuad->position.x + s.x;
			if ( m_box.Max.y < (pQuad->position.y + s.y) )	m_box.Max.y = pQuad->position.y + s.y;
			if ( m_box.Max.z < (pQuad->position.z + s.x) )	m_box.Max.z = pQuad->position.z + s.x;

			if ( m_box.Min.x > (pQuad->position.x - s.x) )	m_box.Min.x = pQuad->position.x - s.x;
			if ( m_box.Min.y > (pQuad->position.y - s.y) )	m_box.Min.y = pQuad->position.y - s.y;
			if ( m_box.Min.z > (pQuad->position.z - s.x) )	m_box.Min.z = pQuad->position.z - s.x;
			
		}	//	for (int i=0; i<

		if ( m_box.Min.x == math::MAX )
		{
			m_box.Min.Set(m_ownerPos.x - 0.05f, m_ownerPos.y - 0.05f, m_ownerPos.z - 0.05f);
			m_box.Max.Set(m_ownerPos.x + 0.05f, m_ownerPos.y + 0.05f, m_ownerPos.z + 0.05f);
		}
		else
		{
			m_box.Min += m_ownerPos;
			m_box.Max += m_ownerPos;
		}

		m_sphere.Zero();
		m_sphere.ComputeByAABox(m_box);

		m_updateBox++;
		if ( m_updateBox > 15 )
		{
			m_updateBox = sx::cmn::Random(5);
			m_Owner->UpdateBoundingVolumes();
		}

		//  force to disable z writing
		DWORD op = m_material.GetActiveMaterial()->GetOption();
		SEGAN_SET_REM(op, SX_MATERIAL_ZWRITING);
		m_material.GetActiveMaterial()->SetOption(op);

	}

	void Particle::Draw( DWORD flag )
	{
		DWORD matoption = m_material.GetActiveMaterial()->GetOption();
		bool flagAlpha = SEGAN_SET_HAS( flag, SX_DRAW_ALPHA );
		bool mtrlAlpha = SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHABLEND ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAADD ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAMUL );

		if ( flag & SX_DRAW_PARTICLE &&	flagAlpha == mtrlAlpha )
		{
			int c = m_quads.Count();
			if ( c )
			{
				static Matrix matWorld = math::MTRX_IDENTICAL;
				const float3& pos = m_Owner->GetPosition_world();
				matWorld._41 = pos.x;
				matWorld._42 = pos.y;
				matWorld._43 = pos.z;
				d3d::Device3D::Matrix_World_Set(matWorld);

				CreateBuffer();
				if ( s_geometry.GetVertexCount() >= (UINT)c * QUAD_NUM_VRTX )
				{
					BurnBuffers();
					s_geometry.SetToDevice();
					m_material.SetToDevice(flag);
					d3d::Device3D::DrawPrimitive( D3DPT_TRIANGLELIST, 0, c * 2 );
				}
			}
		}

		if (flag & SX_DRAW_DEBUG && m_Owner)
		{
			sx::d3d::Device3D::RS_Alpha( 0 );

			float3 z(0,0,-0.4f);
			Matrix mat; mat.RotationX(math::PIDIV2);
			mat.Multiply( mat, m_Owner->GetMatrix_world() );

			d3d::Device3D::RS_ZWritable(false);
			d3d::UI3D::DrawCircle( mat, 0.3f, 0xffaa00aa );
			d3d::UI3D::DrawLine( math::VEC3_ZERO, z );
			d3d::UI3D::DrawLine( z, float3( 0.15f, 0,-0.2f) );
			d3d::UI3D::DrawLine( z, float3(-0.15f, 0,-0.2f) );
			d3d::UI3D::DrawLine( z, float3(0, 0.15f,-0.2f) );
			d3d::UI3D::DrawLine( z, float3(0,-0.15f,-0.2f) );

			if ( flag & SX_DRAW_BOUNDING_BOX )
			{
				d3d::Device3D::Matrix_World_Set( math::MTRX_IDENTICAL );
				d3d::UI3D::DrawWiredAABox( m_box, 0xFFFF0000 );
			}

			if ( flag & SX_DRAW_BOUNDING_SPHERE )
			{
// 				d3d::Device3D::Matrix_World_Set( math::MTRX_IDENTICAL );
// 				d3d::UI3D::DrawWiredSphere( m_sphere, 0xFFFF0000 );
				Matrix matView;
				d3d::Device3D::Matrix_View_Get( matView );
				matView.Inverse( matView );
				matView.SetTranslation(m_sphere.x, m_sphere.y, m_sphere.z );
				d3d::UI3D::DrawCircle( matView, m_sphere.r, 0xFFFF0000 );
			}
		}
	}

	FORCEINLINE void Particle::DrawByViewParam( DWORD flag, float viewParam )
	{
		float r = m_sphere.r * 2.0f;
		r /= viewParam > EPSILON ? viewParam : EPSILON;
		if ( r > s_minimumVolume )
			Draw(flag);
		//float minRange = Settings::MeshLODRange();
		//float maxRange = minRange * 4.0f;
		//int LOD = (viewParam < minRange) ? 0 : ( (viewParam < maxRange) ? 1 : 2 );
	}

	UINT Particle::MsgProc( UINT msgType, void* data )
	{
		switch (msgType)
		{
		case MT_ACTIVATE:
			{
				float d = *((float*)data);
				Validate( ComputeLodValue(d) );
			}
			break;

		case MT_DEACTIVATE:
			{
				Invalidate();
			}
			break;

		case MT_COVERBOX:
			{
				Matrix mat; mat.Inverse( m_Owner->GetMatrix_world() );
				OBBox box; box.Transform( m_box, mat );
				(static_cast<PAABox>(data))->CoverOB( box );
			}
			break;

		case MT_GETBOX_LOCAL:
			{
				s_particleBox.Max = m_box.Max - m_Owner->GetPosition_world();
				s_particleBox.Min = m_box.Min - m_Owner->GetPosition_world();
				*(static_cast<PAABox>(data)) = s_particleBox;
				return 0;
			}
			break;

		case MT_GETBOX_WORLD:
			{
				OBBox box(m_box);
				*(static_cast<POBBox>(data)) = box;
				return 0;
			}
			break;

		case MT_GETSPHERE_LOCAL:
			{
				static_cast<PSphere>(data)->Set( m_sphere.center - m_Owner->GetPosition_world(), m_sphere.radius );
				return 0;
			}
			break;

		case MT_GETSPHERE_WORLD:
			{
				static_cast<PSphere>(data)->Set( m_sphere.center, m_sphere.radius );
				return 0;
			}
			break;

		case MT_INTERSECT_RAY:
			{
				// test ray intersection and return quickly if test failed
				msg_IntersectRay* ray = static_cast<msg_IntersectRay*>(data);
				if ( !SEGAN_SET_HAS(ray->type, NMT_PARTICLE) ) return msgType;

				//  test box is enough
				float3 hitPoint, hitNorm;
				if ( ray->ray.Intersect_AABox( m_box , &hitPoint, &hitNorm) )
				{
					if ( hitPoint != ray->ray.pos )
					{
						float dis = hitPoint.Distance( ray->ray.pos );
						ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
					}
				}
			}
			break;

		case MT_PARTICLE:
			{
				msg_Particle *msgPartcl = static_cast<msg_Particle*>(data);

				if ( msgPartcl->name && m_Name != msgPartcl->name )	return msgType;
		
				if ( msgPartcl->remOption & SX_PARTICLE_LOOP )		RemOption( SX_PARTICLE_LOOP );
				if ( msgPartcl->remOption & SX_PARTICLE_LOCK_Y )	RemOption( SX_PARTICLE_LOCK_Y );
				if ( msgPartcl->remOption & SX_PARTICLE_LOCK_XZ )	RemOption( SX_PARTICLE_LOCK_XZ );
				if ( msgPartcl->remOption & SX_PARTICLE_LOCK_XY )	RemOption( SX_PARTICLE_LOCK_XY );
				if ( msgPartcl->remOption & SX_PARTICLE_LOCK_YZ )	RemOption( SX_PARTICLE_LOCK_YZ );
				if ( msgPartcl->remOption & SX_PARTICLE_SPRAY )		RemOption( SX_PARTICLE_SPRAY );

				if ( msgPartcl->addOption & SX_PARTICLE_LOOP )		AddOption( SX_PARTICLE_LOOP );
				if ( msgPartcl->addOption & SX_PARTICLE_LOCK_Y )	AddOption( SX_PARTICLE_LOCK_Y );
				if ( msgPartcl->addOption & SX_PARTICLE_LOCK_XZ )	AddOption( SX_PARTICLE_LOCK_XZ );
				if ( msgPartcl->addOption & SX_PARTICLE_LOCK_XY )	AddOption( SX_PARTICLE_LOCK_XY );
				if ( msgPartcl->addOption & SX_PARTICLE_LOCK_YZ )	AddOption( SX_PARTICLE_LOCK_YZ );
				if ( msgPartcl->addOption & SX_PARTICLE_SPRAY )
				{
					AddOption( SX_PARTICLE_SPRAY );
					m_delayTime = ( m_Option & SX_PARTICLE_RNDTIME ) ? sx::cmn::Random( m_desc.delayTime ) : 0.0f;
					m_numQuads = 0;
				}

				if ( msgPartcl->matIndex > -1 )
					m_material.SetActiveMaterial( msgPartcl->matIndex );

				msgPartcl->material = m_material.GetActiveMaterial();

				if ( msgPartcl->reset ) Reset();
			}
			break;

		case MT_PARTICLE_COUNT:
			{
				msg_Particle_Count* msgPartcl = static_cast<msg_Particle_Count*>(data);

				if ( msgPartcl->name && m_Name != msgPartcl->name )	return msgType;

				msgPartcl->particles[ msgPartcl->numParticles ] = this;
				msgPartcl->numParticles++;
			}
			break;

		}


		return msgType;
	}

	void Particle::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, particleFileID);

		// write version
		int version = 8;
		SEGAN_STREAM_WRITE(stream, version);

		//  save default things
		NodeMember::Save( stream );

		//  save description
		SEGAN_STREAM_WRITE(stream, m_desc);

		//	save quads
		int n = m_quads.Count();
		SEGAN_STREAM_WRITE(stream, n);
		if (n)
		{
			stream.Write( &m_quads[0], n * sizeof(ParticleQuad) );
		}

		SEGAN_STREAM_WRITE(stream, m_numQuads);

		// save materials
		m_material.Save(stream);

	}

	void Particle::Load( Stream& stream )
	{
		Clear();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != particleFileID)
		{
			sxLog::Log(L"Incompatible file format for loading particle !");
			return;
		}

		// read version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			//  no more supported
		}
		else if (version == 2)
		{
			//	no more supported
		}
		else if ( version == 3 )
		{
			//  no more supported
		}
		else if ( version == 4 )
		{
			//	no more supported
		}
		else if ( version == 5 )
		{
			//  load default things
			NodeMember::Load( stream );

			//  load description
			SEGAN_STREAM_READ(stream, m_desc);
			m_desc.scaleBaseY = m_desc.scaleBaseX;
			m_desc.delayTime = 0;
			m_desc.param = 0;
			SetDesc( &m_desc );

			stream.SetPos( stream.GetPos() - sizeof(float) );
			stream.SetPos( stream.GetPos() - sizeof(float) );
			stream.SetPos( stream.GetPos() - sizeof(DWORD) );

			//	read quads
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if (n)
			{
				struct ParticleQuad_5
				{
					float		age;				//! age of this quad
					float		angle;				//! current position of quad
					float		scale;				//! current scale of quad
					float3		position;			//! current position of quad
					float3		speed;				//! current speed of quad
				};

				int qsize = sizeof(ParticleQuad_5) * n;
				ParticleQuad_5* quads = (ParticleQuad_5*)sx_mem_alloc( qsize );
				stream.Read( quads, qsize );

				m_quads.SetCount( n );
				for ( int i=0; i<n; i++ )
				{
					ParticleQuad* quad = &m_quads[i];
					quad->age		= quads[i].age;
					quad->angle		= quads[i].angle;
					quad->scale.x	= quads[i].scale;
					quad->scale.y	= quads[i].scale;
					quad->position	= quads[i].position;
					quad->speed		= quads[i].speed;
					quad->param		= 0;
				}

				sx_mem_free( quads );
			}

			//  read number of quads in air
			SEGAN_STREAM_READ(stream, m_numQuads);

			// read materials
			m_material.Load(stream);
		}
		else if ( version == 6 )
		{
			//  load default things
			NodeMember::Load( stream );

			//  load description
			SEGAN_STREAM_READ(stream, m_desc);
			m_desc.delayTime = 0;
			m_desc.param = 0;
			SetDesc( &m_desc );

			stream.SetPos( stream.GetPos() - sizeof(float) );
			stream.SetPos( stream.GetPos() - sizeof(DWORD) );

			//	read quads
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if ( n )
			{
				struct ParticleQuad_6
				{
					float		age;				//! age of this quad
					float		angle;				//! current position of quad
					float2		scale;				//! current scale of quad
					float3		position;			//! current position of quad
					float3		speed;				//! current speed of quad
				};

				int qsize = sizeof(ParticleQuad_6) * n;
				ParticleQuad_6* quads = (ParticleQuad_6*)sx_mem_alloc( qsize );
				stream.Read( quads, qsize );
				
				m_quads.SetCount( n );
				for ( int i=0; i<n; i++ )
				{
					ParticleQuad* quad = &m_quads[i];
					quad->age		= quads[i].age;
					quad->angle		= quads[i].angle;
					quad->scale		= quads[i].scale;
					quad->position	= quads[i].position;
					quad->speed		= quads[i].speed;
					quad->param		= 0;
				}

				sx_mem_free( quads );
			}

			//  read number of quads in air
			SEGAN_STREAM_READ(stream, m_numQuads);

			// read materials
			m_material.Load(stream);
		}
		else if ( version == 7 )
		{
			//  load default things
			NodeMember::Load( stream );

			//  load description
			SEGAN_STREAM_READ(stream, m_desc);
			m_desc.param = 0;
			SetDesc( &m_desc );
			stream.SetPos( stream.GetPos() - sizeof(DWORD) );

			//	read quads
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if ( n )
			{
				m_quads.SetCount( n );
				stream.Read( &m_quads[0], n * sizeof(ParticleQuad) );
			}

			//  read number of quads in air
			SEGAN_STREAM_READ(stream, m_numQuads);

			// read materials
			m_material.Load(stream);
		}
		else if ( version == 8 )
		{
			//  load default things
			NodeMember::Load( stream );

			//  load description
			SEGAN_STREAM_READ(stream, m_desc);
			SetDesc( &m_desc );

			//	read quads
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if ( n )
			{
				m_quads.SetCount( n );
				stream.Read( &m_quads[0], n * sizeof(ParticleQuad) );
			}

			//  read number of quads in air
			SEGAN_STREAM_READ(stream, m_numQuads);

			// read materials
			m_material.Load(stream);
		}

		LOADED_LEVEL = 2;
	}

	void Particle::SetDesc( ParticleDesc *pDesc )
	{
		if (!pDesc) return;
		m_desc = *pDesc;

		if ( m_desc.flow > 0.001f )
		{
			m_flowTime = m_flowTimeMax = 1000.0f / m_desc.flow;
		}
		else
		{
			m_flowTime = m_flowTimeMax = m_desc.flow = 0;
		}

		CreateBuffer();

		m_numQuads = 0;
	}

	FORCEINLINE const ParticleDesc *Particle::GetDesc( void )
	{
		return &m_desc;
	}

	void Particle::Reset( void )
	{
		m_quads.Clear();
		CreateBuffer();

		m_numQuads = 0;

		if ( m_Owner )
		{
			m_box.Min = m_Owner->GetPosition_world();
			m_box.Max = m_Owner->GetPosition_world();
			m_sphere.Set( m_Owner->GetPosition_world(), 0.0f );
			
			m_Owner->UpdateBoundingVolumes();
		}
	}

	FORCEINLINE MaterialMan& Particle::Material( void )
	{
		return m_material;
	}

	FORCEINLINE int Particle::ComputeLodValue( float viewParam )
	{
		return viewParam<500 ? 0 : (viewParam<5000 ? 1 : 2);
	}

	FORCEINLINE void Particle::SetupQuad( ParticleQuad& quad, const float age )
	{
		quad.age = age;
		quad.angle = cmn::Random(m_desc.angleBase) - m_desc.angleBase * 0.5f;
		quad.scale.x = m_desc.scaleBaseX;
		quad.scale.y = m_desc.scaleBaseY;

		float3 initRange
			(
			cmn::Random(m_desc.initRange.x) - m_desc.initRange.x * 0.5f,
			cmn::Random(m_desc.initRange.y) - m_desc.initRange.y * 0.5f,
			cmn::Random(m_desc.initRange.z) - m_desc.initRange.z * 0.5f
			);
		quad.position.Transform_Norm( initRange, m_Owner->GetMatrix_world() );

		static const float s_DegToRad = PI / 180.0f;
		float tta = ( cmn::Random(m_desc.initSpeed.x) - m_desc.initSpeed.x * 0.5f ) * s_DegToRad;
		float phi = ( cmn::Random(m_desc.initSpeed.z) - m_desc.initSpeed.z * 0.5f ) * s_DegToRad;
		quad.speed.Set
			(
			sinf( tta ),
			cosf( tta ) * cosf( phi ),
			sinf( phi )
			);
		quad.speed *= m_desc.initSpeed.y;

		quad.speed.Transform_Norm( quad.speed, m_Owner->GetMatrix_world() );

		SEGAN_3TH_BYTEOF(quad.param) = (BYTE)sx::cmn::Random( SEGAN_3TH_BYTEOF(m_desc.param) );
	}

	FORCEINLINE void Particle::CreateBuffer( void )
	{
		int numVertex = QUAD_NUM_VRTX * m_quads.Count();
		if ( s_geometry.GetVertexCount() < (UINT)numVertex )
		{
			s_geometry.Cleanup();
			s_geometry.VB_CreateByIndex( numVertex, 0, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
			s_geometry.VB_CreateByIndex( numVertex, 1, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
		}
	}

	FORCEINLINE void Particle::BurnBuffers( void )
	{
		//  get current view matrix
		Matrix matTmp, matScl, matView, matViewInv;

		if ( m_Option & SX_PARTICLE_LOCK_XZ )
		{
			if ( m_Option & SX_PARTICLE_LOCK_Y )
			{
				d3d::Device3D::Matrix_View_Get(matView);
				matViewInv.Inverse( matView );
				float3 eye( matViewInv._41, 1.0f, matViewInv._43 );
				float3 at( eye.x + matView._13 * 0.00001f, 0, eye.z + matView._33 * 0.00001f );
				float3 up( 0, 1, 0 );
				matView.LookAtLH( eye, at, up );
				matViewInv.Inverse( matView );
			}
			else matViewInv.RotationX( sx::math::PIDIV2 );
		}
		else if ( m_Option & SX_PARTICLE_LOCK_Y )
		{
			d3d::Device3D::Matrix_View_Get(matView);
			matViewInv.Inverse( matView );
			float3 eye( matViewInv._41, 0, matViewInv._43 );
			float3 at( eye.x + matView._13, 0, eye.z + matView._33 );
			float3 up( 0, 1, 0 );
			matView.LookAtLH( eye, at, up );
			matViewInv.Inverse( matView );
		}
		else if ( m_Option & SX_PARTICLE_LOCK_XY )
		{
			matViewInv.RotationY( PI );
		}
		else if ( m_Option & SX_PARTICLE_LOCK_YZ )
		{
			matViewInv.RotationY( - sx::math::PIDIV2 );
		}
		else
		{
			d3d::Device3D::Matrix_View_Get(matView);
			matViewInv.Inverse( matView );
		}

		//  at first sort quads
		s_camDir.Set( matView._13, matView._23, matView._33 );
		m_quads.Sort( &CompareQuadDistance );

		//  define simple quad
		float3 v[4];
		v[0].Set( -0.5f,  0.5f,  0.0 );
		v[1].Set(  0.5f,  0.5f,  0.0 );
		v[2].Set(  0.5f, -0.5f,  0.0 );
		v[3].Set( -0.5f, -0.5f,  0.0 );

		PVertex0 v0 = NULL;
		if ( s_geometry.VB_Lock( 0, (PVertex&)v0 ) )
		{
			PVertex1 v1 = NULL;
			if ( s_geometry.VB_Lock( 1, (PVertex&)v1 ) )
			{

				for (int i=0; i<m_quads.Count(); i++)
				{
					//  update particle quad properties
					ParticleQuad *pQuad = &m_quads[i];
					
					//  make quad matrix
					matScl.Scale(pQuad->scale.x, pQuad->scale.y, pQuad->scale.x);
					matTmp.RotationZ(pQuad->angle);
					matTmp.Multiply( matScl, matTmp );
					matTmp.Multiply( matTmp, matViewInv );
					
					matTmp._41 = pQuad->position.x;
					matTmp._42 = pQuad->position.y;
					matTmp._43 = pQuad->position.z;

					//  send positions to vertex buffer
					int j = i * QUAD_NUM_VRTX;
					v0[j  ].Transform( matTmp, v[0] );
					v0[j+1].Transform( matTmp, v[1] );
					v0[j+2].Transform( matTmp, v[2] );
					v0[j+3] = v0[j  ];	//.Transform( matTmp, v[0] );
					v0[j+4] = v0[j+2];	//.Transform( matTmp, v[2] );
					v0[j+5].Transform( matTmp, v[3] );

					//  send texcoords to vertex buffer
					v1[j+0].txc.x = 0.0f;	v1[j+0].txc.y = 0.0f;
					v1[j+1].txc.x = 1.0f;	v1[j+1].txc.y = 0.0f;
					v1[j+2].txc.x = 1.0f;	v1[j+2].txc.y = 1.0f;

					v1[j+3].txc.x = 0.0f;	v1[j+3].txc.y = 0.0f;
					v1[j+4].txc.x = 1.0f;	v1[j+4].txc.y = 1.0f;
					v1[j+5].txc.x = 0.0f;	v1[j+5].txc.y = 1.0f;

					//  send other information to vertex
					for (int k=0; k < QUAD_NUM_VRTX; k++)
					{
						v1[j+k].col0 = 0xffffffff;
						v1[j+k].col1 = pQuad->param;
						v1[j+k].nrm.x = pQuad->age;
						v1[j+k].nrm.y = m_desc.lifeTime;
						v1[j+k].nrm.z = pQuad->angle;
					}

				}

				s_geometry.VB_UnLock(1);
			}

			s_geometry.VB_UnLock(0);
		}
	}

	void Particle::UpdateQuad( ParticleQuad* pQuad, const float etime, const float* force, const float* ownerSpeed )
	{
		pQuad->age		+= etime;
		pQuad->angle	+= m_desc.angleSpeed * etime;

		float aspect = m_desc.scaleBaseX / ( m_desc.scaleBaseY > 0.001f ? m_desc.scaleBaseY : 0.001f);
		pQuad->scale.x	+= m_desc.scaleSpeed * etime * aspect;
		pQuad->scale.y	+= m_desc.scaleSpeed * etime;

		if ( !LOADED_LEVEL && m_desc.moveIndependently > EPSILON )
		{
			const float osValu = etime * m_desc.moveIndependently;
			pQuad->position.x += pQuad->speed.x * etime + force[0] * pQuad->age - ownerSpeed[0] * osValu;
			pQuad->position.y += pQuad->speed.y * etime + force[1] * pQuad->age - ownerSpeed[1] * osValu;
			pQuad->position.z += pQuad->speed.z * etime + force[2] * pQuad->age - ownerSpeed[2] * osValu;
		}
		else
		{
			pQuad->position.x += pQuad->speed.x * etime + force[0] * pQuad->age;
			pQuad->position.y += pQuad->speed.y * etime + force[1] * pQuad->age;
			pQuad->position.z += pQuad->speed.z * etime + force[2] * pQuad->age;
		}

		if ( pQuad->age > m_desc.lifeTime )
			pQuad->age = m_desc.lifeTime;
	}



	void Particle::Manager::BeginPatch( DWORD flag )
	{

	}

	void Particle::Manager::EndPatch( DWORD flag )
	{

	}

	float& Particle::Manager::MinimumVolume( void )
	{
		return s_minimumVolume;
	}

}}  //  namespace sx { namespace core {