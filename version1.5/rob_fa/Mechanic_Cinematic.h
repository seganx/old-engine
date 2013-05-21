/********************************************************************
	created:	2012/09/23
	filename: 	Mechanic_Cinematic.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple mechanic to play cinematic in game
*********************************************************************/
#ifndef GUARD_Mechanic_Cinematic_HEADER_FILE
#define GUARD_Mechanic_Cinematic_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"



class FirstPresents
{
public:
	FirstPresents( void );
	~FirstPresents( void );
	void AddPresents( const WCHAR* texture, const float size );
	void Update( float elpstime );
	void Draw( void );
	bool Presenting( void );

public:
	Array<sx::gui::Control*>	m_list;
	int							m_index;
	float						m_time;
	float						m_maxtime;
	sx::core::Node*				m_soundNode;
	float						m_soundVolume;
};


namespace GM
{
	//! use this mechanic class to implement a simple cinematic
	class Mechanic_Cinematic: public Mechanic
	{

	public:

		Mechanic_Cinematic(void);
		~Mechanic_Cinematic(void);

		//! initialize mechanic after game start
		void Initialize(void);

		//! finalize mechanic before game closed
		void Finalize(void);

		//! process player input
		void ProcessInput(bool& inputHandled, float elpsTime);

		//! update mechanic
		void Update(float elpsTime);

		//! process messages
		void MsgProc(UINT recieverID, UINT msg, void* data);

	};


} // namespace GM

#endif	//	GUARD_Mechanic_Cinematic_HEADER_FILE
