#ifndef HEADER_GAMEIN
#define HEADER_GAMEIN

#include "../../sxLib/Def.h"
#include "../../sxLib/Memory.h"
#include "../../sxLib/Trace.h"
#include "../../sxLib/Platform.h"
#include "../../sxLib/Time.h"
#include "../../sxLib/Math.h"
#include "../../sxLib/String.h"
#include "../../sxLib/Json.h"

#define MG_ENABLE_MQTT 0
#define MG_ENABLE_MQTT_BROKER 0
#define MG_ENABLE_COAP 0
#define MG_ENABLE_HTTP_CGI 0
#define MG_ENABLE_HTTP_SSI 0
#define MG_ENABLE_HTTP_SSI_EXEC 0
#define MG_ENABLE_HTTP_WEBDAV 0
#define MG_ENABLE_HTTP_WEBSOCKET 0
#define MG_ENABLE_THREADS 0
#define MG_DISABLE_HTTP_DIGEST_AUTH 0
#define MG_ENABLE_FILESYSTEM 0

#include "mongoose.h"

#define gamein_key_len  32

typedef enum gamein_error
{
    GIE_NO_ERROR = 0,
    GIE_INVALID_DATA = 1,
    GIE_INVALID_USERPASS = 2
}
gamein_error;

typedef struct gamein_header
{
    gamein_error    error;
    char            user_data[64];
    char            access_code[96];
}
gamein_header;

typedef struct gamein_access_token
{
    byte            version;                            //  version of the object
    uint            game_id;                            //  unique id of the game used to access from database
    uint            profile_id;                         //  unique id of the player used to access from database
    char            local_key[gamein_key_len];          //  local key
}
gamein_access_token;

typedef struct gamein
{
    char                         trace_file[32];        //  filename of generated crash/memory reports
    char                         http_port[8];          //  port number
    uint                         threads_count;         //  number of threads in threadpool
    uint                         request_count;
    struct sx_threadpool *       threadpool;
    struct sx_database_config *  database_config;
}
gamein;
extern struct gamein g_gamein;  //  global pointer to the gamein instance


//////////////////////////////////////////////////////////////////////
//  INTERFACE

//! implement this function in your own application to handle requests
void gamein_handle_request(struct mg_connection *nc, struct http_message *hm);

//! implement this function in your own application to handle connection pool
void gamein_handle_pool(struct mg_connection *nc);


//////////////////////////////////////////////////////////////////////
//  HELPER FUNCTIONS

//! write header to the destination buffer and return the lenght of access code in the header
uint gamein_header_read(struct gamein_header *dest, struct http_message *hm);

//! extract access token object from string and return the pointer the destination buffer
struct gamein_access_token * gamein_access_token_read(struct gamein_access_token *dest, const char* data, const uint size);

//! send data to the client and close the connection
int gamein_send_and_close(struct mg_connection *nc, const gamein_header *header, const void *data, const int lenght);



#endif // HEADER_GAMEIN