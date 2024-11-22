#include "GTK.h"

void update_login_list(GtkListBox *login_list, cJSON *parsed_json) {
    // Clear existing list items
    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(login_list));
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(login_list), child);
        child = next;
    }

    // Request all users from the server
    // get_all_clients_userslist(ssl);

    // Simulated response for example purposes
    //const char *response = "{\"success\":true,\"number_of_users\":1,\"users\":[{\"id\":1,\"active\":true,\"unread_messages\":-1,\"nickname\":\"FirstAdmin\",\"online\":false}],\"command_code\":17}";

    // // Parse the JSON response
    // cJSON *json = cJSON_Parse(response);
    // if (json == NULL) {
    //     fprintf(stderr, "Error: Failed to parse JSON response.\n");
    //     return;
    // }

    // // Check if the response indicates success
    // cJSON *success = cJSON_GetObjectItem(json, "success");
    // if (!cJSON_IsBool(success) || !cJSON_IsTrue(success)) {
    //     cJSON_Delete(json);
    //     fprintf(stderr, "Error: Server response indicates failure.\n");
    //     return;
    // }

    // Get the users array
    cJSON *users = cJSON_GetObjectItem(parsed_json, "users");
    if (!cJSON_IsArray(users)) {
        cJSON_Delete(parsed_json);
        fprintf(stderr, "Error: Users list is not an array.\n");
        return;
    }

    // Iterate over the users and add them to the login list
    cJSON *user = NULL;
    cJSON_ArrayForEach(user, users) {
        cJSON *active = cJSON_GetObjectItem(user, "active");
        cJSON *nickname = cJSON_GetObjectItem(user, "nickname");

        if (cJSON_IsBool(active) && cJSON_IsTrue(active) && cJSON_IsString(nickname)) {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new(nickname->valuestring);
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
            gtk_list_box_append(login_list, row);
        }
    }

    cJSON_Delete(parsed_json);
}
