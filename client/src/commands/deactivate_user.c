#include "cJSON.h"
#include "commands.h"
#include "client.h"

void deactivate_user(SSL *ssl, int user_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", DEACTIVATE_USER); 
    cJSON_AddNumberToObject(json, "user_id", user_id);

    send_and_delete_json(ssl, &json);
}