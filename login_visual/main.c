#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../inc/cJSON.h" // Ensure you have cJSON library installed and accessible

// Function prototypes
bool check_name(const char *name);
bool check_password(const char *password);

/* Apply CSS styling to the widget and its children */
static void apply_css(GtkWidget *widget) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GError *error = NULL;
    
    GFile *file = g_file_new_for_path("style.css");
    gtk_css_provider_load_from_file(css_provider, file);
    
    if (error) {
        g_warning("Error loading CSS: %s", error->message);
        g_error_free(error);
    } else {
        gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                                   GTK_STYLE_PROVIDER(css_provider),
                                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
    
    g_object_unref(file);
    g_object_unref(css_provider);
}

/* Function to convert login and password to JSON and print */
static void print_json_data(const char *login, const char *password) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", login);
    cJSON_AddStringToObject(json, "password", password);

    char *json_str = cJSON_Print(json);
    printf("%s\n", json_str);

    cJSON_Delete(json);
    free(json_str);
}

/* Name validation function */
bool check_name(const char *name) {
    int length = strlen(name);
    return length >= 2 && length <= 30;
}

/* Password validation function */
bool check_password(const char *password) {
    int length = strlen(password);
    if (length < 8 || length > 64) return false;

    for (int i = 0; i < length; i++) {
        if (password[i] < 33 || password[i] > 126) {
            return false;
        }
    }
    return true;
}

/* Callback function to handle the login button click */
static void on_login_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;
    const char *login = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(entries[1]));
    GtkWidget *error_label = entries[2];

    bool valid_login = check_name(login);
    bool valid_password = check_password(password);

    // Reset error label text
    gtk_label_set_text(GTK_LABEL(error_label), "");

    // Determine the error message based on validation results
    if (!valid_login && !valid_password) {
        gtk_label_set_text(GTK_LABEL(error_label), "Both login and password are invalid.");
    } else if (!valid_login) {
        gtk_label_set_text(GTK_LABEL(error_label), "Invalid login. Must be 2-30 characters.");
    } else if (!valid_password) {
        gtk_label_set_text(GTK_LABEL(error_label), "Invalid password. Must be 8-64 characters.");
    } else {
        // If validation passes, print JSON data
        print_json_data(login, password);
    }

    // Add red color style to the error label if there's an error
    if (!valid_login || !valid_password) {
        gtk_widget_add_css_class(error_label, "error-label");
    } else {
        gtk_widget_remove_css_class(error_label, "error-label");
    }
}

static void on_activate(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(main_container, 40);
    gtk_widget_set_margin_end(main_container, 40);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    
    // Login field container
    GtkWidget *login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *login_label = gtk_label_new("Login:");
    gtk_widget_add_css_class(login_label, "input-label");
    gtk_widget_set_halign(login_label, GTK_ALIGN_START);
    GtkWidget *login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_entry), "Enter login");
    gtk_widget_set_hexpand(login_entry, TRUE);
    gtk_box_append(GTK_BOX(login_box), login_label);
    gtk_box_append(GTK_BOX(login_box), login_entry);

    // Password field container
    GtkWidget *password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_add_css_class(password_label, "input-label");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_hexpand(password_entry, TRUE);
    gtk_box_append(GTK_BOX(password_box), password_label);
    gtk_box_append(GTK_BOX(password_box), password_entry);

    // Error label for displaying login/password issues
    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(password_box), error_label);

    // Remember me checkbox and login button
    GtkWidget *remember_me_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *remember_me_check = gtk_check_button_new_with_label("Remember me");
    gtk_widget_add_css_class(remember_me_check, "remember-me-checkbox");
    gtk_widget_set_hexpand(remember_me_check, TRUE);

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    gtk_widget_add_css_class(login_button, "login-button");

    // Store login, password entries and error label for callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 3);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = error_label;

    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entries);

    gtk_box_append(GTK_BOX(remember_me_box), remember_me_check);
    gtk_box_append(GTK_BOX(remember_me_box), login_button);
    
    gtk_box_append(GTK_BOX(vbox), login_box);
    gtk_box_append(GTK_BOX(vbox), password_box);
    gtk_box_append(GTK_BOX(vbox), remember_me_box);
    
    gtk_box_append(GTK_BOX(main_container), vbox);
    
    apply_css(window);

    gtk_window_set_child(GTK_WINDOW(window), main_container);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
