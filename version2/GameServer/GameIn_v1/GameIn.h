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

typedef enum gamein_error
{
    GIE_NO_ERROR = 0,
    GIE_INVALID_DATA = 1,
    GIE_INVALID_USERPASS = 2
}
gamein_error;


typedef struct gamein
{
    char                         trace_file[32];
    char                         http_port[8];
    uint                         threads_count;
    uint                         request_count;
    struct sx_threadpool *       threadpool;
    struct sx_database_config *  database_config;
}
gamein;

extern struct gamein g_gamein;

//! send data to the client and close the connection
void send_and_close(struct mg_connection *nc, const void* data, const int lenght);

//! implement this function in your own application to handle requests
void gamein_handle_request(struct mg_connection *nc, struct http_message *hm);

//! implement this function in your own application to handle pool
void gamein_handle_pool(struct mg_connection *nc);

#endif // HEADER_GAMEIN