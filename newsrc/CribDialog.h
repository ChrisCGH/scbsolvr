// CribDialog.h - Crib dialog (GTK3 Linux port)
#ifndef __CRIB_DIALOG_H
#define __CRIB_DIALOG_H

#include <gtk/gtk.h>
#include "crib.h"
#include "mono_solver.h"

class CribDialog
{
public:
    CribDialog(GtkWindow* parent, const Crib& crib, bool preserve_punctuation);
    ~CribDialog();

    // Returns TRUE if OK was pressed
    bool run(Crib& result);

private:
    GtkWidget* dialog_;
    GtkWidget* cipher_label_;
    GtkWidget* crib_entry_;
    GtkWidget* plain_label_;
    GtkWidget* key_label_;

    Crib crib_;
    bool preserve_punctuation_;
    bool crib_signal_blocked_;

    void do_display();

    static void on_crib_changed(GtkEditable*, gpointer);
    static void on_clear(GtkButton*, gpointer);
    static void on_next_left(GtkButton*, gpointer);
    static void on_next_right(GtkButton*, gpointer);
};

#endif
