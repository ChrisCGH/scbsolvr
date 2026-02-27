// CribDialog.cpp - Crib dialog (GTK3 Linux port)
#include "CribDialog.h"
#include <string>

CribDialog::CribDialog(GtkWindow* parent, const Crib& crib, bool preserve_punctuation)
    : crib_(crib),
      preserve_punctuation_(preserve_punctuation),
      crib_signal_blocked_(false)
{
    dialog_ = gtk_dialog_new_with_buttons(
        "Cribbing",
        parent,
        GTK_DIALOG_MODAL,
        "_OK",     GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL,
        nullptr);
    gtk_window_set_default_size(GTK_WINDOW(dialog_), 600, 320);

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog_));
    gtk_container_set_border_width(GTK_CONTAINER(content), 8);

    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(content), grid);

    int row = 0;

    // Cipher text display (read-only, monospace, scrollable)
    GtkWidget* ct_lbl = gtk_label_new("Cipher text:");
    gtk_label_set_xalign(GTK_LABEL(ct_lbl), 0.0);
    gtk_grid_attach(GTK_GRID(grid), ct_lbl, 0, row, 2, 1);
    row++;

    // Apply monospace CSS to all labels in this dialog
    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        ".mono-label { font-family: monospace; font-size: 10pt; }", -1, nullptr);

    cipher_label_ = gtk_label_new(crib_.get_ct() ? crib_.get_ct() : "");
    gtk_label_set_selectable(GTK_LABEL(cipher_label_), TRUE);
    gtk_label_set_xalign(GTK_LABEL(cipher_label_), 0.0);
    gtk_style_context_add_provider(gtk_widget_get_style_context(cipher_label_),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_widget_set_name(cipher_label_, "mono-label");
    GtkWidget* ct_frame = gtk_frame_new(nullptr);
    gtk_container_add(GTK_CONTAINER(ct_frame), cipher_label_);
    gtk_widget_set_hexpand(ct_frame, TRUE);
    gtk_grid_attach(GTK_GRID(grid), ct_frame, 0, row, 2, 1);
    row++;

    // Crib string entry
    GtkWidget* crib_lbl = gtk_label_new("Crib:");
    gtk_label_set_xalign(GTK_LABEL(crib_lbl), 0.0);
    gtk_grid_attach(GTK_GRID(grid), crib_lbl, 0, row, 1, 1);
    crib_entry_ = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(crib_entry_),
                       crib_.get_crib_string() ? crib_.get_crib_string() : "");
    gtk_widget_set_hexpand(crib_entry_, TRUE);
    gtk_grid_attach(GTK_GRID(grid), crib_entry_, 1, row, 1, 1);
    g_signal_connect(crib_entry_, "changed", G_CALLBACK(on_crib_changed), this);
    row++;

    // Plain text display
    GtkWidget* pt_lbl = gtk_label_new("Plain text:");
    gtk_label_set_xalign(GTK_LABEL(pt_lbl), 0.0);
    gtk_grid_attach(GTK_GRID(grid), pt_lbl, 0, row, 2, 1);
    row++;

    plain_label_ = gtk_label_new("");
    gtk_label_set_selectable(GTK_LABEL(plain_label_), TRUE);
    gtk_label_set_xalign(GTK_LABEL(plain_label_), 0.0);
    gtk_style_context_add_provider(gtk_widget_get_style_context(plain_label_),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_widget_set_name(plain_label_, "mono-label");
    GtkWidget* pt_frame = gtk_frame_new(nullptr);
    gtk_container_add(GTK_CONTAINER(pt_frame), plain_label_);
    gtk_widget_set_hexpand(pt_frame, TRUE);
    gtk_grid_attach(GTK_GRID(grid), pt_frame, 0, row, 2, 1);
    row++;

    // Key display
    GtkWidget* key_lbl_h = gtk_label_new("Key:");
    gtk_label_set_xalign(GTK_LABEL(key_lbl_h), 0.0);
    gtk_grid_attach(GTK_GRID(grid), key_lbl_h, 0, row, 2, 1);
    row++;

    key_label_ = gtk_label_new("");
    gtk_label_set_selectable(GTK_LABEL(key_label_), TRUE);
    gtk_label_set_xalign(GTK_LABEL(key_label_), 0.0);
    gtk_style_context_add_provider(gtk_widget_get_style_context(key_label_),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_widget_set_name(key_label_, "mono-label");
    g_object_unref(css);
    GtkWidget* key_frame = gtk_frame_new(nullptr);
    gtk_container_add(GTK_CONTAINER(key_frame), key_label_);
    gtk_widget_set_hexpand(key_frame, TRUE);
    gtk_grid_attach(GTK_GRID(grid), key_frame, 0, row, 2, 1);
    row++;

    // Navigation buttons
    GtkWidget* nav_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(nav_box), GTK_BUTTONBOX_START);
    gtk_box_set_spacing(GTK_BOX(nav_box), 4);

    GtkWidget* clear_btn = gtk_button_new_with_mnemonic("_Clear");
    GtkWidget* left_btn  = gtk_button_new_with_mnemonic("◄ Pre_vious");
    GtkWidget* right_btn = gtk_button_new_with_mnemonic("_Next ►");

    gtk_container_add(GTK_CONTAINER(nav_box), clear_btn);
    gtk_container_add(GTK_CONTAINER(nav_box), left_btn);
    gtk_container_add(GTK_CONTAINER(nav_box), right_btn);

    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear),      this);
    g_signal_connect(left_btn,  "clicked", G_CALLBACK(on_next_left),  this);
    g_signal_connect(right_btn, "clicked", G_CALLBACK(on_next_right), this);

    gtk_grid_attach(GTK_GRID(grid), nav_box, 0, row, 2, 1);

    gtk_widget_show_all(content);
    do_display();
}

CribDialog::~CribDialog()
{
    gtk_widget_destroy(dialog_);
}

bool CribDialog::run(Crib& result)
{
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog_));
    if (resp == GTK_RESPONSE_OK)
    {
        result = crib_;
        return true;
    }
    return false;
}

void CribDialog::do_display()
{
    // Plain text
    char* pt = crib_.get_pt();
    std::string plain = pt ? pt : "";
    if (preserve_punctuation_ && crib_.get_original_ct())
    {
        const char* ref = Mono_Solver::reformat(plain.c_str(),
                                                crib_.get_original_ct());
        plain = ref ? ref : "";
    }
    gtk_label_set_text(GTK_LABEL(plain_label_), plain.c_str());

    // Key  (format: key_string \n abcdefghijklmnopqrstuvwxyz)
    std::string key_str = crib_.get_fixed_key().get_key();
    key_str += "\nabcdefghijklmnopqrstuvwxyz";
    gtk_label_set_text(GTK_LABEL(key_label_), key_str.c_str());
}

void CribDialog::on_crib_changed(GtkEditable* editable, gpointer data)
{
    CribDialog* self = static_cast<CribDialog*>(data);
    if (self->crib_signal_blocked_) return;

    const gchar* crib_text = gtk_entry_get_text(GTK_ENTRY(editable));
    int pos = self->crib_.get_position();
    const char* ct = self->crib_.get_ct();
    self->crib_ = Crib(ct ? ct : "", crib_text ? crib_text : "");
    self->crib_.place_at(pos);
    self->do_display();
}

void CribDialog::on_clear(GtkButton*, gpointer data)
{
    CribDialog* self = static_cast<CribDialog*>(data);
    self->crib_signal_blocked_ = true;
    gtk_entry_set_text(GTK_ENTRY(self->crib_entry_), "");
    self->crib_signal_blocked_ = false;
    const char* ct = self->crib_.get_ct();
    self->crib_ = Crib(ct ? ct : "", "");
    self->do_display();
}

void CribDialog::on_next_left(GtkButton*, gpointer data)
{
    CribDialog* self = static_cast<CribDialog*>(data);
    self->crib_.next_left();
    self->do_display();
}

void CribDialog::on_next_right(GtkButton*, gpointer data)
{
    CribDialog* self = static_cast<CribDialog*>(data);
    self->crib_.next_right();
    self->do_display();
}
