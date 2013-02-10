/********************************************************************
	created:	2011/01/22
	filename: 	sxPanelEx.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the class of extended panel that support
				multi states as same as animation key frame
*********************************************************************/
#ifndef GUARD_sxPanelEx_HEADER_FILE
#define GUARD_sxPanelEx_HEADER_FILE

#include "sxControl.h"
#include "../sxCommon/sxCommon.h"

namespace sx { namespace gui {

	//! this is the structure of state that used in extended panel as same as animation key frame
	struct SEGAN_API GUIPanelState
	{
		float2		Align;			//  align system of the extended panel
		float3		Center;			//  center of panel
		float3		Position;		//  position of panel
		float3		Rotation;		//  rotation of panel
		float3		Scale;			//  scale of panel
		float4		Color;			//  color of panel
		float2		Blender;		//  blending weights / velocity / amplitude

		GUIPanelState(){
			ZeroMemory( this, sizeof(GUIPanelState) );
		}
	};


	//! extended panel is most useful object in my gui. it support multi states as same as key frames in animation system
	class SEGAN_API PanelEx: public Control
	{
		SEGAN_STERILE_CLASS(PanelEx);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		PanelEx(void);
		virtual ~PanelEx(void);

		//! save the current control to the stream ( all EXCEPT Parent & UserData )
		void Save( Stream& S );

		//! load control properties from stream	( all EXCEPT Parent & UserData )
		void Load( Stream& S );

		//! return the size of this control
		void SetSize(float2 S);

		//! reference to position vector of current state
		Vector3& Position(void);

		//! reference to rotation vector of current state
		Vector3& Rotation(void);

		//! reference to rotation vector of current state
		Vector3& Scale(void);

		//! calculate world matrix to draw GUI correctly and process inputs. return true if process handled by itself or it's childes
		void Update(float elpsTime);

		//! draw GUI
		void Draw(DWORD option);
		
		//! clear all states except one first state
		void State_Clear(void);

		//! return the number of states
		int State_Count(void);

		//! add a new state and fill it by current state and return index of new state
		int State_Add(void);

		//! remove an state by specified index
		void State_Remove(int index);

		//! return the index of current state
		int State_GetIndex(void);

		//! set index of the state
		void State_SetIndex(int index);

		//! return reference to the current state structure
		GUIPanelState& State_GetCurrent(void);

		//! return reference to state by index
		GUIPanelState& State_GetByIndex(int index);

		//! return reference of the blended state structure
		GUIPanelState& State_GetBlended(void);

		//! return true if the state is blending
		bool State_IsBlending(void);

		//! set a new pixel shader for this panel
		void Shader_Set(const WCHAR* strShader);

		//! return the current shader in the panel
		const WCHAR* Shader_Get(void);

		//! compile the current pixel shader.
		void Shader_Compile(void);

		//! clear and release the current shader
		void Shader_Clean(void);

		//! set scene texture to use in shader programming
		void SetSceneTexture(d3d::PTexture3D pSceneTexture);

		//! blend between states
		void BlendStates(float elpsTime);

	protected:
		//! build matrix by current properties/ override this function to create your own matrix
		void BuildMatrix(void);

	private:
		
		String					m_PS_part;

		d3d::PTexture3D			m_pTexture;
		d3d::Shader3D			m_Shader;
		D3DShaderHandle			m_pWVP;
		D3DShaderHandle			m_pColor;
		D3DShaderHandle			m_pTime;

		GUIPanelState			m_curState;
		GUIPanelState			m_oldState;
		int						m_iCurState;

		float					m_time;
		cmn::WeightBlender		m_WBlend;
		Array<GUIPanelState>	m_States;
	};
	typedef PanelEx *PPanelEx;



}}	//	namespace sx { namespace gui {


#endif	//	GUARD_sxPanelEx_HEADER_FILE