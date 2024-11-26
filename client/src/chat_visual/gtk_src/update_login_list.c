#include "GTK.h"

void update_login_list(GtkListBox *login_list, cJSON *parsed_json) {
    if (!login_list || !parsed_json) {
        fprintf(stderr, "Error: Invalid login_list or parsed_json\n");
        return;
    }

    // Clear existing list items
    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(login_list));
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(login_list, child);
        child = next;
    }

    // Get the users array
    cJSON *users = cJSON_GetObjectItem(parsed_json, "users");
    if (!cJSON_IsArray(users)) {
        fprintf(stderr, "Error: Users list is not an array.\n");
        return;
    }

    // Iterate over the users and add them to the login list
    cJSON *user = NULL;
    int count = 0;
    cJSON_ArrayForEach(user, users) {
        cJSON *active = cJSON_GetObjectItem(user, "active");
        cJSON *nickname = cJSON_GetObjectItem(user, "nickname");
        cJSON *user_id = cJSON_GetObjectItem(user, "id");

        // Skip invalid or inactive users and FirstAdmin (ID 1)
        if (!cJSON_IsBool(active) || !cJSON_IsTrue(active) || !cJSON_IsString(nickname) || (cJSON_IsNumber(user_id) && user_id->valueint == 1)) {
            continue;
        }

        // Create a new row for the login list
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(nickname->valuestring);
        gtk_widget_set_halign(label, GTK_ALIGN_START);  // Align text to the left
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);  // Set label directly as child
        gtk_list_box_append(login_list, row);
        count++;
    }

    if (count == 0) {
        fprintf(stderr, "Warning: No active users were added to the login list.\n");
    } else {
        printf("Added %d users to the login list\n", count);
    }

    // Ensure the list box is visible
    gtk_widget_set_visible(GTK_WIDGET(login_list), TRUE);
}
