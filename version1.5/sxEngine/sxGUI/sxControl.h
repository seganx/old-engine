/********************************************************************
	created:	2010/11/25
	filename: 	sxControl.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the base of other GUI controls.
				all other GUI class will inherit this class.
*********************************************************************/
#ifndef GUARD_sxControl_HEADER_FILE
#define GUARD_sxControl_HEADER_FILE

#include "sxElement.h"

namespace sx { namespace gui {

	typedef class Control *PControl;
	typedef Array<PControl> ArrayPControl;

	//! this class is the base of all other GUI controls presented in SeganX engine
	class SEGAN_API Control
	{
		friend class Element;
		SEGAN_STERILE_CLASS(Control);
	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();
		Control(void);
		virtual ~Control(void);
	public:

		//! set a new name
		void SetName(const WCHAR* pwcName);

		//! return the name of the control
		const WCHAR* GetName(void);

		//! set hint
		void SetHint(const WCHAR* pwcName);

		//! return the hint of the control
		const WCHAR* GetHint(void);

		//! return type of this control
		GUIControlType	GetType(void);

		//! save the current control to the stream ( all EXCEPT Parent & UserData )
		virtual void	Save( Stream& S );

		//! load control properties from stream	( all EXCEPT Parent & UserData )
		virtual void	Load( Stream& S );

		//! set width and height of this control
		virtual	float2	GetSize(void);

		//! return the size of this control
		virtual void	SetSize(float2 S);

		//! set a new rectangle to this control
		RectF	GetRect(void);

		//! add a new property 'SX_GUI_PROPERTY_'
		virtual void AddProperty(DWORD prop);

		//! remove a property from current property set 'SX_GUI_PROPERTY_'
		virtual void RemProperty(DWORD prop);

		//! return true if this control has specified property 'SX_GUI_PROPERTY_'
		bool	HasProperty(DWORD prop);

		//! return user data of this control
		void*	GetUserData(void);

		//! set a new user data to this control
		void	SetUserData(void* userData);

		//! return user tag of this control
		DWORD	GetUserTag(void);

		//! set a new user tag to this control
		void	SetUserTag(DWORD userTag);

		//! return true if this control has focused
		bool	GetFocused(void);

		//! set this object focused
		void	SetFocused(bool val);

		//! return the parent of object. return null if object has no parent
		PControl	GetParent(void);

		//! set the parent of object. set null to make object single
		void		SetParent(PControl parent);

		//! return the child of the control. return null if no child found
		PControl GetChild(int index);

		//! find and return the index of child by name. return -1 if no match found.
		int	GetChild(const WCHAR* pwcName, PControl& OUT pControl);

		//! return list of all children of children
		void GetChildren(ArrayPControl& OUT controlList);

		//! return number of childes
		int	GetChildCount(void);

		//! return number of elements in this control
		int	GetElementCount(void);

		//! return the specific element of control
		PElement	GetElement(int index);

		//! return selected element by mouse down
		PElement	GetSelectedElement(void);

		//! set callback function for mouse click
		void SetOnMouseClick(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for mouse enter
		void SetOnMouseEnter(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for mouse leave
		void SetOnMouseExit(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for mouse move
		void SetOnMouseMove(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for mouse wheel
		void SetOnMouseWheel(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for key down
		void SetOnKeyDown(PForm pForm, GUICallbackEvent pFunc);

		//! reference to position vector of this control
		virtual Vector3& Position(void);

		//! reference to rotation vector of this control
		virtual Vector3& Rotation(void);

		//! reference to scale vector of this control. NOTE: work only on some control !
		virtual Vector3& Scale(void);

		//! reference to position vector of this control
		Vector3& PositionOffset(void);

		//! reference to rotation vector of this control
		Vector3& RotationOffset(void);

		//! reference to position vector of this control
		Vector3& ScaleOffset(void);

		//! reference to matrix of this control
		Matrix& GetMatrix(void);

		//! calculate world matrix to draw GUI correctly.
		virtual void Update(float elpsTime);

		//! process inputs. set 'result' true if process handled by itself or it's childes.
		virtual void ProcessInput(bool& result, int playerID = 0);

		//! draw GUI
		virtual void Draw(DWORD option);
		
		//! draw GUI border
		virtual void DrawOutline(void);

		//! use to communicate by this control
		virtual UINT MsgProc(UINT msgType, void* data);

		//! create and return a new GUI control just like this with all childes
		Control* Clone(void);

		//! return focused control
		static Control* GetFocusedControl(void);

		//! return current control under cursor
		static Control*& GetCapturedControl(void);

	protected:
		//! build matrix by current properties/ override this function to create your own matrix
		virtual void BuildMatrix(void);

		/*return index of element contacted by mouse ray. return -1 if no contact
		NOTE: this function will called on update.*/
		virtual int	MouseOver(float absX, float absY);

	public:
		/*return index of element contacted by mouse ray and uv point of intersection. return -1 if no contact
		NOTE: this function will NOT called on update. call it manually*/
		virtual int MouseOver(float absX, float absY, OUT math::Vector2& uv, int element = -1 );

		//! return index of element contacted by mouse ray. return -1 if no contact
		virtual int	MouseDown(float absX, float absY);

		//! return index of element contacted by mouse ray. return -1 if no contact
		virtual int	MouseUp(float absX, float absY);

		/*! call this function to pass keyboard events to gui
		set KeyCode in LOWORD and set extended KeyCode in HIWORD */
		virtual DWORD	OnKeyDown(DWORD keyCode);

		/*! call this function to pass keyboard events to gui
		set KeyCode in LOWORD and set extended KeyCode in HIWORD */
		virtual DWORD	OnKeyPress(DWORD KeyCode);

		//! process keys from hardware input device
		virtual int	ProcessKeyboardInput(const char* pKeys, GUICallbackInputToChar InputCharFunc = NULL);

		//! set number of elements in this control. this function will destroy current controls
		void	SetElementCount(int count);

		//! draw it's childes
		void Draw_Childes(DWORD option);

		//! draw line it's childes
		void DrawLine_Childes(void);

		//! save it's children
		void Save_Childes(Stream& S);

		//! load it's childes
		void Load_Childes(Stream& S);

		String		m_Name;				//  name of this control
		String		m_Hint;				//  use to show hint
		GUIControlType		m_Type;				//  type of this control
		DWORD				m_Option;			//  as I said before! It's really helpful

		float2				m_Size;				//  control's dimension
		PElementArray		m_Elements;			//  array of elements
		int					m_SelectedElement;	//  selected element

		math::Vector3		m_Pos;				//  position of this control
		math::Vector3		m_Rot;				//	rotation of this control
		math::Vector3		m_PosOffset;		//	rotation offset of this control
		math::Vector3		m_RotOffset;		//	rotation offset of this control
		math::Vector3		m_SclOffset;		//	scale offset of this control
		math::Matrix		m_Mtrx;				//  matrix transformation of this control

		void*				m_UserData;			//  user data can be anything
		DWORD				m_UserTag;			//  user tag can be useful
		PControl			m_Parent;			//  parent of this control

		int					m_LastMouseMessage;	//	last message of the cursor checker

		GUICallbackFuncPtr		m_OnClick;		//  callback function for mouse click
		GUICallbackFuncPtr		m_OnEnter;		//  callback function for mouse enter
		GUICallbackFuncPtr		m_OnExit;		//  callback function for mouse leave
		GUICallbackFuncPtr		m_OnMove;		//  callback function for mouse move
		GUICallbackFuncPtr		m_OnWheel;		//  callback function for mouse wheel
		GUICallbackFuncPtr		m_OnKeyDown;	//  call back function for key down

		Array<PControl>	m_Child;	//  array of childes

	protected:
		//  additional functions to better control of input
		virtual void processInputMouse(bool& result, int playerID);
		virtual void processInputKeyboard(bool& result, int playerID);

	};



}}	//	namespace sx { namespace gui {


#endif	//	GUARD_sxControl_HEADER_FILE