#include "GTK.h"

user_list_t* create_user_list(void) {
    user_list_t *list = malloc(sizeof(user_list_t));
    list->capacity = 10;
    list->count = 0;
    list->users = malloc(sizeof(user_data_t*) * list->capacity);
    return list;
}

void create_user(user_list_t *list,  char *username, int id, bool is_online, char *last_message,  char *time) {

    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->users = realloc(list->users, sizeof(user_list_t*) * list->capacity);
    }
    
    user_data_t *user = malloc(sizeof(user_data_t));
    user->username = mx_strdup(username);
    user->id = id;
    user->is_online = is_online;
    user->last_message = mx_strdup(last_message ? last_message : "NULL");
    user->last_time = mx_strdup(time ? time : "NULL");
    
    list->users[list->count++] = user;
}

