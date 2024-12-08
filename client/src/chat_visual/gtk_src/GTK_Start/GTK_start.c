#include "GTK.h"

bool switch_to_groups;
bool switch_settings_in_group;

static void on_settings_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    if (!GTK_data) return;
    
    // Get the current window
    GtkWidget *current_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
    if (!current_window) return;
    
    // Hide the current window
    //gtk_widget_set_visible(current_window, true);


    // Start profile form
    start_login(GTK_data->call_data);
    //profile_start(GTK_data);
    
    //apply_css("src/chat_visual/gtk_src/GTK_Start/style.css");
    // Show the main window again
    //gtk_window_present(GTK_WINDOW(current_window));
}

void show_all_contacts(GTK_data_t *GTK_data, gboolean is_group) {
    if (!GTK_data) {
        printf("GTK_data is NULL.\n");
        return;
    }
    
    chat_manager_t *manager = is_group ? GTK_data->group_manager : GTK_data->chat_manager;
    
    if (!manager) {
        printf("%s manager is NULL.\n", is_group ? "Group" : "Chat");
        return;
    }
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, manager->chats);
   
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        chat_data_t *chat_data = (chat_data_t *)value;
        if (!chat_data->is_show) {
            chat_data->is_show = true;
            gtk_widget_set_visible(chat_data->button, TRUE);
        }
    }
}

// Function to print chat manager information
void print_chat_manager_info(GTK_data_t *GTK_data, const char *search_enter) {
    if (!GTK_data) {
        printf("GTK_data is NULL.\n");
        return;
    }
    
    // Process chat_manager
    if (GTK_data->chat_manager) {
        chat_manager_t *chat_manager = GTK_data->chat_manager;
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, chat_manager->chats);
        
        int search_length = strlen(search_enter);
        if (search_length > 0) {
            while (g_hash_table_iter_next(&iter, &key, &value)) {
                chat_data_t *chat_data = (chat_data_t *)value;
                if (mx_strstr(chat_data->contact_name, search_enter) != 0) {
                    chat_data->is_show = true;
                    gtk_widget_set_visible(chat_data->button, TRUE);
                } else {
                    chat_data->is_show = false;
                    gtk_widget_set_visible(chat_data->button, FALSE);
                }
            }
        } else {
            show_all_contacts(GTK_data, FALSE);
        }
    }
    
    // Process group_manager
    if (GTK_data->group_manager) {
        chat_manager_t *group_manager = GTK_data->group_manager;
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, group_manager->chats);
        
        int search_length = strlen(search_enter);
        if (search_length > 0) {
            while (g_hash_table_iter_next(&iter, &key, &value)) {
                chat_data_t *chat_data = (chat_data_t *)value;
                if (mx_strstr(chat_data->contact_name, search_enter) != 0) {
                    chat_data->is_show = true;
                    gtk_widget_set_visible(chat_data->button, TRUE);
                } else {
                    chat_data->is_show = false;
                    gtk_widget_set_visible(chat_data->button, FALSE);
                }
            }
        } else {
            show_all_contacts(GTK_data, TRUE);
        }
    }
}

// Signal handler for search bar changes
void on_search_bar_changed(GtkEntry *entry, gpointer user_data) {
    (void)entry;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;
    const char *enter = gtk_editable_get_text(GTK_EDITABLE(entry));
    print_chat_manager_info(GTK_data, enter);
}

void on_erase_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;
    // Clear the search bar text
    if (GTK_data->search_bar) {
        gtk_editable_set_text(GTK_EDITABLE(GTK_data->search_bar), ""); // Clear the search bar
    }
    show_all_contacts(GTK_data, true);
    show_all_contacts(GTK_data, false);
}

void switch_between_groups_chats(GtkWidget *widget, gpointer user_data) {
    (void) widget; // if you erase this, GTK_data will be empty
    GtkWidget **entries = (GtkWidget **)user_data;
    GTK_data_t *GTK_data = (GTK_data_t *)entries[0];
    GtkWidget *add_group_button = entries[1];
    GtkWidget *sidebar_scroll_groups = entries[2];
    GtkWidget *sidebar_scroll_users = entries[3];

    if (!GTK_data) {
        printf("GTK_data is NULL\n");
        return;
    }

    // chat_manager_t *chat_manager = GTK_data->chat_manager;
    // chat_manager_t *group_manager = GTK_data->group_manager;

    // if (!chat_manager) {
    //     printf("chat_manager is NULL\n");
    //     return;
    // }

    // Toggle visibility
    switch_to_groups = !switch_to_groups;

    if (switch_to_groups) {
        gtk_widget_set_visible(sidebar_scroll_users, FALSE);
        gtk_widget_set_visible(sidebar_scroll_groups, TRUE);
        gtk_label_set_text(GTK_LABEL(entries[4]), "Groups");

        gtk_widget_set_visible(add_group_button, TRUE);
    } else {
        gtk_widget_set_visible(sidebar_scroll_users, TRUE);
        gtk_widget_set_visible(sidebar_scroll_groups, FALSE);
        gtk_label_set_text(GTK_LABEL(entries[4]), "Chats");

        gtk_widget_set_visible(add_group_button, FALSE);
    }

}

static void on_window_destroy(GtkWindow *window, gpointer user_data) {
    //(void)window;
    GtkWidget **entries = (GtkWidget **)user_data;
    GTK_data_t *GTK_data = (GTK_data_t *)entries[0];

    GList *user_list = g_hash_table_get_keys(GTK_data->group_manager->chats);
    GList *iter;

    for (iter = user_list; iter != NULL; iter = iter->next) {
        gpointer key = iter->data;
        chat_data_t *user_chat = g_hash_table_lookup(GTK_data->group_manager->chats, key);
        g_object_unref(user_chat->message_entry);
        g_object_unref(user_chat->user_list_for_add);
        g_object_unref(user_chat->user_list_for_delete);
    }

    user_list = g_hash_table_get_keys(GTK_data->chat_manager->chats);

    for (iter = user_list; iter != NULL; iter = iter->next) {
        gpointer key = iter->data;
        chat_data_t *user_chat = g_hash_table_lookup(GTK_data->chat_manager->chats, key);
        g_object_unref(user_chat->message_entry);

    }
    g_list_free(user_list);
    
    send_exit_command(GTK_data->call_data->ssl);
    *(GTK_data->call_data->stop_flag) = true;
    free(GTK_data); //Потрібно продивитися яку виділену пам'ять мы тримаемо у GTK_data і теж її очистити!!!!!!!
    GTK_data = NULL;
    g_free(entries); 
    printf("\nclosing the window\n");
    gtk_window_destroy(window);
}

static void on_create_group_clicked(GtkWidget *button, gpointer user_data) {
    (void) button;
    GtkWidget **entries = (GtkWidget **)user_data;
    GtkWidget *dialog = entries[0];
    GtkWidget *entry = entries[1];
    GTK_data_t *GTK_data = (GTK_data_t *)entries[2];
    if (!GTK_data) {
        printf("GTK_data is NULL\n");
        return;
    }
    
    // Get the group name
    const char *group_name = gtk_editable_get_text(GTK_EDITABLE(entry));

    // Debug print
    // printf("Debug: Entry text retrieved = '%s'\n", group_name ? group_name : "NULL");
    
    // Print the group name to console
    if (group_name && mx_strlen(group_name) > 0) {
        // printf("New group created: %s\n", group_name);
        create_chat(GTK_data->call_data->ssl, (char *)group_name);
    } else {
        printf("Group name cannot be empty\n");
    }
    
    // Free the allocated memory
    g_free(entries);
    // Close the dialog - FIXED: Cast to GtkWindow*
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void on_add_group_button_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    GTK_data_t *GTK_data = (GTK_data_t *)data;

    // Create a new dialog window
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Create New Group");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);

    // Make dialog a transient window of the main window, but still allow independent movement
    if (GTK_data && GTK_data->window) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(GTK_data->window));
        gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE); // Ensure it doesn't move the main window
    }

    // Allow the dialog to be moved independently
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE); // Not modal

    // Create a box to hold the content
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    // Create a label
    GtkWidget *label = gtk_label_new("Enter Group Name:");
    gtk_widget_add_css_class(label, "group-setting-label");
    gtk_box_append(GTK_BOX(box), label);

    // Create an entry for group name
    GtkWidget *group_name_entry = gtk_entry_new();
    gtk_widget_add_css_class(group_name_entry, "group-name-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(group_name_entry), "Group name");
    gtk_box_append(GTK_BOX(box), group_name_entry);

    // Create buttons box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);

    // Create buttons
    GtkWidget *create_button = gtk_button_new_with_label("Create");
    GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");

    gtk_box_append(GTK_BOX(button_box), cancel_button);
    gtk_widget_add_css_class(create_button, "group-create-button");
    gtk_box_append(GTK_BOX(button_box), create_button);
    gtk_widget_add_css_class(cancel_button, "group-cancel-button");
    gtk_box_append(GTK_BOX(box), button_box);

    // Set the box as the child of the dialog
    gtk_window_set_child(GTK_WINDOW(dialog), box);

    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 3);
    entries[0] = dialog;
    entries[1] = group_name_entry;
    entries[2] = data;

    // Connect button signals
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_group_clicked), entries);

    // Present the dialog
    gtk_window_present(GTK_WINDOW(dialog));
}

void switch_between_settings_in_group(GtkWidget *widget, gpointer user_data) {
    (void) widget; // if you erase this, GTK_data will be empty
    GtkWidget **entries = (GtkWidget **)user_data;
    GTK_data_t *GTK_data = (GTK_data_t *)entries[0];
    GtkWidget *slider_inner_container_1 = entries[1];
    GtkWidget *slider_inner_container_2 = entries[2];

    if (!GTK_data) {
        printf("GTK_data is NULL\n");
        return;
    }

    // Toggle visibility
    switch_settings_in_group = !switch_settings_in_group;

    if (switch_settings_in_group) {
        gtk_widget_set_visible(slider_inner_container_1, FALSE);
        gtk_widget_set_visible(slider_inner_container_2, TRUE);
        gtk_label_set_text(GTK_LABEL(entries[3]), "Delete users");
    } else {
        gtk_widget_set_visible(slider_inner_container_1, TRUE);
        gtk_widget_set_visible(slider_inner_container_2, FALSE);
        gtk_label_set_text(GTK_LABEL(entries[3]), "Add users");
    }

}

static void delete_name_from_list(GtkWidget *button, gpointer user_data) {
    // Get the entry widget from which we'll read the name to delete
    GtkWidget *entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "entry"));

    GTK_data_t *GTK_data = (GTK_data_t*)user_data;

    GtkWidget *list_box = GTK_data->group_manager->user_list_for_delete;
   
    // Get the text from the entry
    const char *name_to_delete = gtk_editable_get_text(GTK_EDITABLE(entry));
    printf("name_to_delete: %s\n", name_to_delete);
   
    // Only proceed if name is not empty
    if (strlen(name_to_delete) > 0) {
        // Get the first child of the list box
        GtkWidget *child = gtk_widget_get_first_child(list_box);
        gtk_widget_add_css_class(list_box, "settings_list_box");
       
        // Iterate through all children
        while (child != NULL) {
            GtkWidget *label = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child));
            const char *current_name = gtk_label_get_text(GTK_LABEL(label));                   
            if (strcmp(current_name, name_to_delete) == 0) {
                delete_user_from_group(GTK_data->call_data->ssl, GPOINTER_TO_INT(g_object_get_data(G_OBJECT(label), "user_id")), GPOINTER_TO_INT(g_object_get_data(G_OBJECT(list_box), "chat_id")));
    
                gtk_editable_set_text(GTK_EDITABLE(entry), "");
                break;
            }
            child = gtk_widget_get_next_sibling(child);
        }
    }
}

static void add_name_to_list(GtkWidget *button, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "entry"));
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    GtkWidget *list_box = GTK_data->group_manager->user_list_for_add;
    
    // Get the text from the entry
    const char *name = gtk_editable_get_text(GTK_EDITABLE(entry));
    
    // Only add non-empty names
    if (strlen(name) > 0) {
        GtkWidget *child = gtk_widget_get_first_child(list_box);
        // Iterate through all children
        while (child != NULL) {
            GtkWidget *label = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child));
            const char *current_name = gtk_label_get_text(GTK_LABEL(label));                               
            if (strcmp(current_name, name) == 0) {
                int user_ids[] = {GPOINTER_TO_INT(g_object_get_data(G_OBJECT(label), "user_id"))};
                add_many_users_to_group(GTK_data->call_data->ssl, user_ids, 1, GPOINTER_TO_INT(g_object_get_data(G_OBJECT(list_box), "chat_id")));

                gtk_editable_set_text(GTK_EDITABLE(entry), "");
                break;
            }
            child = gtk_widget_get_next_sibling(child);
        }
    }
}

void group_delete(GtkWidget *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    int chat_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(GTK_data->group_manager->user_list_for_add), "chat_id"));
    delete_group(GTK_data->call_data->ssl, chat_id);
}

static void on_group_user_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    (void)list_box;
    GtkEntry *entry = GTK_ENTRY(user_data);

    if (row) {
        GtkWidget *child = gtk_list_box_row_get_child(row);
        if (GTK_IS_LABEL(child)) {
            const char *user_text = gtk_label_get_text(GTK_LABEL(child));
            gtk_editable_set_text(GTK_EDITABLE(entry), user_text);
        }
    }
}

void on_settings_group_button_clicked(GtkWidget *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;

    // Create a new window
    GtkWidget *settings_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(settings_window), "Settings");
    gtk_window_set_default_size(GTK_WINDOW(settings_window), 400, 300);

    // Set the new window as transient for the main window
    if (GTK_data && GTK_data->window) {
        gtk_window_set_transient_for(GTK_WINDOW(settings_window), GTK_WINDOW(GTK_data->window));
    }

    // Create a grid to organize the layout
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_margin_start(grid, 10); // Optional: set margins
    gtk_widget_set_margin_end(grid, 10);
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);
    gtk_window_set_child(GTK_WINDOW(settings_window), grid); // Use gtk_window_set_child instead of gtk_container_add

    // Set the grid to be homogeneous
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE); // Set to FALSE to allow different row heights
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    // Create main content area (for example, a label or a text entry)
    // GtkWidget *main_content = gtk_label_new("Main Content Goes Here");
    GtkWidget *main_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(grid), main_grid, 0, 0, 1, 1); // Attach to grid at (0, 0)
    gtk_widget_set_hexpand(main_grid, TRUE); // Allow horizontal expansion
    gtk_widget_set_vexpand(main_grid, TRUE); // Allow vertical expansion

    GtkWidget *page_controller_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(page_controller_container, "settings_page_controller");
    gtk_grid_attach(GTK_GRID(main_grid), page_controller_container, 0, 2, 1, 1);
    gtk_widget_set_hexpand(page_controller_container, TRUE); // Allow horizontal expansion
    gtk_widget_set_vexpand(page_controller_container, FALSE); // Disable vertical expansion
    gtk_box_set_homogeneous(GTK_BOX(page_controller_container), TRUE); // Make all children the same size

    // Page controller container
    gtk_widget_set_margin_top(page_controller_container, 30);
    gtk_widget_set_margin_bottom(page_controller_container, 30);
    gtk_widget_set_margin_start(page_controller_container, 30);
    gtk_widget_set_margin_end(page_controller_container, 30);


    // Left button
    GtkWidget *left_button = gtk_button_new();
    gtk_widget_add_css_class(left_button, "left-button");
    gtk_box_append(GTK_BOX(page_controller_container), left_button);
    GtkWidget *back_icon = gtk_image_new_from_file("src/chat_visual/images/back.svg");
    gtk_button_set_child(GTK_BUTTON(left_button), back_icon);

    // Center label
    GtkWidget *center_label = gtk_label_new("Add users");
    gtk_widget_add_css_class(center_label, "center-label");
    gtk_widget_set_size_request(center_label, 150, -1);
    gtk_label_set_ellipsize(GTK_LABEL(center_label), PANGO_ELLIPSIZE_END); // Handle overflow gracefully
    gtk_box_append(GTK_BOX(page_controller_container), center_label);

    // Right button
    GtkWidget *right_button = gtk_button_new();
    gtk_widget_add_css_class(right_button, "right-button");
    GtkWidget *next_icon = gtk_image_new_from_file("src/chat_visual/images/forward_arrow.svg");
    gtk_button_set_child(GTK_BUTTON(right_button), next_icon);
    gtk_box_append(GTK_BOX(page_controller_container), right_button);
    
    // Slider container
    GtkWidget *slider_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(slider_container, "settings_slider-container");
    gtk_grid_attach(GTK_GRID(main_grid), slider_container, 0, 3, 1, 1);
    gtk_widget_set_hexpand(slider_container, TRUE);
    gtk_widget_set_vexpand(slider_container, TRUE);

    // Slider container
    gtk_widget_set_margin_top(slider_container, 30);
    gtk_widget_set_margin_bottom(slider_container, 30);
    gtk_widget_set_margin_start(slider_container, 30);
    gtk_widget_set_margin_end(slider_container, 30);

    GtkWidget *slider_inner_container_1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append(GTK_BOX(slider_container), slider_inner_container_1);

    // Top container with entry and add button
    GtkWidget *entry_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(entry_container, "settings_entry-container");
    gtk_widget_set_hexpand(entry_container, TRUE); // Allow horizontal expansion
    
    // Entry field
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter user name");
    gtk_widget_add_css_class(name_entry, "settings_name-entry");
    gtk_box_append(GTK_BOX(entry_container), name_entry);
    gtk_widget_set_hexpand(name_entry, TRUE); // Allow horizontal expansion
    
    // Add button
    GtkWidget *add_button = gtk_button_new_with_label("Add");
    gtk_widget_add_css_class(add_button, "settings_add-button");
    gtk_box_append(GTK_BOX(entry_container), add_button);
    
    // Append entry container to slider container
    gtk_box_append(GTK_BOX(slider_inner_container_1), entry_container);

    // Scrollable list of names
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_add_css_class(scrolled_window, "settings_names-scroll");
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    
    // GTK_data->group_manager->user_list_for_add = gtk_list_box_new();

    // gtk_widget_add_css_class(GTK_data->group_manager->user_list_for_add , "settings_names-list");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_data->group_manager->user_list_for_add );
    gtk_box_append(GTK_BOX(slider_inner_container_1), scrolled_window);
    
    GtkWidget *slider_inner_container_2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append(GTK_BOX(slider_container), slider_inner_container_2);
    gtk_widget_set_visible(slider_inner_container_2, FALSE);
    // Top container with entry and add button
    GtkWidget *entry_container_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(entry_container_2, "settings_entry-container");
    gtk_widget_set_hexpand(entry_container_2, TRUE); // Allow horizontal expansion
    
    // Entry field
    GtkWidget *name_entry_2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry_2), "Enter user name");
    gtk_widget_add_css_class(name_entry_2, "settings_name-entry");
    gtk_box_append(GTK_BOX(entry_container_2), name_entry_2);
    gtk_widget_set_hexpand(name_entry_2, TRUE); // Allow horizontal expansion
    
    // Add button
    GtkWidget *del_button = gtk_button_new_with_label("Delete");
    gtk_widget_add_css_class(del_button, "settings_del-button");
    gtk_box_append(GTK_BOX(entry_container_2), del_button);
    
    // Append entry container to slider container
    gtk_box_append(GTK_BOX(slider_inner_container_2), entry_container_2);

    // Scrollable list of names
    GtkWidget *scrolled_window_2 = gtk_scrolled_window_new();
    gtk_widget_add_css_class(scrolled_window_2, "settings_names-scroll");
    gtk_widget_set_vexpand(scrolled_window_2, TRUE);
    // GTK_data->group_manager->user_list_for_delete = gtk_list_box_new();
    // gtk_widget_add_css_class(GTK_data->group_manager->user_list_for_delete, "settings_names-list");
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window_2), GTK_data->group_manager->user_list_for_delete);
    gtk_box_append(GTK_BOX(slider_inner_container_2), scrolled_window_2);
    // for slider bellow
    // gtk_widget_set_hexpand(page_controller_container, TRUE); // Allow horizontal expansion
    // gtk_widget_set_vexpand(page_controller_container, TRUE); // Allow vertical expansion

    // Create a small button
    GtkWidget *small_button = gtk_button_new_with_label("Delete group");
    gtk_widget_add_css_class(small_button, "settings-del-button");
    gtk_grid_attach(GTK_GRID(grid), small_button, 0, 1, 1, 1); // Attach to grid at (0, 1)
    gtk_widget_set_hexpand(small_button, TRUE); // Allow horizontal expansion
    gtk_widget_set_vexpand(small_button, FALSE); // Do not allow vertical expansion
    gtk_widget_set_size_request(small_button, -1, 70); // Set a fixed height for the button

    // Small button
    gtk_widget_set_margin_top(small_button, 30);
    gtk_widget_set_margin_bottom(small_button, 30);
    gtk_widget_set_margin_start(small_button, 30);
    gtk_widget_set_margin_end(small_button, 30);

    // Store login, password entries and error label for callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 4);
    entries[0] = user_data; // Here GTK_data
    entries[1] = slider_inner_container_1;
    entries[2] = slider_inner_container_2;
    entries[3] = center_label;

    // Add buttons connections
    //Functioal buttons
    g_object_set_data(G_OBJECT(add_button), "entry", name_entry);
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_name_to_list), GTK_data);
    g_object_set_data(G_OBJECT(del_button), "entry", name_entry_2);
    g_signal_connect(del_button, "clicked", G_CALLBACK(delete_name_from_list), GTK_data);
    g_signal_connect(GTK_data->group_manager->user_list_for_add, "row-selected", G_CALLBACK(on_group_user_row_selected), name_entry);
    g_signal_connect(GTK_data->group_manager->user_list_for_delete, "row-selected", G_CALLBACK(on_group_user_row_selected), name_entry_2);

    //delete group
    g_signal_connect(small_button, "clicked", G_CALLBACK(group_delete), GTK_data);
    //Controller:
    g_signal_connect(left_button, "clicked", G_CALLBACK(switch_between_settings_in_group), entries);
    g_signal_connect(right_button, "clicked", G_CALLBACK(switch_between_settings_in_group), entries);

    // Show the new window
    gtk_window_present(GTK_WINDOW(settings_window));

    // Connect the destroy signal to close the window properly
    g_signal_connect(settings_window, "destroy", G_CALLBACK(gtk_window_destroy), NULL);
}

// Main application window setup
static void on_activate(GtkApplication *app, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 1800, 800); //1800, 1000 було
    GTK_data->window = window;

    // Create the main grid for layout
    GtkWidget *main_grid = gtk_grid_new();
    gtk_widget_set_margin_top(main_grid, 20);
    gtk_widget_set_margin_bottom(main_grid, 20);
    gtk_widget_set_margin_start(main_grid, 20);
    gtk_widget_set_margin_end(main_grid, 20);
    gtk_widget_set_hexpand(main_grid, TRUE); // Make the main grid horizontally expandable
    gtk_widget_set_vexpand(main_grid, TRUE); // Make the main grid vertically expandable

    // --- Top Panel (Settings and Search Bar) ---
    GtkWidget *top_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_grid_attach(GTK_GRID(main_grid), top_panel, 0, 0, 1, 1);

    // Settings button setup
    GtkWidget *settings_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(settings_background, "settings-background");
    gtk_box_append(GTK_BOX(top_panel), settings_background);

    GtkWidget *settings_button = gtk_button_new();
    gtk_widget_add_css_class(settings_button, "settings-button");
    gtk_box_append(GTK_BOX(settings_background), settings_button);

    GtkWidget *settings_icon = gtk_image_new_from_file("src/chat_visual/images/settings.svg");
    gtk_button_set_child(GTK_BUTTON(settings_button), settings_icon);

    // Add this line to connect the callback:
    g_signal_connect(settings_button, "clicked", G_CALLBACK(on_settings_clicked), GTK_data);

    // Search bar setup
    GtkWidget *search_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(search_container, "search-container");
    gtk_box_append(GTK_BOX(top_panel), search_container);

    GtkWidget *search_icon = gtk_image_new_from_file("src/chat_visual/images/search.svg");
    gtk_widget_set_size_request(search_icon, 28, 28);
    gtk_box_append(GTK_BOX(search_container), search_icon);

    GTK_data->search_bar = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(GTK_data->search_bar), "Search");
    gtk_widget_add_css_class(GTK_data->search_bar, "search-bar");
    gtk_box_append(GTK_BOX(search_container), GTK_data->search_bar);

    GtkWidget *erase_button = gtk_button_new_with_label("X");
    gtk_widget_add_css_class(erase_button, "search-erase-button");
    gtk_button_set_has_frame(GTK_BUTTON(erase_button), FALSE);
    gtk_widget_set_size_request(erase_button, 28, 28);
    gtk_box_append(GTK_BOX(search_container), erase_button);

    g_signal_connect(erase_button, "clicked", G_CALLBACK(on_erase_button_clicked), GTK_data);
    g_signal_connect(GTK_data->search_bar, "changed", G_CALLBACK(on_search_bar_changed), GTK_data);

    // --- Sidebar Background Setup For Contacts ---

    GtkWidget *sidebar_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(sidebar_background, "sidebar-background");
    gtk_grid_attach(GTK_GRID(main_grid), sidebar_background, 0, 1, 1, 1);

    GtkWidget *sidebar_scroll_users = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sidebar_scroll_users), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sidebar_scroll_users, TRUE);
    gtk_box_append(GTK_BOX(sidebar_background), sidebar_scroll_users);
    
    GtkWidget *sidebar_users = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(sidebar_users, 350, -1);
    gtk_widget_add_css_class(sidebar_users, "sidebar");
    gtk_widget_set_hexpand(sidebar_users, FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll_users), sidebar_users);
    // gtk_widget_set_visible(sidebar_users, FALSE);

    GtkWidget *sidebar_scroll_groups = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sidebar_scroll_groups), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sidebar_scroll_groups, TRUE);
    gtk_box_append(GTK_BOX(sidebar_background), sidebar_scroll_groups);

    GtkWidget *sidebar_groups = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(sidebar_groups, 350, -1);
    gtk_widget_add_css_class(sidebar_groups, "sidebar");
    gtk_widget_set_hexpand(sidebar_groups, FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll_groups), sidebar_groups);
    gtk_widget_set_visible(sidebar_scroll_groups, FALSE);

    // Add new group button
    GtkWidget *add_group_button = gtk_button_new_with_label("Add new group");
    gtk_widget_add_css_class(add_group_button, "add-group-button");
    gtk_box_append(GTK_BOX(sidebar_background), add_group_button);
    gtk_widget_set_visible(add_group_button, FALSE);

    g_signal_connect(add_group_button, "clicked", G_CALLBACK(on_add_group_button_clicked), GTK_data);
    
    // --- Sidebar Container Setup ---
    GtkWidget *sidebar_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
    gtk_widget_add_css_class(sidebar_container, "sidebar-container");

    // Set fixed size
    gtk_widget_set_size_request(sidebar_container, 400, 60);  // Width: 400px, Height: 60px

    // Prevent expansion
    gtk_widget_set_hexpand(sidebar_container, FALSE);
    gtk_widget_set_vexpand(sidebar_container, FALSE);

    // Keep alignment
    gtk_widget_set_halign(sidebar_container, GTK_ALIGN_FILL);
    gtk_widget_set_valign(sidebar_container, GTK_ALIGN_START);

    // Keep existing margins
    gtk_widget_set_margin_top(sidebar_container, 25);
    gtk_widget_set_margin_bottom(sidebar_container, 17);
    gtk_widget_set_margin_start(sidebar_container, 10);
    gtk_widget_set_margin_end(sidebar_container, 10);
    gtk_grid_attach(GTK_GRID(main_grid), sidebar_container, 0, 2, 1, 1);

    // Left button
    GtkWidget *left_button = gtk_button_new();
    gtk_widget_add_css_class(left_button, "left-button");
    gtk_widget_set_size_request(left_button, 30, 30); // Set specific size for the left button
    gtk_box_append(GTK_BOX(sidebar_container), left_button);

    GtkWidget *back_icon = gtk_image_new_from_file("src/chat_visual/images/back.svg");
    gtk_button_set_child(GTK_BUTTON(left_button), back_icon);
    gtk_widget_set_size_request(back_icon, 24, 24); // Set specific size for the back icon

    // Center label
    GtkWidget *center_label = gtk_label_new("Chats");
    gtk_widget_add_css_class(center_label, "center-label");
    gtk_widget_set_size_request(center_label, 100, 50); // Set specific size for the center label
    gtk_label_set_ellipsize(GTK_LABEL(center_label), PANGO_ELLIPSIZE_END); // Handle overflow gracefully
    gtk_widget_set_halign(center_label, GTK_ALIGN_CENTER); // Center-align the label horizontally
    gtk_widget_set_valign(center_label, GTK_ALIGN_CENTER); // Center-align the label vertically
    gtk_box_append(GTK_BOX(sidebar_container), center_label);

    // Right button
    GtkWidget *right_button = gtk_button_new();
    gtk_widget_add_css_class(right_button, "right-button");
    gtk_widget_set_size_request(right_button, 30, 30); // Set specific size for the right button
    gtk_box_append(GTK_BOX(sidebar_container), right_button);

    GtkWidget *next_icon = gtk_image_new_from_file("src/chat_visual/images/forward_arrow.svg");
    gtk_button_set_child(GTK_BUTTON(right_button), next_icon);
    gtk_widget_set_size_request(next_icon, 24, 24); // Set specific size for the forward icon
    
    gtk_box_set_homogeneous(GTK_BOX(sidebar_container), TRUE); // Make all children the same size

    // --- Chat Area Setup ---
    GtkWidget *chat_area_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(chat_area_background, "chat-area-background");
    gtk_grid_attach(GTK_GRID(main_grid), chat_area_background, 1, 1, 1, 1);

    // Chat header
    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_size_request(chat_header, -1, 60);
    gtk_widget_add_css_class(chat_header, "chat-header");

    GtkWidget *avatar_circle = gtk_image_new_from_file("src/chat_visual/images/person.svg");
    gtk_widget_set_size_request(avatar_circle, 60, 60);
    //gtk_widget_add_css_class(avatar_circle, "avatar-circle");
    gtk_box_append(GTK_BOX(chat_header), avatar_circle);

    GtkWidget *avatar_circle_group = gtk_image_new_from_file("src/chat_visual/images/group.svg");
    gtk_widget_set_size_request(avatar_circle_group, 60, 60);
    gtk_widget_set_margin_start(avatar_circle_group, 10);
    gtk_widget_set_margin_end(avatar_circle_group, 10);
    gtk_widget_set_margin_top(avatar_circle_group, 9);
    //gtk_widget_add_css_class(avatar_circle_group, "group-avatar-circle");
    gtk_box_append(GTK_BOX(chat_header), avatar_circle_group);

    // Spacer (optional, ensures spacing between the label and button)
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE); // Take up remaining horizontal space
    

    // Create the settings button
    GtkWidget *settings_group_button = gtk_button_new();
    gtk_widget_add_css_class(settings_group_button, "settings-group-button");
    gtk_widget_set_margin_end(settings_group_button, 10);
    gtk_widget_set_margin_bottom(settings_group_button, 10);
    GtkWidget *settings_group_icon = gtk_image_new_from_file("src/chat_visual/images/settings.svg");
    gtk_button_set_child(GTK_BUTTON(settings_group_button), settings_group_icon);
    gtk_widget_set_hexpand(settings_group_button, FALSE); // Prevent the button from expanding
    gtk_widget_set_halign(settings_group_button, GTK_ALIGN_END); // Align to the far right

    // Initially hide the settings button
    gtk_widget_set_visible(settings_group_button, FALSE);

    GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); 
    gtk_widget_set_margin_top(input_container, 10);

    GtkWidget *error_label = gtk_label_new("Too long message > 500");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_widget_set_visible(error_label, FALSE);
    gtk_box_append(GTK_BOX(input_container), error_label);

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(input_box, "input-box");
    gtk_box_append(GTK_BOX(input_container), input_box);

    // Create an overlay container
    GtkWidget *entry_overlay = gtk_overlay_new();
    gtk_widget_set_hexpand(entry_overlay, TRUE);
    gtk_box_append(GTK_BOX(input_box), entry_overlay);

    // Create message entry
    GTK_data->message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(GTK_data->message_entry), "Enter message");
    gtk_widget_set_hexpand(GTK_data->message_entry, TRUE);
    gtk_widget_add_css_class(GTK_data->message_entry, "message-entry");
    gtk_overlay_set_child(GTK_OVERLAY(entry_overlay), GTK_data->message_entry);
    GTK_data->entry_overlay = entry_overlay;

    // Create and setup cancel button
    // GtkWidget *cancel_button = gtk_button_new_with_label("X");
    // gtk_widget_add_css_class(cancel_button, "search-erase-button");
    // gtk_widget_set_size_request(cancel_button, 20, 20);
    // gtk_overlay_add_overlay(GTK_OVERLAY(entry_overlay), cancel_button);
    // gtk_widget_set_halign(cancel_button, GTK_ALIGN_END);
    // gtk_widget_set_valign(cancel_button, GTK_ALIGN_CENTER);
    // gtk_widget_set_margin_end(cancel_button, 15);
    // gtk_widget_set_visible(cancel_button, false);

    chat_data_t *active_chat = NULL;

    chat_manager_t *chat_manager = g_new(chat_manager_t, 1);
    chat_manager->chats = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    chat_manager->active_chat = active_chat;
    chat_manager->sidebar = sidebar_users;
    chat_manager->error_label = error_label;
    //chat_manager->cancel_button = cancel_button;
    chat_manager->avatar_circle = avatar_circle;
    chat_manager->settings_group_button = NULL;

    chat_manager_t *group_manager = g_new(chat_manager_t, 1);
    group_manager->chats = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    group_manager->active_chat = active_chat;
    group_manager->sidebar = sidebar_groups;
    group_manager->error_label = error_label;
    //group_manager->cancel_button = cancel_button;
    group_manager->avatar_circle = avatar_circle_group;
    group_manager->settings_group_button = settings_group_button;

    GtkWidget *chat_user_label = gtk_label_new("");
    gtk_widget_add_css_class(chat_user_label, "header-name");
    gtk_box_append(GTK_BOX(chat_header), chat_user_label);
    chat_manager->chat_user_label = chat_user_label;
    group_manager->chat_user_label = chat_user_label;

    gtk_box_append(GTK_BOX(chat_header), spacer);

    gtk_box_append(GTK_BOX(chat_header), settings_group_button);

    GTK_data->chat_manager = chat_manager;
    GTK_data->group_manager = group_manager;
    // printf("chat_manager initialized at: %p\n", (void *)GTK_data->chat_manager);
    // printf("GTK_data address: %p, chat_manager address: %p\n", (void *)GTK_data, (void *)GTK_data->chat_manager);
    // chat_manager->active_chat = NULL;
    chat_manager->input_box = input_box;
    chat_manager->chat_header = chat_header;
    chat_manager->chat_area_background = chat_area_background;

    // group_manager->active_chat = NULL;
    group_manager->input_box = input_box;
    group_manager->chat_header = chat_header;
    group_manager->chat_area_background = chat_area_background;

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "send-button");
    chat_manager->send_button = send_button;
    group_manager->send_button = send_button;
    // Connect the callback to the send button
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), GTK_data);    

    gtk_box_append(GTK_BOX(input_box), send_button);

    gtk_widget_set_visible(input_box, false);
    gtk_widget_set_visible(chat_area_background, false);
    gtk_widget_set_visible(chat_header, false);

    GtkWidget *select_a_chat_label = gtk_label_new("Select a chat to start messaging");
    gtk_widget_add_css_class(select_a_chat_label, "placeholder-message");
    gtk_grid_attach(GTK_GRID(main_grid), select_a_chat_label, 1, 1, 1, 1);
    gtk_widget_set_size_request(select_a_chat_label, -1, 500);
    gtk_widget_set_hexpand(select_a_chat_label, true);
    gtk_widget_set_visible(select_a_chat_label, true);
    chat_manager->select_a_chat_label = select_a_chat_label;
    group_manager->select_a_chat_label = select_a_chat_label;

    // Store login, password entries and error label for callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 5);
    entries[0] = user_data; // Here GTK_data
    entries[1] = add_group_button;
    entries[2] = sidebar_scroll_groups;
    entries[3] = sidebar_scroll_users;
    entries[4] = center_label;

    // Buttons connects functions
    g_signal_connect(right_button, "clicked", G_CALLBACK(switch_between_groups_chats), entries);
    g_signal_connect(left_button, "clicked", G_CALLBACK(switch_between_groups_chats), entries); 
    g_signal_connect(settings_group_button, "clicked", G_CALLBACK(on_settings_group_button_clicked), user_data);   
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), entries);
    // Layout setup
    gtk_grid_attach(GTK_GRID(main_grid), chat_header, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), input_container, 1, 2, 1, 1);

    // Add main grid to window
    gtk_window_set_child(GTK_WINDOW(window), main_grid);
    gtk_window_present(GTK_WINDOW(window));
    
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, &send_msg_handler, (void*)GTK_data) != 0) {
        printf("ERROR: pthread\n");
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, (void*)GTK_data) != 0) {
        printf("ERROR: pthread\n");
    }

}

void GTK_start(GTK_data_t *GTK_data) {
    static GtkApplication *app = NULL;

    if (app == NULL) {
        switch_to_groups = false;
        switch_settings_in_group = false;
        GTK_data->profile_data = (profile_data_t *)malloc(sizeof(profile_data_t));
        GTK_data->profile_data->login_list = NULL;
        
        pthread_mutex_init(&GTK_data->pthread_mutex, NULL);
        pthread_cond_init(&GTK_data->pthread_cond, NULL);

        app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_NON_UNIQUE);
        g_signal_connect(app, "activate", G_CALLBACK(on_activate), GTK_data);
    }

    g_application_run(G_APPLICATION(app), 0, NULL);
}
