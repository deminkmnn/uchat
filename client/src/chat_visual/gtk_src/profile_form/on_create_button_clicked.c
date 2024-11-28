#include "GTK.h"

void on_create_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;

    // Validate user_data
    if (!user_data) {
        fprintf(stderr, "Error: user_data is NULL.\n");
        return;
    }

    // Unpack entries
    gpointer *entries = (gpointer *)user_data;
    GtkWidget *login_entry = GTK_WIDGET(entries[0]);
    GtkWidget *password_entry = GTK_WIDGET(entries[1]);
    GtkWidget *error_label = GTK_WIDGET(entries[2]);
    GtkWidget *success_label = GTK_WIDGET(entries[5]);
    GtkWidget *admin_check = GTK_WIDGET(entries[6]);
    GTK_data_t *GTK_data = (GTK_data_t *)entries[3];
    SSL *ssl = GTK_data->call_data->ssl;

    // Validate individual widgets
    if (!GTK_IS_WIDGET(login_entry) || !GTK_IS_WIDGET(password_entry) || 
        !GTK_IS_WIDGET(error_label) || !GTK_IS_WIDGET(success_label) || 
        !GTK_IS_WIDGET(admin_check)) {
        fprintf(stderr, "Error: One or more widgets are invalid.\n");
        return;
    }

    // Get text from input fields
    const char *login_input = gtk_editable_get_text(GTK_EDITABLE(login_entry));
    const char *password_input = gtk_editable_get_text(GTK_EDITABLE(password_entry));

    if (!login_input || !*login_input || !password_input || !*password_input) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Login and password cannot be empty.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message

        // Set timeout to hide error label after 1.5 seconds
        g_timeout_add(1500, hide_label_after_timeout, error_label);
        return;
    }

    // Validate login and password
    if (strlen(login_input) < 2 || strlen(login_input) > 30) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Login must be 2-30 characters.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message

        // Set timeout to hide error label after 1.5 seconds
        g_timeout_add(1500, hide_label_after_timeout, error_label);
        return;
    }
    if (!check_password((char *)password_input)) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Invalid password. Must be 8-20 characters.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message

        // Set timeout to hide error label after 1.5 seconds
        g_timeout_add(1500, hide_label_after_timeout, error_label);
        return;
    }

    // Get the state of the admin checkbox
    gboolean is_admin = gtk_check_button_get_active(GTK_CHECK_BUTTON(admin_check));

    // Clear error label
    gtk_label_set_text(GTK_LABEL(error_label), "");
    gtk_widget_remove_css_class(error_label, "error-label");

    // Send data to server
    create_new_user(ssl, (char *)login_input, (char *)password_input, is_admin);
}
