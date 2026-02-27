// FixKeyDialog.h - Fix key dialog (GTK3 Linux port)
#ifndef __FIX_KEY_DIALOG_H
#define __FIX_KEY_DIALOG_H

#include <gtk/gtk.h>
#include "fixed.h"

class FixKeyDialog
{
public:
    FixKeyDialog(GtkWindow* parent, const Fixed_Key& fixed);
    ~FixKeyDialog();

    // Returns TRUE if OK was pressed
    bool run(Fixed_Key& result);

private:
    GtkWidget* dialog_;
    GtkWidget* entries_[26]; // one entry per plaintext letter a-z

    Fixed_Key fixed_;

    static void on_entry_changed(GtkEditable*, gpointer);
    static void on_clear(GtkButton*, gpointer);

    void do_display();
    void on_entry_changed_impl(int index);
};

#endif
