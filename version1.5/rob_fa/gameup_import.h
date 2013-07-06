/********************************************************************
	created:	2013/06/23
	filename: 	gameup_import.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic class to import GameUp DLL
*********************************************************************/
#ifndef GUARD_gameup_import_HEADER_FILE
#define GUARD_gameup_import_HEADER_FILE

#define GAME_SCORE_LEVEL	1	//	getting score because of reason one
#define GAME_SCORE_BOSS		2	//	getting score because of reason two
#define GAME_SCORE_ACHIV	3	//	getting score because of reason three

#define GAME_SCORE_EASY		1	//	getting score in easy mode
#define GAME_SCORE_NORM		2	//	getting score in normal mode
#define GAME_SCORE_HARD		3	//	getting score in hard mode

struct GameUp
{
	/*! return the lock number by specified index */
	int ( __cdecl * get_lock_code )( const int index );

	/*! start to add game score */
	int ( __cdecl * begin_score )( void );

	/*! add new score to current score */
	int ( __cdecl * add_score )( const int reason, const int mode );

	/*! update current score to the profile. return -1 if function failed */
	int ( __cdecl * end_score )( void );

	/*! set a new value to the info by given index.
		NOTE: index must be between 0..250
		NOTE: value will clamp between 0..200
		return -1 if function failed */
	int ( __cdecl * set_info )( const int index, int val );

	/*! get the value from the info by given index.
		NOTE: index must be between 0..250
		return -1 if function failed */
	int ( __cdecl * get_info )( const int index );

	/*! set a new value to the user data by given index.
		NOTE: index must be between 0..500
		return -1 if function failed */
	int ( __cdecl * set_data_i )( const int index, int val );

	/*! get the value from the user data by given index.
		NOTE: index must be between 0..500 */
	int ( __cdecl * get_data_i )( const int index );

	/*! set a new value to the user data with 3 real part by given index.
		NOTE: index must be between 0..500
		return -1 if function failed */
	int ( __cdecl * set_data_f )( const int index, float val );

	/*! get the value with 3 real part from the user data by given index.
		NOTE: index must be between 0..500 */
	float ( __cdecl * get_data_f )( const int index );

	void *dllmodul;
};


int gameup_init( GameUp* gameup );

void gameup_finit( GameUp* gameup );


#endif	//	GUARD_gameup_import_HEADER_FILE