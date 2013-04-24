/********************************************************************
created:	2011/03/08
filename: 	sxSettings.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain some internal settings of the engine, game, editor ...
*********************************************************************/
#ifndef GUARD_sxSettings_HEADER_FILE
#define GUARD_sxSettings_HEADER_FILE

#include "sxCore_def.h"


namespace sx { namespace core {

	/*!
	this static class contain all internal settings of the engine, some setting of 
	the game and editor and etc...
	this class do NOT save any settings anywhere and the application is responsible 
	to save each necessary settings !
	*/
	class SEGAN_API Settings
	{
	public:

		//! contain the option level and permission types
		class SEGAN_API Option
		{
		public:
			Option(void);

			//! return true if specified option is okay
			bool HasPermission(void);

			//! add a permission type to option
			void AddPermission(OptionPermissionType OPT_ who);

			//! remove a permission type from option
			void RemPermission(OptionPermissionType OPT_ who);

			//! return the level of option 0=High 1=medium 2=low
			int GetLevel(void);

			//! set a new level for this option 0=High 1=medium 2=low
			void SetLevel(int level);

		public:

			DWORD value;
		};
		typedef Option *POption;

		//! initialize settings of engine. this called by engine automatically
		static void Initialize(void);

		//! return the option for texture level
		static POption	GetOption_TextureLevel(void);

		//! return the option for shadow level
		static POption	GetOption_Shadow(void);

		//! return the option for reflection
		static POption	GetOption_Reflection(void);

		//! return current sun light theta and phi
		static float2 GetSunLightPosition(void);

		//! set current sun light theta and phi
		static void SetSunLightPosition(float theta, float phi);

		//! return reference to sun light diffuse. use w as intensity
		static float4& SunLightColor(void);

		//! return reference to sun light ambient. use w as intensity
		static float4& AmbientColor(void);

	};

}} // namespace sx { namespace core {


#endif	//	GUARD_sxSettings_HEADER_FILE