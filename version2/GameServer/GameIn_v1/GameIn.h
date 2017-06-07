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

void send_to_client(struct mg_connection *nc, const void* data, const int lenght);

#endif // HEADER_GAMEIN