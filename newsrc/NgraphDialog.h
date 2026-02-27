// NgraphDialog.h - NGraph file creator dialog (GTK3 Linux port)
#ifndef __NGRAPH_DIALOG_H
#define __NGRAPH_DIALOG_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <string>
#include "ngraph.h"
#include "callback.h"

class NgraphDialog : public Ngraph_File_Creator_Callback
{
public:
    NgraphDialog(GtkWindow* parent);
    ~NgraphDialog();

    void run();

    // Ngraph_File_Creator_Callback implementation
    void process_progress(const Ngraph_File_Creator& creator) override;

private:
    GtkWidget* dialog_;
    GtkWidget* sample_entry_;
    GtkWidget* ngraph_entry_;
    GtkWidget* n_spin_;
    GtkWidget* spaces_check_;
    GtkWidget* progress_bar_;
    GtkWidget* progress_label_;
    GtkWidget* create_btn_;
    GtkWidget* done_btn_;

    Ngraph_File_Creator* creator_;
    pthread_t creator_thread_;
    long int sample_size_;
    long int progress_;

    pthread_mutex_t progress_mutex_;
    bool update_pending_;

    static void on_browse_sample(GtkButton*, gpointer);
    static void on_browse_ngraph(GtkButton*, gpointer);
    static void on_create(GtkButton*, gpointer);
    static void on_done(GtkButton*, gpointer);
    static gboolean on_idle_progress(gpointer);
    static void* creator_thread_func(void*);

    void apply_progress_update();
};

#endif
