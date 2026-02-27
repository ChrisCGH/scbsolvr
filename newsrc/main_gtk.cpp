// main_gtk.cpp - Entry point for Secret Code Breaker GTK3 Linux application
#include <gtk/gtk.h>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    MainWindow* window = new MainWindow();

    gtk_main(); // blocks until gtk_main_quit() is called

    // gtk_main() has returned; the GTK widget tree is still alive here.
    // Deleting window calls the C++ destructor (mutex cleanup, etc.).
    // The underlying GtkWindow is destroyed inside on_quit before gtk_main_quit().
    delete window;
    return 0;
}
