// FixKeyDialog.cpp - Fix key dialog (GTK3 Linux port)
#include "FixKeyDialog.h"
#include <cstring>
#include <cctype>

struct EntryData
{
    FixKeyDialog* dlg;
    int           index;
};

FixKeyDialog::FixKeyDialog(GtkWindow* parent, const Fixed_Key& fixed)
    : fixed_(fixed)
{
    dialog_ = gtk_dialog_new_with_buttons(
        "Fix Key",
        parent,
        GTK_DIALOG_MODAL,
        "_OK",     GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL,
        nullptr);
    gtk_window_set_default_size(GTK_WINDOW(dialog_), 480, 400);

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog_));
    gtk_container_set_border_width(GTK_CONTAINER(content), 8);

    // Scrollable area for the 26 rows
    GtkWidget* scroll = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(content), scroll);

    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 4);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    // Header
    GtkWidget* hdr_pt = gtk_label_new("Plaintext");
    GtkWidget* hdr_ct = gtk_label_new("Ciphertext");
    gtk_grid_attach(GTK_GRID(grid), hdr_pt, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), hdr_ct, 1, 0, 1, 1);

    for (int i = 0; i < 26; i++)
    {
        char label_text[4] = { static_cast<char>('a' + i), ':', '\0' };
        GtkWidget* lbl = gtk_label_new(label_text);
        gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);

        GtkWidget* entry = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(entry), 1);
        gtk_entry_set_width_chars(GTK_ENTRY(entry), 3);
        gtk_widget_set_hexpand(entry, FALSE);
        entries_[i] = entry;

        EntryData* ed = new EntryData{this, i};
        g_signal_connect_data(entry, "changed",
                              G_CALLBACK(on_entry_changed),
                              ed,
                              [](gpointer p, GClosure*){ delete static_cast<EntryData*>(p); },
                              static_cast<GConnectFlags>(0));

        gtk_grid_attach(GTK_GRID(grid), lbl,   0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, i + 1, 1, 1);
    }

    // Clear button at the bottom
    GtkWidget* clear_btn = gtk_button_new_with_mnemonic("_Clear All");
    gtk_container_add(GTK_CONTAINER(content), clear_btn);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear), this);

    gtk_widget_show_all(content);
    do_display();
}

FixKeyDialog::~FixKeyDialog()
{
    gtk_widget_destroy(dialog_);
}

bool FixKeyDialog::run(Fixed_Key& result)
{
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog_));
    if (resp == GTK_RESPONSE_OK)
    {
        result = fixed_;
        return true;
    }
    return false;
}

void FixKeyDialog::do_display()
{
    for (int i = 0; i < 26; i++)
    {
        char ct = fixed_.get_ct(static_cast<char>('a' + i));
        gchar text[2] = { (ct != ' ') ? ct : '\0', '\0' };
        // Block signal to avoid re-entrant on_entry_changed
        g_signal_handlers_block_matched(entries_[i], G_SIGNAL_MATCH_FUNC,
                                        0, 0, nullptr, (gpointer)on_entry_changed, nullptr);
        gtk_entry_set_text(GTK_ENTRY(entries_[i]), text);
        g_signal_handlers_unblock_matched(entries_[i], G_SIGNAL_MATCH_FUNC,
                                          0, 0, nullptr, (gpointer)on_entry_changed, nullptr);
    }
}

void FixKeyDialog::on_entry_changed(GtkEditable* editable, gpointer data)
{
    EntryData* ed = static_cast<EntryData*>(data);
    ed->dlg->on_entry_changed_impl(ed->index);
    (void)editable;
}

void FixKeyDialog::on_entry_changed_impl(int i)
{
    const gchar* text = gtk_entry_get_text(GTK_ENTRY(entries_[i]));
    char pt = static_cast<char>('a' + i);

    if (!text || !*text)
    {
        // Cleared
        if (fixed_.is_set(pt))
            fixed_.clear(fixed_.get_ct(pt));
        do_display();
        return;
    }

    char ct = text[0];
    if (ct >= 'a' && ct <= 'z')
        ct = static_cast<char>(ct - 'a' + 'A'); // auto-uppercase

    if (ct < 'A' || ct > 'Z')
    {
        // Invalid character – revert
        do_display();
        return;
    }

    if (fixed_.is_set(pt))
        fixed_.clear(fixed_.get_ct(pt));
    fixed_.set(pt, ct);
    do_display();
}

void FixKeyDialog::on_clear(GtkButton*, gpointer data)
{
    FixKeyDialog* self = static_cast<FixKeyDialog*>(data);
    self->fixed_.clear();
    self->do_display();
}
