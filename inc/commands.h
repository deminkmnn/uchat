#pragma once

#include <openssl/ssl.h>
#include "command_codes.h"




void send_to_chat(SSL *ssl, int chat_id, char *message);
void send_to_user(SSL *ssl, int reciever_id, char *message);
void see_all_chats(SSL *ssl);
void see_all_users(SSL *ssl);
void create_chat(SSL *ssl, char *new_chat_name);
void send_exit_command(SSL *ssl);
void add_contact_to_chat(SSL *ssl, int contact_id, int chat_id);
void join_chat(SSL *ssl, int chat_id);
void see_my_contacts(SSL *ssl);
void add_contact(SSL *ssl, int new_contact_id);
void get_my_contacts(SSL *ssl);
void get_all_talks(SSL *ssl);


void send_and_delete_json(SSL *ssl, cJSON **json);

