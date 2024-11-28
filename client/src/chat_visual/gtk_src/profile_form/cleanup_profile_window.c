#include "GTK.h"

void cleanup_profile_window(GTK_data_t *GTK_data) {
    if (!GTK_data || !GTK_data->profile_data) return;

    // Clear any existing messages
    if (GTK_data->profile_data->create_success_label) {
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->create_success_label), "");
        gtk_widget_set_visible(GTK_data->profile_data->create_success_label, FALSE);
    }
    if (GTK_data->profile_data->create_error_label) {
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->create_error_label), "");
        gtk_widget_set_visible(GTK_data->profile_data->create_error_label, FALSE);
    }

    // Clear input fields
    if (GTK_data->profile_data->login_entry) {
        gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->login_entry), "");
    }
    if (GTK_data->profile_data->password_entry) {
        gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->password_entry), "");
    }
}
