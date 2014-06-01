#define HASH_VERIFY_RES1(systime)	(systime / 8 + systime / 5)
#define HASH_VERIFY_RES2(systime)	(systime / 9 + systime / 6)
#define HASH_VERIFY_RES3(systime)	((systime / 8 + systime / 5) * (systime / 9 + systime / 6))


int show_hash_lock( HINSTANCE instance, wchar_t* dest );
unsigned int verify_hash_lock_code ( const unsigned systime, const wchar_t * code, int* res );
void show_steam( HINSTANCE instance );