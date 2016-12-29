#include "authenticator.h"

#include "../plugin.h"
#include "../../request.h"
#include "../../imports.h"

#define session_size	sizeof(AuthenSession)

static AuthenConfig s_config;	//	authentication configuration

int authen_handle_request(Request* req, const uint user_data)
{
	char rcvdkey[diffie_hellman_l] = { 0 };
	if (sx_str_get_value(rcvdkey, diffie_hellman_l, req->data, "public_key") == false)
		return 1;

	// validate received key
	{
		const int hirange = 65 + diffie_hellman_p;
		for (int i = 0; i < diffie_hellman_l; ++i)
			if (sx_between_i(rcvdkey[i], 65, hirange) == false)
				return 1;
	}

	//////////////////////////////////////////////////////////////////////////
	// setup Diffie-Hellman keys to start communication
	//////////////////////////////////////////////////////////////////////////
	sx_randomize((uint)sx_time_counter());

	// generate secret key
	char secret_key[diffie_hellman_l] = { 0 };
	sx_dh_secret_Key(secret_key, diffie_hellman_l);

	// generate public key based on secret key
	char public_key[diffie_hellman_l] = { 0 };
	sx_dh_public_key(public_key, secret_key, diffie_hellman_l, diffie_hellman_g, diffie_hellman_p);

	char final_key[diffie_hellman_l] = { 0 };
	sx_dh_final_key(final_key, secret_key, rcvdkey, diffie_hellman_l, diffie_hellman_p);

	// create a session for the request
	AuthenSession as;
	as.access_key = sx_checksum(final_key, diffie_hellman_l);
	as.birth_time = sx_time_seconds();
	as.time_out = s_config.time_out;

	// encrypt the session
	byte enas[session_size] = {0};
	sx_encrypt(enas, &as, session_size, session_cryptokey);

	// create session token as string
	char stoken[session_size * 2] = {0};
	sx_base64_encode( stoken, sizeof(stoken), enas, session_size );

	// create signature for token
	char signature[33] = {0};
	sx_md5( signature, stoken, session_secret, null );

	//	send token to the client
	char msg[128] = { 0 };
	int len = sprintf_s(msg, 128, "{\"user_data\":%u,\"token\":\"%s.%s\",\"key\":\"%.*s\"}", user_data, stoken, signature, diffie_hellman_l, public_key);
	request_send(req, msg, len);

	return 1;
}

int authen_handle_access(Request* req)
{
	//	basic verification of the request
	int req_len = sx_str_len(&req->uri[1]);
	if ( !sx_between_i( req_len, 33 + session_size, 33 + session_size * 2 ) ) return 0;

	//	extract data
	char token[session_size * 2] = {0};
	const char* signature = sx_str_get_token( token, sizeof(token), &req->uri[1], '.' );
	if ( signature == null || *signature == 0 ) return 0;
	else signature++;
	
	//	check signature of the request
	char curr_signature[33] = { 0 };
	sx_md5(curr_signature, token, session_secret, null);
	if ( sx_str_cmp(curr_signature, signature) ) return 0;

	//	decode session data
	char stoken[session_size * 2] = {0};
	if ( sx_base64_decode(stoken, session_size, token) == 0 ) return 0;

	//	decrypt session to structure
	AuthenSession as;
	sx_decrypt(&as, stoken, session_size, session_cryptokey);
	if ( as.time_out > s_config.time_out ) return 0;

	printf("Key: %12u Birth: %u Time: %u\n", as.access_key, as.birth_time, as.time_out);

	return 0;
}


void authen_load_config(const char* fileName)
{
	s_config.time_out = 30;
}

