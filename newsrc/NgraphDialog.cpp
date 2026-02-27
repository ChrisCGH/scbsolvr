// NgraphDialog.cpp - NGraph file creator dialog (GTK3 Linux port)
#include "NgraphDialog.h"
#include <cstdio>
#include <sys/stat.h>

NgraphDialog::NgraphDialog(GtkWindow* parent)
    : creator_(nullptr),
      sample_size_(0),
      progress_(0),
      update_pending_(false)
{
    pthread_mutex_init(&progress_mutex_, nullptr);

    dialog_ = gtk_dialog_new_with_buttons(
        "Create NGraph File",
        parent,
        GTK_DIALOG_MODAL,
        "_Close", GTK_RESPONSE_CLOSE,
        nullptr);
    gtk_window_set_default_size(GTK_WINDOW(dialog_), 500, 280);

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog_));
    gtk_container_set_border_width(GTK_CONTAINER(content), 8);

    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(content), grid);

    int row = 0;

    // Sample file row
    GtkWidget* sl  = gtk_label_new("Sample file:");
    gtk_label_set_xalign(GTK_LABEL(sl), 0.0);
    sample_entry_  = gtk_entry_new();
    gtk_widget_set_hexpand(sample_entry_, TRUE);
    GtkWidget* sb  = gtk_button_new_with_label("Browse…");
    gtk_grid_attach(GTK_GRID(grid), sl,           0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), sample_entry_, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), sb,           2, row, 1, 1);
    g_signal_connect(sb, "clicked", G_CALLBACK(on_browse_sample), this);
    row++;

    // NGraph output file row
    GtkWidget* nl  = gtk_label_new("NGraph file:");
    gtk_label_set_xalign(GTK_LABEL(nl), 0.0);
    ngraph_entry_  = gtk_entry_new();
    gtk_widget_set_hexpand(ngraph_entry_, TRUE);
    GtkWidget* nb  = gtk_button_new_with_label("Browse…");
    gtk_grid_attach(GTK_GRID(grid), nl,           0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ngraph_entry_, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), nb,           2, row, 1, 1);
    g_signal_connect(nb, "clicked", G_CALLBACK(on_browse_ngraph), this);
    row++;

    // N value row
    GtkWidget* nv_lbl = gtk_label_new("N (graph length 3–8):");
    gtk_label_set_xalign(GTK_LABEL(nv_lbl), 0.0);
    n_spin_ = gtk_spin_button_new_with_range(3, 8, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(n_spin_), 4);
    gtk_grid_attach(GTK_GRID(grid), nv_lbl, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), n_spin_, 1, row, 1, 1);
    row++;

    // Spaces checkbox
    spaces_check_ = gtk_check_button_new_with_label("Include spaces");
    gtk_grid_attach(GTK_GRID(grid), spaces_check_, 0, row, 2, 1);
    row++;

    // Progress bar
    progress_bar_   = gtk_progress_bar_new();
    gtk_grid_attach(GTK_GRID(grid), progress_bar_, 0, row, 3, 1);
    row++;

    // Progress text label
    progress_label_ = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(progress_label_), 0.0);
    gtk_grid_attach(GTK_GRID(grid), progress_label_, 0, row, 3, 1);
    row++;

    // Action buttons
    GtkWidget* hbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbox), GTK_BUTTONBOX_START);
    gtk_box_set_spacing(GTK_BOX(hbox), 4);
    create_btn_ = gtk_button_new_with_mnemonic("_Create");
    done_btn_   = gtk_button_new_with_mnemonic("_Done");
    gtk_widget_set_sensitive(done_btn_, FALSE);
    gtk_container_add(GTK_CONTAINER(hbox), create_btn_);
    gtk_container_add(GTK_CONTAINER(hbox), done_btn_);
    gtk_grid_attach(GTK_GRID(grid), hbox, 0, row, 3, 1);

    g_signal_connect(create_btn_, "clicked", G_CALLBACK(on_create), this);
    g_signal_connect(done_btn_,   "clicked", G_CALLBACK(on_done),   this);

    gtk_widget_show_all(content);
}

NgraphDialog::~NgraphDialog()
{
    gtk_widget_destroy(dialog_);
    pthread_mutex_destroy(&progress_mutex_);
    delete creator_;
}

void NgraphDialog::run()
{
    gtk_dialog_run(GTK_DIALOG(dialog_));
}

// Ngraph_File_Creator_Callback
void NgraphDialog::process_progress(const Ngraph_File_Creator& creator)
{
    pthread_mutex_lock(&progress_mutex_);
    progress_ = creator.get_progress();
    if (!update_pending_)
    {
        update_pending_ = true;
        g_idle_add(on_idle_progress, this);
    }
    pthread_mutex_unlock(&progress_mutex_);
}

gboolean NgraphDialog::on_idle_progress(gpointer data)
{
    NgraphDialog* self = static_cast<NgraphDialog*>(data);
    self->apply_progress_update();
    return G_SOURCE_REMOVE;
}

void NgraphDialog::apply_progress_update()
{
    pthread_mutex_lock(&progress_mutex_);
    long int prog = progress_;
    update_pending_ = false;
    pthread_mutex_unlock(&progress_mutex_);

    if (creator_->is_done())
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_), 1.0);
        gtk_label_set_text(GTK_LABEL(progress_label_), "Reading file, 100% done");
        gtk_widget_set_sensitive(done_btn_,   TRUE);
        gtk_widget_set_sensitive(create_btn_, TRUE);
    }
    else if (sample_size_ > 0)
    {
        double pct = (static_cast<double>(prog) * 100.0) /
                     static_cast<double>(sample_size_);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_),
                                      pct / 100.0);
        char buf[128];
        snprintf(buf, sizeof(buf), "Reading file, %4.2f%% done", pct);
        gtk_label_set_text(GTK_LABEL(progress_label_), buf);
    }
}

void NgraphDialog::on_browse_sample(GtkButton*, gpointer data)
{
    NgraphDialog* self = static_cast<NgraphDialog*>(data);
    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Select Sample File",
        GTK_WINDOW(self->dialog_),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        nullptr);
    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
    {
        gchar* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (fname)
        {
            gtk_entry_set_text(GTK_ENTRY(self->sample_entry_), fname);
            g_free(fname);
        }
    }
    gtk_widget_destroy(dlg);
}

void NgraphDialog::on_browse_ngraph(GtkButton*, gpointer data)
{
    NgraphDialog* self = static_cast<NgraphDialog*>(data);
    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Save NGraph File As",
        GTK_WINDOW(self->dialog_),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",   GTK_RESPONSE_ACCEPT,
        nullptr);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg), TRUE);
    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
    {
        gchar* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (fname)
        {
            gtk_entry_set_text(GTK_ENTRY(self->ngraph_entry_), fname);
            g_free(fname);
        }
    }
    gtk_widget_destroy(dlg);
}

void* NgraphDialog::creator_thread_func(void* data)
{
    Ngraph_File_Creator* creator = static_cast<Ngraph_File_Creator*>(data);
    creator->go();
    return nullptr;
}

void NgraphDialog::on_create(GtkButton*, gpointer data)
{
    NgraphDialog* self = static_cast<NgraphDialog*>(data);

    const gchar* sample = gtk_entry_get_text(GTK_ENTRY(self->sample_entry_));
    const gchar* ngfile = gtk_entry_get_text(GTK_ENTRY(self->ngraph_entry_));
    int n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(self->n_spin_));
    int spaces = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(self->spaces_check_)) ? 1 : 0;

    if (!sample || !*sample || !ngfile || !*ngfile || n <= 0)
    {
        GtkWidget* err = gtk_message_dialog_new(
            GTK_WINDOW(self->dialog_), GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Please enter file names and graph length.");
        gtk_dialog_run(GTK_DIALOG(err));
        gtk_widget_destroy(err);
        return;
    }

    // Get sample file size
    struct stat statbuf;
    if (stat(sample, &statbuf) == 0)
        self->sample_size_ = statbuf.st_size;
    else
        self->sample_size_ = 1;

    delete self->creator_;
    self->creator_ = new Ngraph_File_Creator(sample, ngfile, n, spaces);
    self->creator_->set_callback(self);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(self->progress_bar_), 0.0);
    gtk_label_set_text(GTK_LABEL(self->progress_label_), "Starting…");
    gtk_widget_set_sensitive(self->done_btn_,   FALSE);
    gtk_widget_set_sensitive(self->create_btn_, FALSE);

    pthread_create(&self->creator_thread_, nullptr, creator_thread_func,
                   self->creator_);
}

void NgraphDialog::on_done(GtkButton*, gpointer data)
{
    NgraphDialog* self = static_cast<NgraphDialog*>(data);
    gtk_dialog_response(GTK_DIALOG(self->dialog_), GTK_RESPONSE_CLOSE);
}
