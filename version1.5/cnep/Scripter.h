/********************************************************************
	created:	2011/07/18
	filename: 	Scripter.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain an static class of scripter who load
				all game parameters from some different text files
*********************************************************************/
#ifndef GUARD_Scripter_HEADER_FILE
#define GUARD_Scripter_HEADER_FILE

#include "ImportEngine.h"

class Scripter
{
	SEGAN_STERILE_CLASS(Scripter);

public:
	Scripter(void);
	~Scripter(void);

	//! clear all current data
	void Clear(void);

	//! this will load an script file.
	void Load(const WCHAR* scriptFile);

	//! return number of sections
	int GetObjectCount(void);

	//! return true if named parameter founded and fill value by parameter
	bool GetFloat(const int objectIndex, const WCHAR* name, float& value);

	//! return true if named parameter founded and fill value by parameter
	bool GetInteger(const int objectIndex, const WCHAR* name, int& value);

	//! return true if named parameter founded and fill value by parameter
	bool GetString(const int objectIndex, const WCHAR* name, str512& value);

private:
	int		m_SectionCount;			//  number of sections in script file

	Map<UINT, float>		m_mapValue;
	Map<UINT, str512>		m_mapString;
};

#endif	//	GUARD_Scripter_HEADER_FILE