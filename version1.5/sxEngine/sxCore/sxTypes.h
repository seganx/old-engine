/********************************************************************
	created:	2010/11/05
	filename: 	sxTypes.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some data types and enumeration types
				that used in engine.
*********************************************************************/
#ifndef GUARD_sxTypes_HEADER_FILE
#define GUARD_sxTypes_HEADER_FILE

//! forward declaration
//class Node;
//class NodeMember;

//! define the type of node's member
#define NMT_
enum NodeMemberType
{
	NMT_NULL					= 0x00000000,
	NMT_UNKNOWN					= 0x00000001,
	NMT_MESH					= 0x00000002,
	NMT_ANIMATOR				= 0x00000004,
	NMT_GUI						= 0x00000008,
	NMT_PARTICLE				= 0x00000010,
	NMT_PATHNODE				= 0x00000020,
	NMT_TERRAIN					= 0x00000040,
	NMT_SOUND					= 0x00000080,

	NMT_ALL						= 0xFFFFFFFF
};

//! option permission type describes who allowed/denied an option in the settings
#define OPT_
enum OptionPermissionType
{
	OPT_BY_SYSTEM	= 0x00100000,	//  option is permitted by engine
	OPT_BY_ARTIST	= 0x00200000,	//  option is permitted by artist
	OPT_BY_USER		= 0x00400000	//  option is permitted by user
};

//! these are message type of the communication system between nodes and members
#define MT_
enum MessageType
{
	MT_ACTIVATE,					//! request to activate resources. data will contain a pointer to a float that describe view distance
	MT_DEACTIVATE,					//! request to deactivate resources. data will be NULL
	MT_GETBOX_WORLD,				//! request to get box in world space. data will contain pointer to OBBox
	MT_GETBOX_LOCAL,				//! request to get box in local space. data will contain pointer to AABox
	MT_COVERBOX,					//! request to cover boxes in attached box. data will contain pointer to AABox
	MT_GETSPHERE_WORLD,				//! request to get sphere in world space. data will contain pointer to Sphere
	MT_GETSPHERE_LOCAL,				//! request to get sphere in local space. data will contain pointer to Sphere
	
	MT_NODE_DELETE,					//! notify that a node is going to delete. data will be pointer to the node
	MT_MEMBER_ATTACHED,				//! notify that a member has been attached to a node. data will contain a pointer to the member
	MT_MEMBER_DETACHED,				//! notify that a member has been detached from a node. data will contain a pointer to the member
	
	MT_INTERSECT_RAY,				//! fill out intersection ray structure as data to pick objects in space
	
	MT_MESH,						//! modify/change property of mesh(es) in in the node. data must be a pointer to mesh message
	MT_MESH_COUNT,					//! count meshes in the node and retrieve pointers to meshes. data must be a pointer to meshes counter structure

	MT_ANIMATOR,					//! modify/change property of animation(s) in in the node. data must be a pointer to animator message
	MT_ANIMATOR_COUNT,				//! count animators in the node and retrieve pointers to animators. data must be a pointer to animator counter structure
	
	MT_PARTICLE,					//! modify/change property of particle(s) in in the node. data must be a pointer to particle message
	MT_PARTICLE_COUNT,				//! count particles in the node and retrieve pointers to particles. data must be a pointer to particles counter structure
	
	MT_SOUND_PLAY,					//!	play sound in the node. data can be null or pointer to sound play message structure
	MT_SOUND_STOP,					//!	stop/pause sound in the node. data should be null or pointer to sound stop message structure

	MT_SCENE_ADD_NODE,				//! notify that a new node has been added to the scene. data will be pointer to the new node
	MT_SCENE_REMOVE_NODE,			//! notify that a node has been removed from the scene. data will be pointer to the node
	MT_SCENE_CLEAN,					//! notify that scene is going to clear
};


//! draw flag use to choose a draw mode for a member
#define SX_DRAW_DEBUG				0x00001000		//! draw the nodes ( just bounding box or bounding sphere )
#define SX_DRAW_MESH				0x00002000		//! draw the mesh content
#define SX_DRAW_TERRAIN				0x00004000		//! draw terrain node
#define SX_DRAW_PARTICLE			0x00008000		//! draw particles
#define SX_DRAW_ALPHA				0x00010000		//! draw objects with alpha material
#define SX_DRAW_PATH				0x00020000		//! draw path nodes
#define SX_DRAW_BOUNDING_BOX		0x00040000		//! draw the bounding box of the member
#define SX_DRAW_BOUNDING_SPHERE		0x00080000		//! draw the bounding sphere of the member
#define SX_DRAW_WIRED				0x00100000		//! draw the member in wired frame
#define SX_DRAW_SELECTED			0x00200000		//! use special color to draw selected node


//! node properties describe node's behavior 'SX_NODE_'
#define SX_NODE_
#define SX_NODE_LOYAL				0x00000100		//! the node can't be detach from parent to change parent
#define SX_NODE_ABSOLUTE			0x00000200		//! the node is absolute and will not removed/saved by the other nodes
#define SX_NODE_SOLITARY			0x00000400		//! the node will not update bounding box of its parent. changes of the node will not effect to parent
#define SX_NODE_SELECTABLE			0x00000800		//!	the node and all it's children and members can intersect by ray and can be selected by mouse

//! mesh properties describe mesh's behavior 'SX_MESH_'
#define SX_MESH_
#define SX_MESH_SELECTABLE			0x00000001		//! the mesh can't intersect with any ray and can't be selected by mouse
#define SX_MESH_CASTSHADOW			0x00000002		//! the mesh can cast it's shadow in shadow map
#define SX_MESH_RECIEVESHADOW		0x00000004		//! the mesh can receive shadow from shadow map
#define SX_MESH_INVISIBLE			0x00000008		//! the mesh will not draw to the scene
#define SX_MESH_INVIS_IN_REFLET		0x00000010		//! the mesh will not draw in any reflection texture
#define	SX_MESH_REFLECTOR			0x00000020		//!	the mesh will make a reflector object

//! animator properties describe animator's behavior 'SX_ANIMATOR_'
#define SX_ANIMATOR_
#define SX_ANIMATOR_PLAY			0x00000001		//! animator play the animation
#define SX_ANIMATOR_LOOP			0x00000002		//! animator replay the animation at the end of animation


//! particle properties describe particles's behavior 'SX_PARTICLE_'
#define SX_PARTICLE_
#define SX_PARTICLE_LOOP			0x00000001		//! continue to emit particles when all quads has been sprayed
#define SX_PARTICLE_SPRAY			0x00000002		//! spray particles in the world
#define SX_PARTICLE_LOCK_Y			0x00000004		//!	lock particle transformation on Y axis
#define SX_PARTICLE_LOCK_XZ			0x00000008		//!	lock particle transformation on XZ plane
#define SX_PARTICLE_INVIS_IN_REFLET	0x00000010		//!	the particle will not draw in any reflection texture
#define SX_PARTICLE_LOCK_XY			0x00000020		//!	lock particle transformation on XY plane
#define SX_PARTICLE_LOCK_YZ			0x00000040		//!	lock particle transformation on YZ plane
#define SX_PARTICLE_RNDTIME			0x00000080		//!	spay particle in random time specified in a member which named delay time

//! sound properties describe sounds's behavior 'SX_SOUND_'
#define SX_SOUND_
#define SX_SOUND_PLAYONLOAD			0x00000001		//!	start playing after the sound loaded immediately
#define SX_SOUND_LOOP				0x00000002		//!	restart playing after the sound finished
#define SX_SOUND_PLAYING			0x00000004		//! use to internal control



//////////////////////////////////////////////////////////////////////////
//	MESSAGE STRUCTURES
//////////////////////////////////////////////////////////////////////////

//! message structures
struct msg_IntersectRay
{
	enum DepthLevel						//! level of intersection test
	{
		SPHERE,							//! test only by sphere
		BOX,							//! test by sphere and box
		GEOMETRY						//! test by geometry after box and sphere
	};

	struct result
	{
		float	distance;				//! return distance if intersection happened
		float3	position;				//! intersection point in world space
		float3	normal;					//! intersection normal in world space
		void*	node;					//! intersected node
		void*	member;					//! intersected node member
	};

	const	WCHAR*		name;			//! object name
	IN		DWORD		type;			//! types of members which can be test intersect. set to ZERO to select root node
	IN		Ray			ray;			//! ray structure to test intersection
	IN		DepthLevel	depthLevel;		//! level of intersection test

	OUT		result		results[32];	//! intersected results


	msg_IntersectRay(IN DWORD _type, IN Ray& _ray, IN DepthLevel dlevel, const WCHAR* pname)
		: name(pname)
		, type(_type)
		, ray(_ray.pos, _ray.dir)
		, depthLevel(dlevel)
	{
			for (int i=0; i<32; i++)
			{
				results[i].distance = FLT_MAX;
				results[i].position.Set(0,0,0);
				results[i].normal.Set(0,0,0);
				results[i].node = NULL;
				results[i].member = NULL;
			}
	}

	void pushToResults(float dis, float3 pos, float3 norm, void* _node, void* _member)
	{
		for ( int i=0; i<32; i++ )
		{
			if ( results[i].node )
			{
				if ( results[i].distance > dis )
				{
					for ( int j=31; j>i; j-- ) results[j] = results[j-1];

					results[i].distance = dis;
					results[i].position = pos;
					results[i].normal	= norm;
					results[i].node		= _node;
					results[i].member	= _member;
					return;
				}
			}
			else
			{
				results[i].distance = dis;
				results[i].position = pos;
				results[i].normal	= norm;
				results[i].node		= _node;
				results[i].member	= _member;
				return;
			}
		}
	}
};

struct msg_Animator
{
	const WCHAR*	name;			//	the name of an specified animation. this can be null
	DWORD			addOption;		//  add these options to the animator
	DWORD			remOption;		//  remove these options from animator
	int				animIndex;		//  set new animation index. pass -1 to leave default
	float			animSpeed;		//	set a new animation speed. pass -1 to leave default
	float			animSpeedScale;	//	set a new animation speed scale. pass -1 to leave default
	float			animTime;		//	time of animation. pass -1 to leave current

	msg_Animator(DWORD _addOption, DWORD _remOption=0, const WCHAR* _name=0, int _animIndex=-1, float _animSpeed=-1, float _animSpeedScale=-1, float _animTime = -1 )
		: addOption(_addOption), remOption(_remOption), name(_name), animIndex(_animIndex), animSpeed(_animSpeed), animSpeedScale(_animSpeedScale), animTime(_animTime) {}
};

struct msg_Animator_Count
{
	const WCHAR*	name;			//	the name of an specified animator to retrieve. this can be null to retrieve all animator
	int				numAnimator;	//	number of animators founded in node
	void*			animators[16];	//  pointer to animator objects

	msg_Animator_Count(const WCHAR* _name=0): name(_name), numAnimator(0) {}
};

struct msg_Particle
{
	const WCHAR*	name;			//	the name of an specified particle. this can be null
	DWORD			addOption;		//  add these options to the particle
	DWORD			remOption;		//  remove these options from particle
	int				matIndex;		//	set material index, pass -1 to avoid changes
	void*			material;		//	fill out pointer to current material object. return null if no material found
	bool			reset;			//  reset particle object

	msg_Particle(DWORD _addOption, DWORD _remOption=0, const WCHAR* _name=0, bool _reset=false, int matindex=-1)
		: addOption(_addOption), remOption(_remOption), name(_name), reset(_reset), matIndex(matindex), material(0) {}
};

struct msg_Particle_Count
{
	const WCHAR*	name;			//	the name of an specified particle to retrieve. this can be null to retrieve all particle
	int				numParticles;	//	number of particle founded in node
	void*			particles[16];	//  pointer to particle objects

	msg_Particle_Count(const WCHAR* _name=0): name(_name), numParticles(0) {}
};

struct msg_SoundPlay
{
	const WCHAR*	name;			//  use sound name to play special sound member in the node
	DWORD			option;			//	use sound option to play special sound member in the node which has this option
	int				addFreq;		//  additional frequency to play a sound with different style
	bool			restart;		//  play sound from beginning
	int				index;			//	index of sound to play. pass -1 to leave it to current

	msg_SoundPlay(bool _restart, int _addFreq=0, DWORD _option=0, const WCHAR* _name=0, const int _index = -1)
		: restart(_restart), addFreq(_addFreq), option(_option), name(_name), index(_index) {}
};

struct msg_SoundStop
{
	const WCHAR*	name;		//  use sound name to stop/pause special sound member in the node
	DWORD			option;		//	use sound option to stop/pause special sound member in the node which has this option
	bool			pause;		//  just pause the sound and do not stop it

	msg_SoundStop(bool _pause, DWORD _option=0, const WCHAR* _name=0)
		: pause(_pause), option(_option), name(_name) {}
};

struct msg_Mesh
{
	const WCHAR*	name;		//	the name of an specified mesh. this can be null for apply to all meshes
	DWORD			addOption;	//  add these options
	DWORD			remOption;	//  remove these options
	int				matIndex;	//	set material index, pass -1 to fill out current index
	void*			material;	//	fill out pointer to current material object. return null if no material found

	msg_Mesh(DWORD _addOption, DWORD _remOption=0, const WCHAR* _name=0, int _matIndex=-1)
		: addOption(_addOption), remOption(_remOption), name(_name), matIndex(_matIndex), material(0) {}
};

struct msg_Mesh_Count
{
	const WCHAR*	name;			//	the name of an specified mesh to retrieve. this can be null to retrieve all meshes
	int				numMeshes;		//	number of meshes founded in node
	void*			meshes[16];		//  pointer to meshes objects

	msg_Mesh_Count(const WCHAR* _name=0): name(_name), numMeshes(0) {}
};

//////////////////////////////////////////////////////////////////////////
//! particle quad describe information of each quad
struct ParticleQuad
{
	float		age;				//! age of this quad
	float		angle;				//! current position of quad
	float2		scale;				//! current scale of quad
	float3		position;			//! current position of quad
	float3		speed;				//! current speed of quad
	DWORD		param;				//! pass as color 1 to shader but can contain more information

	ParticleQuad(void): age(0), angle(0), scale(0,0), position(0,0,0), speed(0,0,0), param(0) {}

	bool operator == (ParticleQuad& p){	return false; }
};

//! particle description
struct ParticleDesc
{
	int			quadCount;			//!  number of display quads
	float		flow;				//!	 how many quads sprite in a second
	float		lifeTime;			//!  max life time of quads
	float		angleBase;			//!  angle base of quads
	float		angleSpeed;			//!  angle speed of quads
	float		scaleBaseX;			//!  init value of X scale of quads
	float		scaleSpeed;			//!  scale speed of quads
	float		moveIndependently;	//!  particles move independently from owner
	float3		initSpeed;			//!  initial direction of quads speed
	float3		initRange;			//!  radius of random position in quad initialization
	float3		gravity;			//!  additional force of gravity will apply to particles
	float3		wind;				//!  additional force of wind will apply to particles
	float		scaleBaseY;			//!  init value of Y scale of quads
	float		delayTime;			//!  delay time to spray particle
	DWORD		param;				//!	 some useful info

	ParticleDesc(void)
		: quadCount(0), lifeTime(0), angleBase(0), angleSpeed(0), scaleBaseX(0), scaleBaseY(0), scaleSpeed(0.1f), 
		moveIndependently(0), initSpeed(0,0,0), initRange(0,0,0), gravity(0,0,0), wind(0,0,0), flow(1), delayTime(0),
		param(0) {}
};


#endif	//	GUARD_sxTypes_HEADER_FILE
