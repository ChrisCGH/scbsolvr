// MainWindow.h - Main GTK3 window for Secret Code Breaker (Linux port)
#ifndef __MAIN_WINDOW_H
#define __MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <string>
#include "mono_solver.h"
#include "fixed.h"
#include "crib.h"
#include "callback.h"

class MainWindow : public Solver_Callback
{
public:
    MainWindow();
    ~MainWindow();

    // Solver_Callback implementation
    void process_result(const Mono_Solver& solver) override;

private:
    // Solver state
    enum SolverState { CLEARED = 0, STOPPED = 1, SOLVING = 2, INTERRUPTED = 3 };

    // GTK widgets
    GtkWidget* window_;
    GtkWidget* cipher_textview_;
    GtkWidget* plain_textview_;
    GtkWidget* key_textview_;
    GtkWidget* ngraph_entry_;
    GtkWidget* cipher_filename_label_;
    GtkWidget* phi_label_;
    GtkWidget* score_label_;
    GtkWidget* keys_checked_label_;

    GtkWidget* solve_btn_;
    GtkWidget* stop_btn_;
    GtkWidget* interrupt_btn_;
    GtkWidget* resume_btn_;
    GtkWidget* clear_btn_;
    GtkWidget* quit_btn_;

    GtkTextBuffer* cipher_buf_;
    GtkTextBuffer* plain_buf_;
    GtkTextBuffer* key_buf_;

    // Menu items that need enable/disable
    GtkWidget* file_open_item_;
    GtkWidget* file_save_item_;
    GtkWidget* set_ngraph_item_;
    GtkWidget* tools_fixkey_item_;
    GtkWidget* tools_cribbing_item_;
    GtkCheckMenuItem* preserve_format_item_;
    GtkCheckMenuItem* remove_punct_item_;

    // Solver state
    SolverState state_;
    Mono_Solver solver_;
    pthread_t solver_thread_;
    bool solver_thread_active_;

    // Thread-safe pending update data
    pthread_mutex_t update_mutex_;
    bool update_pending_;
    bool count_only_update_;
    std::string pending_plaintext_;
    std::string pending_key_;
    int pending_keys_checked_;
    int pending_score_;

    // Application data
    std::string ngraph_file_;
    std::string cipher_filename_;
    std::string solve_start_ciphertext_; // ciphertext snapshot at solve start (thread-safe read)
    bool preserve_original_format_;
    bool remove_punctuation_;
    bool ciphertext_changed_;
    bool cipher_signal_blocked_;
    Fixed_Key fixed_;
    Crib crib_;
    std::string working_directory_;

    // Helper methods
    void do_display();
    std::string get_cipher_text() const;
    void set_text_view(GtkTextBuffer* buf, const std::string& text);
    std::string get_text_view(GtkTextBuffer* buf) const;
    void do_stop();
    void apply_pending_update();
    static std::string remove_spaces_str(const std::string& s);
    static std::string remove_punctuation_str(const std::string& s);

    // GTK widget builders
    GtkWidget* create_menubar();
    GtkWidget* create_content_area();
    GtkWidget* create_button_bar();

    // Static signal handlers
    static void on_solve(GtkWidget*, gpointer);
    static void on_stop(GtkWidget*, gpointer);
    static void on_interrupt(GtkWidget*, gpointer);
    static void on_resume(GtkWidget*, gpointer);
    static void on_clear(GtkWidget*, gpointer);
    static void on_quit(GtkWidget*, gpointer);
    static void on_file_open(GtkMenuItem*, gpointer);
    static void on_file_save(GtkMenuItem*, gpointer);
    static void on_file_exit(GtkMenuItem*, gpointer);
    static void on_set_ngraph(GtkMenuItem*, gpointer);
    static void on_create_ngraph(GtkMenuItem*, gpointer);
    static void on_fix_key(GtkMenuItem*, gpointer);
    static void on_cribbing(GtkMenuItem*, gpointer);
    static void on_preserve_format(GtkCheckMenuItem*, gpointer);
    static void on_remove_punct(GtkCheckMenuItem*, gpointer);
    static void on_instructions(GtkMenuItem*, gpointer);
    static void on_theory(GtkMenuItem*, gpointer);
    static void on_about(GtkMenuItem*, gpointer);
    static void on_cipher_changed(GtkTextBuffer*, gpointer);
    static gboolean on_idle_update(gpointer);
    static gboolean on_delete_event(GtkWidget*, GdkEvent*, gpointer);
    static void* solver_thread_func(void*);
};

#endif
