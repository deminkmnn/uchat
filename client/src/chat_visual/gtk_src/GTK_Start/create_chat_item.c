#include "GTK.h"

GtkWidget* create_chat_item(const char *name, int chat_id, const char *message, const char *time, gboolean is_online, gboolean is_group, GTK_data_t *GTK_data) {
    chat_data_t *chat;
    if (is_group) {
        chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(chat_id));
    }else {
        chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(chat_id));
    }
    chat->is_group = is_group;
    
    GtkWidget *button = gtk_button_new();
    gtk_widget_add_css_class(button, "chat-item-button");
    
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_add_css_class(grid, "chat-item");
    gtk_grid_set_column_spacing(GTK_GRID(grid), 4);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);

    // Avatar
    GtkWidget *avatar_circle = gtk_image_new_from_file("src/chat_visual/images/person.svg");
    gtk_widget_set_size_request(avatar_circle, 60, 60);
    //gtk_widget_add_css_class(avatar_circle, "avatar-circle");
    gtk_grid_attach(GTK_GRID(grid), avatar_circle, 0, 0, 1, 2);

    // Horizontal box to hold name label and status indicator
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Name label
    GtkWidget *name_label = gtk_label_new(name);
    gtk_widget_add_css_class(name_label, "chat-name");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(name_box), name_label);

    GtkWidget *number_of_unread_messages = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(number_of_unread_messages), "");
    gtk_widget_add_css_class(number_of_unread_messages, "number_of_unread_messages");
    gtk_widget_set_halign(number_of_unread_messages, GTK_ALIGN_END);
    gtk_widget_set_valign(number_of_unread_messages, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_of_unread_messages, TRUE);
    gtk_widget_set_visible(number_of_unread_messages, false); 
    chat->number_of_unread_messages = number_of_unread_messages;
    
    gtk_grid_attach(GTK_GRID(grid), number_of_unread_messages, 3, 1, 1, 1);

    // Status indicator
    if (!is_group) { // Only show status indicator if it's not a group
        GtkWidget *status_indicator = gtk_drawing_area_new();
        gtk_widget_set_size_request(status_indicator, 10, 10);
        gtk_widget_add_css_class(status_indicator, is_online ? "status-online" : "status-offline");
        gtk_box_append(GTK_BOX(name_box), status_indicator);
        chat->status_indicator = status_indicator;
    }

    // Place name_box at (1, 0)
    gtk_grid_attach(GTK_GRID(grid), name_box, 1, 0, 1, 1);

    // Message preview label
    GtkWidget *message_label = gtk_label_new(message);
    gtk_widget_add_css_class(message_label, "message-preview");
    gtk_widget_set_halign(message_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), message_label, 1, 1, 1, 1);
    chat->message_label = message_label;
    // Time label
    GtkWidget *time_label = gtk_label_new(time);
    gtk_widget_add_css_class(time_label, "message-time");
    gtk_widget_set_halign(time_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), time_label, 2, 0, 1, 2);
    chat->time_label = time_label;
    if (is_group) {
        gtk_widget_add_css_class(grid, "group-chat"); // Apply a specific style for group chats
    }

    gtk_button_set_child(GTK_BUTTON(button), grid);

    g_object_set_data(G_OBJECT(button), "chat_id", GINT_TO_POINTER(chat_id));
    if (is_group) {
        g_object_set_data(G_OBJECT(button), "is_group", GINT_TO_POINTER(1));
    }

    // Single signal connection for both group and user chats
    g_signal_connect(button, "clicked", G_CALLBACK(switch_chat), GTK_data);
    
    chat->button = button;

    return button;
}

