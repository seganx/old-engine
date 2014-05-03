#include <Windows.h>
#include <stdio.h>

#include "../sxLib/Lib.h"


void encrypt_file( const char* filename )
{
	printf( "encrypting %s ", filename );

	FILE* file = 0;
	if ( fopen_s( &file, filename, "rb" ) )
	{
		printf( "can't open file.\n" );
		return;
	}

	//	read size
	fseek( file, 0, SEEK_END );
	uint filesize = ftell( file );
	fseek( file, 0, SEEK_SET );

	//	create buffer
	char* buffer = (char*)sx_mem_alloc( filesize );
	fread_s( buffer, filesize, 1, filesize, file );
	fclose( file );
	
	if ( buffer[0] != 'e' || buffer[1] != 'n' )
	{
		sx_encrypt( buffer, buffer, filesize, 12345 );

		if ( fopen_s( &file, filename, "wb" ) == 0 )
		{
			char tmp[2] = { 'e', 'n' };
			fwrite( tmp, 1, 2, file );
			fwrite( buffer, 1, filesize, file );
			fclose( file );
			printf( "done.\n" );
		}
		else printf( "can't create file.\n" );
	}
	else printf( "already encrypted.\n" );
	sx_mem_free( buffer );
}

void decrypt_file( const char* filename )
{
	printf( "decrypting %s ", filename );

	FILE* file = 0;
	if ( fopen_s( &file, filename, "rb" ) )
	{
		printf( "can't open file.\n" );
		return;
	}

	//	read size
	fseek( file, 0, SEEK_END );
	uint filesize = ftell( file );
	fseek( file, 0, SEEK_SET );

	char tmp[2] = {0};
	fread_s( tmp, 2, 2, 1, file );
	if ( tmp[0] != 'e' || tmp[1] != 'n' )
	{
		printf( "is not encrypted.\n" );
		return;
	}
	filesize -= 2;

	//	create buffer
	char* buffer = (char*)sx_mem_alloc( filesize );
	fread_s( buffer, filesize, 1, filesize, file );
	fclose( file );

	//	decrypt file
	sx_decrypt( buffer, buffer, filesize, 12345 );

	if ( fopen_s( &file, filename, "wb" ) == 0 )
	{
		fwrite( buffer, 1, filesize, file );
		fclose( file );
		printf( "done.\n" );
	}
	else printf( "can't create file.\n" );
	
	sx_mem_free( buffer );
}

int main( int argc, char* argv[] )
{
	if ( argc > 2 )
	{
		if ( !strcmp( argv[1], "en" ) )
			encrypt_file( argv[2] );
		else if ( !strcmp( argv[1], "de" ) )
			decrypt_file( argv[2] );
	}

	return 0;
}