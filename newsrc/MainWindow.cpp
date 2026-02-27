// MainWindow.cpp - Main GTK3 window implementation (Linux port of Windows MFC scbDlg)
#include "MainWindow.h"
#include "NgraphDialog.h"
#include "FixKeyDialog.h"
#include "CribDialog.h"

#include <cstdio>
#include <cstring>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────────
// Construction / destruction
// ─────────────────────────────────────────────────────────────────────────────

MainWindow::MainWindow()
    : state_(CLEARED),
      solver_thread_active_(false),
      preserve_original_format_(true),
      remove_punctuation_(false),
      ciphertext_changed_(false),
      cipher_signal_blocked_(false),
      update_pending_(false),
      count_only_update_(false),
      pending_keys_checked_(0),
      pending_score_(0)
{
    pthread_mutex_init(&update_mutex_, nullptr);

    // Working directory
    char buf[4096];
    if (getcwd(buf, sizeof(buf)))
        working_directory_ = buf;

    // Default ngraph file in working directory
    ngraph_file_ = working_directory_ + "/english.tet";

    // ── Main window ──────────────────────────────────────────────────────────
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_), "Secret Code Breaker");
    gtk_window_set_default_size(GTK_WINDOW(window_), 900, 650);
    gtk_container_set_border_width(GTK_CONTAINER(window_), 4);

    g_signal_connect(window_, "delete-event", G_CALLBACK(on_delete_event), this);

    // ── Outer VBox ───────────────────────────────────────────────────────────
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(window_), vbox);

    gtk_box_pack_start(GTK_BOX(vbox), create_menubar(),      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), create_content_area(), TRUE,  TRUE,  4);
    gtk_box_pack_start(GTK_BOX(vbox), create_button_bar(),   FALSE, FALSE, 2);

    gtk_widget_show_all(window_);
    do_display();
}

MainWindow::~MainWindow()
{
    pthread_mutex_destroy(&update_mutex_);
}

// ─────────────────────────────────────────────────────────────────────────────
// Widget builders
// ─────────────────────────────────────────────────────────────────────────────

GtkWidget* MainWindow::create_menubar()
{
    GtkWidget* bar = gtk_menu_bar_new();

    // ── File menu ────────────────────────────────────────────────────────────
    GtkWidget* file_menu  = gtk_menu_new();
    GtkWidget* file_item  = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

    file_open_item_ = gtk_menu_item_new_with_mnemonic("_Open…");
    file_save_item_ = gtk_menu_item_new_with_mnemonic("_Save…");
    GtkWidget* file_exit  = gtk_menu_item_new_with_mnemonic("E_xit");

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_open_item_);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_save_item_);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_exit);

    g_signal_connect(file_open_item_, "activate", G_CALLBACK(on_file_open), this);
    g_signal_connect(file_save_item_, "activate", G_CALLBACK(on_file_save), this);
    g_signal_connect(file_exit,       "activate", G_CALLBACK(on_file_exit), this);

    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_item);

    // ── Tools menu ───────────────────────────────────────────────────────────
    GtkWidget* tools_menu = gtk_menu_new();
    GtkWidget* tools_item = gtk_menu_item_new_with_mnemonic("_Tools");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools_item), tools_menu);

    set_ngraph_item_    = gtk_menu_item_new_with_mnemonic("Set _NGraph File…");
    GtkWidget* create_ng = gtk_menu_item_new_with_mnemonic("_Create NGraph File…");
    tools_fixkey_item_  = gtk_menu_item_new_with_mnemonic("Fix _Key…");
    tools_cribbing_item_= gtk_menu_item_new_with_mnemonic("Cr_ibbing…");

    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), set_ngraph_item_);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), create_ng);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), tools_fixkey_item_);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), tools_cribbing_item_);

    g_signal_connect(set_ngraph_item_,    "activate", G_CALLBACK(on_set_ngraph),  this);
    g_signal_connect(create_ng,           "activate", G_CALLBACK(on_create_ngraph),this);
    g_signal_connect(tools_fixkey_item_,  "activate", G_CALLBACK(on_fix_key),     this);
    g_signal_connect(tools_cribbing_item_,"activate", G_CALLBACK(on_cribbing),    this);

    gtk_menu_shell_append(GTK_MENU_SHELL(bar), tools_item);

    // ── View menu ────────────────────────────────────────────────────────────
    GtkWidget* view_menu = gtk_menu_new();
    GtkWidget* view_item = gtk_menu_item_new_with_mnemonic("_View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);

    preserve_format_item_ = GTK_CHECK_MENU_ITEM(
        gtk_check_menu_item_new_with_mnemonic("_Preserve Original Format"));
    remove_punct_item_    = GTK_CHECK_MENU_ITEM(
        gtk_check_menu_item_new_with_mnemonic("_Remove Punctuation"));

    gtk_check_menu_item_set_active(preserve_format_item_, TRUE);
    gtk_check_menu_item_set_active(remove_punct_item_,    FALSE);

    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu),
                          GTK_WIDGET(preserve_format_item_));
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu),
                          GTK_WIDGET(remove_punct_item_));

    g_signal_connect(preserve_format_item_, "toggled",
                     G_CALLBACK(on_preserve_format), this);
    g_signal_connect(remove_punct_item_,    "toggled",
                     G_CALLBACK(on_remove_punct),    this);

    gtk_menu_shell_append(GTK_MENU_SHELL(bar), view_item);

    // ── Help menu ────────────────────────────────────────────────────────────
    GtkWidget* help_menu  = gtk_menu_new();
    GtkWidget* help_item  = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);

    GtkWidget* instr  = gtk_menu_item_new_with_mnemonic("_Instructions");
    GtkWidget* theory = gtk_menu_item_new_with_mnemonic("_Theory");
    GtkWidget* about  = gtk_menu_item_new_with_mnemonic("_About…");

    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), instr);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), theory);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about);

    g_signal_connect(instr,  "activate", G_CALLBACK(on_instructions), this);
    g_signal_connect(theory, "activate", G_CALLBACK(on_theory),       this);
    g_signal_connect(about,  "activate", G_CALLBACK(on_about),        this);

    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help_item);

    return bar;
}

GtkWidget* MainWindow::create_content_area()
{
    // Top-level hpaned: left (inputs) | right (outputs)
    GtkWidget* hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    // ── Left panel ───────────────────────────────────────────────────────────
    GtkWidget* left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(left_box), 4);

    // NGraph file row
    GtkWidget* ng_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget* ng_lbl  = gtk_label_new("NGraph file:");
    gtk_label_set_xalign(GTK_LABEL(ng_lbl), 0.0);
    ngraph_entry_ = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(ngraph_entry_), ngraph_file_.c_str());
    gtk_widget_set_hexpand(ngraph_entry_, TRUE);
    gtk_box_pack_start(GTK_BOX(ng_box), ng_lbl,      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ng_box), ngraph_entry_, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(left_box), ng_box, FALSE, FALSE, 0);

    // Cipher filename row
    GtkWidget* cf_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget* cf_lbl = gtk_label_new("Cipher file:");
    gtk_label_set_xalign(GTK_LABEL(cf_lbl), 0.0);
    cipher_filename_label_ = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(cipher_filename_label_), 0.0);
    gtk_widget_set_hexpand(cipher_filename_label_, TRUE);
    gtk_box_pack_start(GTK_BOX(cf_box), cf_lbl,               FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cf_box), cipher_filename_label_, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(left_box), cf_box, FALSE, FALSE, 0);

    // PHI / Score row
    GtkWidget* phi_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget* phi_lbl  = gtk_label_new("PHI:");
    phi_label_          = gtk_label_new("");
    GtkWidget* sc_lbl   = gtk_label_new("Score:");
    score_label_        = gtk_label_new("");
    GtkWidget* kc_lbl   = gtk_label_new("Keys checked:");
    keys_checked_label_ = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(phi_box), phi_lbl,           FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(phi_box), phi_label_,         FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(phi_box), sc_lbl,            FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(phi_box), score_label_,       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(phi_box), kc_lbl,            FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(phi_box), keys_checked_label_, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), phi_box, FALSE, FALSE, 0);

    // Cipher text
    GtkWidget* ct_lbl = gtk_label_new("Cipher text:");
    gtk_label_set_xalign(GTK_LABEL(ct_lbl), 0.0);
    gtk_box_pack_start(GTK_BOX(left_box), ct_lbl, FALSE, FALSE, 0);

    cipher_textview_ = gtk_text_view_new();
    cipher_buf_      = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cipher_textview_));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(cipher_textview_), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(cipher_textview_), TRUE);
    GtkWidget* ct_sw = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ct_sw),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(ct_sw), cipher_textview_);
    gtk_widget_set_size_request(ct_sw, -1, 120);
    gtk_box_pack_start(GTK_BOX(left_box), ct_sw, TRUE, TRUE, 0);

    g_signal_connect(cipher_buf_, "changed", G_CALLBACK(on_cipher_changed), this);

    gtk_paned_add1(GTK_PANED(hpaned), left_box);

    // ── Right panel ──────────────────────────────────────────────────────────
    GtkWidget* right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 4);

    // Plain text
    GtkWidget* pt_lbl = gtk_label_new("Plain text:");
    gtk_label_set_xalign(GTK_LABEL(pt_lbl), 0.0);
    gtk_box_pack_start(GTK_BOX(right_box), pt_lbl, FALSE, FALSE, 0);

    plain_textview_ = gtk_text_view_new();
    plain_buf_      = gtk_text_view_get_buffer(GTK_TEXT_VIEW(plain_textview_));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(plain_textview_), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(plain_textview_), TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(plain_textview_), FALSE);
    GtkWidget* pt_sw = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pt_sw),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(pt_sw), plain_textview_);
    gtk_widget_set_size_request(pt_sw, -1, 120);
    gtk_box_pack_start(GTK_BOX(right_box), pt_sw, TRUE, TRUE, 0);

    // Key text
    GtkWidget* key_lbl = gtk_label_new("Key:");
    gtk_label_set_xalign(GTK_LABEL(key_lbl), 0.0);
    gtk_box_pack_start(GTK_BOX(right_box), key_lbl, FALSE, FALSE, 0);

    key_textview_ = gtk_text_view_new();
    key_buf_      = gtk_text_view_get_buffer(GTK_TEXT_VIEW(key_textview_));
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(key_textview_), TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(key_textview_), FALSE);
    GtkWidget* key_sw = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(key_sw),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(key_sw), key_textview_);
    gtk_widget_set_size_request(key_sw, -1, 60);
    gtk_box_pack_start(GTK_BOX(right_box), key_sw, FALSE, FALSE, 0);

    gtk_paned_add2(GTK_PANED(hpaned), right_box);
    gtk_paned_set_position(GTK_PANED(hpaned), 450);

    return hpaned;
}

GtkWidget* MainWindow::create_button_bar()
{
    GtkWidget* bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
    gtk_box_set_spacing(GTK_BOX(bbox), 4);

    solve_btn_     = gtk_button_new_with_mnemonic("_Solve");
    stop_btn_      = gtk_button_new_with_mnemonic("S_top");
    interrupt_btn_ = gtk_button_new_with_mnemonic("_Interrupt");
    resume_btn_    = gtk_button_new_with_mnemonic("R_esume");
    clear_btn_     = gtk_button_new_with_mnemonic("_Clear");
    quit_btn_      = gtk_button_new_with_mnemonic("_Quit");

    for (GtkWidget* b : {solve_btn_, stop_btn_, interrupt_btn_,
                         resume_btn_, clear_btn_, quit_btn_})
        gtk_container_add(GTK_CONTAINER(bbox), b);

    g_signal_connect(solve_btn_,     "clicked", G_CALLBACK(on_solve),     this);
    g_signal_connect(stop_btn_,      "clicked", G_CALLBACK(on_stop),      this);
    g_signal_connect(interrupt_btn_, "clicked", G_CALLBACK(on_interrupt), this);
    g_signal_connect(resume_btn_,    "clicked", G_CALLBACK(on_resume),    this);
    g_signal_connect(clear_btn_,     "clicked", G_CALLBACK(on_clear),     this);
    g_signal_connect(quit_btn_,      "clicked", G_CALLBACK(on_quit),      this);

    return bbox;
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper utilities
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::set_text_view(GtkTextBuffer* buf, const std::string& text)
{
    gtk_text_buffer_set_text(buf, text.c_str(), -1);
}

std::string MainWindow::get_text_view(GtkTextBuffer* buf) const
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);
    gchar* raw = gtk_text_buffer_get_text(buf, &start, &end, FALSE);
    std::string s(raw ? raw : "");
    g_free(raw);
    return s;
}

std::string MainWindow::get_cipher_text() const
{
    return get_text_view(cipher_buf_);
}

std::string MainWindow::remove_spaces_str(const std::string& s)
{
    std::string out;
    for (char c : s)
        if (c != static_cast<char>('z' + 1))
            out += c;
    return out;
}

std::string MainWindow::remove_punctuation_str(const std::string& s)
{
    std::string out;
    for (char c : s)
        if (isalpha(static_cast<unsigned char>(c)))
            out += c;
    return out;
}

void MainWindow::do_display()
{
    switch (state_)
    {
    case CLEARED:
        gtk_widget_set_sensitive(solve_btn_,     FALSE);
        gtk_widget_set_sensitive(stop_btn_,      FALSE);
        gtk_widget_set_sensitive(interrupt_btn_, FALSE);
        gtk_widget_set_sensitive(resume_btn_,    FALSE);
        gtk_widget_set_sensitive(clear_btn_,     FALSE);
        gtk_widget_set_sensitive(cipher_textview_, TRUE);
        break;
    case STOPPED:
        gtk_widget_set_sensitive(solve_btn_,     TRUE);
        gtk_widget_set_sensitive(stop_btn_,      FALSE);
        gtk_widget_set_sensitive(interrupt_btn_, FALSE);
        gtk_widget_set_sensitive(resume_btn_,    FALSE);
        gtk_widget_set_sensitive(clear_btn_,     TRUE);
        gtk_widget_set_sensitive(cipher_textview_, TRUE);
        break;
    case SOLVING:
        gtk_widget_set_sensitive(solve_btn_,     FALSE);
        gtk_widget_set_sensitive(stop_btn_,      TRUE);
        gtk_widget_set_sensitive(interrupt_btn_, TRUE);
        gtk_widget_set_sensitive(resume_btn_,    FALSE);
        gtk_widget_set_sensitive(clear_btn_,     TRUE);
        gtk_widget_set_sensitive(cipher_textview_, FALSE);
        break;
    case INTERRUPTED:
        gtk_widget_set_sensitive(solve_btn_,     FALSE);
        gtk_widget_set_sensitive(stop_btn_,      TRUE);
        gtk_widget_set_sensitive(interrupt_btn_, FALSE);
        gtk_widget_set_sensitive(resume_btn_,    TRUE);
        gtk_widget_set_sensitive(clear_btn_,     TRUE);
        gtk_widget_set_sensitive(cipher_textview_, TRUE);
        break;
    }

    // Menu items
    bool can_open = (state_ == STOPPED || state_ == CLEARED);
    bool can_save = ((state_ == STOPPED) || state_ == INTERRUPTED);
    bool can_fixkey  = (state_ != SOLVING && state_ != INTERRUPTED);
    bool can_crib    = (state_ == STOPPED);

    gtk_widget_set_sensitive(file_open_item_,    can_open);
    gtk_widget_set_sensitive(file_save_item_,    can_save);
    gtk_widget_set_sensitive(set_ngraph_item_,   can_open);
    gtk_widget_set_sensitive(tools_fixkey_item_, can_fixkey);
    gtk_widget_set_sensitive(tools_cribbing_item_, can_crib);
}

void MainWindow::do_stop()
{
    if (solver_thread_active_)
    {
        solver_.set_terminate();
        pthread_join(solver_thread_, nullptr);
        solver_thread_active_ = false;
    }

    const char* res = solver_.best_result(0);
    std::string plain = res ? res : "";
    if (solver_.spaces_scored())
        plain = remove_spaces_str(plain);
    if (preserve_original_format_)
    {
        std::string ct = get_cipher_text();
        const char* ref = Mono_Solver::reformat(plain.c_str(), ct.c_str());
        plain = ref ? ref : "";
    }
    set_text_view(plain_buf_, plain);

    std::string key_display = solver_.best_key(0);
    key_display += "\nabcdefghijklmnopqrstuvwxyz";
    set_text_view(key_buf_, key_display);
}

// ─────────────────────────────────────────────────────────────────────────────
// Solver_Callback – called from solver background thread
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::process_result(const Mono_Solver& solver)
{
    if (solver.get_terminate()) return;

    const char* res = solver.best_result(0);
    std::string plain = res ? res : "";
    if (solver.spaces_scored())
        plain = remove_spaces_str(plain);

    // Note: preserve_original_format_ and get_cipher_text() are read from the
    // solver thread. preserve_original_format_ is a plain bool only written on
    // the main thread, so a torn read is safe (worst case is a stale value for
    // one iteration). We use solve_start_ciphertext_ which is written once
    // before the thread starts and only read here, so no lock is needed.
    if (preserve_original_format_ && !solve_start_ciphertext_.empty())
    {
        const char* ref = Mono_Solver::reformat(plain.c_str(),
                                                solve_start_ciphertext_.c_str());
        plain = ref ? ref : "";
    }

    int new_keys = solver.keys_checked();

    pthread_mutex_lock(&update_mutex_);
    bool need_idle = !update_pending_;
    if (plain != pending_plaintext_ || new_keys - pending_keys_checked_ > 100000)
    {
        pending_plaintext_    = plain;
        pending_key_          = solver.best_key(0);
        pending_keys_checked_ = new_keys;
        pending_score_        = solver.score();
        count_only_update_    = (plain == pending_plaintext_);
        if (need_idle)
        {
            update_pending_ = true;
            g_idle_add(on_idle_update, this);
        }
    }
    pthread_mutex_unlock(&update_mutex_);
}

// Runs on main thread
gboolean MainWindow::on_idle_update(gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    self->apply_pending_update();
    return G_SOURCE_REMOVE;
}

void MainWindow::apply_pending_update()
{
    pthread_mutex_lock(&update_mutex_);
    bool count_only       = count_only_update_;
    std::string plain     = pending_plaintext_;
    std::string key       = pending_key_;
    int keys              = pending_keys_checked_;
    int score             = pending_score_;
    update_pending_       = false;
    pthread_mutex_unlock(&update_mutex_);

    // Update keys-checked label always
    char buf[64];
    snprintf(buf, sizeof(buf), "%d", keys);
    gtk_label_set_text(GTK_LABEL(keys_checked_label_), buf);

    if (!count_only)
    {
        set_text_view(plain_buf_, plain);

        std::string key_display = key + "\nabcdefghijklmnopqrstuvwxyz";
        set_text_view(key_buf_, key_display);

        snprintf(buf, sizeof(buf), "%d", score);
        gtk_label_set_text(GTK_LABEL(score_label_), buf);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Solver thread
// ─────────────────────────────────────────────────────────────────────────────

void* MainWindow::solver_thread_func(void* data)
{
    Mono_Solver* solver = static_cast<Mono_Solver*>(data);
    solver->solve();
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Button signal handlers
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::on_solve(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    const char* ngfile = gtk_entry_get_text(GTK_ENTRY(self->ngraph_entry_));
    struct stat statbuf;
    if (stat(ngfile, &statbuf) < 0)
    {
        char msg[512];
        snprintf(msg, sizeof(msg), "NGraph file '%s' does not exist.", ngfile);
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(self->window_),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
    }
    self->ngraph_file_ = ngfile;

    // Clear output
    self->set_text_view(self->plain_buf_, "");
    self->set_text_view(self->key_buf_,   "");
    gtk_label_set_text(GTK_LABEL(self->keys_checked_label_), "0");
    gtk_label_set_text(GTK_LABEL(self->score_label_),        "");
    self->state_ = SOLVING;
    self->do_display();

    {
        // Wait cursor while loading scoring file
        GdkCursor* wait_cursor = gdk_cursor_new_for_display(
            gdk_display_get_default(), GDK_WATCH);
        gdk_window_set_cursor(gtk_widget_get_window(self->window_), wait_cursor);
        g_object_unref(wait_cursor);
        self->solver_.set_scoring_file(self->ngraph_file_.c_str());
        gdk_window_set_cursor(gtk_widget_get_window(self->window_), nullptr);
    }

    std::string ct = self->get_cipher_text();
    self->solve_start_ciphertext_ = ct;
    self->solver_.set_cipher_text(ct.c_str());

    if (self->fixed_.number_fixed() > 0)
    {
        if (self->fixed_.check() < 0)
        {
            GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(self->window_),
                GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                "Fixed key is invalid.");
            gtk_dialog_run(GTK_DIALOG(dlg));
            gtk_widget_destroy(dlg);
            self->solver_.set_fixed(nullptr);
        }
        else
        {
            self->solver_.set_fixed(&self->fixed_);
        }
    }
    else
    {
        self->solver_.set_fixed(nullptr);
    }

    self->solver_.set_verbose();
    self->solver_.set_solver_callback(self);

    pthread_create(&self->solver_thread_, nullptr, solver_thread_func, &self->solver_);
    self->solver_thread_active_ = true;
}

void MainWindow::on_stop(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    self->do_stop();
    self->state_ = STOPPED;
    self->do_display();
}

void MainWindow::on_interrupt(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    // Suspend-equivalent: set a flag the solver can notice, then collect result
    // (pthread has no suspend; we set terminate and rejoin, then re-launch on resume)
    // For a proper interrupt we stop the thread and allow resume to restart it.
    // Store state so resume can re-launch.
    if (self->solver_thread_active_)
    {
        self->solver_.set_terminate();
        pthread_join(self->solver_thread_, nullptr);
        self->solver_thread_active_ = false;
    }

    const char* res = self->solver_.best_result(0);
    std::string plain = res ? res : "";
    if (self->solver_.spaces_scored())
        plain = remove_spaces_str(plain);
    if (self->preserve_original_format_)
    {
        std::string ct = self->get_cipher_text();
        const char* ref = Mono_Solver::reformat(plain.c_str(), ct.c_str());
        plain = ref ? ref : "";
    }
    self->set_text_view(self->plain_buf_, plain);

    self->state_ = INTERRUPTED;
    self->do_display();
}

void MainWindow::on_resume(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    // Re-launch the solver from where it is (it will restart; no true resume)
    self->state_ = SOLVING;
    self->do_display();

    self->solver_.set_solver_callback(self);
    pthread_create(&self->solver_thread_, nullptr, solver_thread_func, &self->solver_);
    self->solver_thread_active_ = true;
}

void MainWindow::on_clear(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    self->do_stop();

    // Block cipher-changed signal to prevent re-triggering do_stop
    self->cipher_signal_blocked_ = true;
    self->set_text_view(self->cipher_buf_, "");
    self->cipher_signal_blocked_ = false;

    self->set_text_view(self->plain_buf_, "");
    self->set_text_view(self->key_buf_,   "");
    gtk_label_set_text(GTK_LABEL(self->cipher_filename_label_), "");
    gtk_label_set_text(GTK_LABEL(self->phi_label_),             "");
    gtk_label_set_text(GTK_LABEL(self->score_label_),           "");
    gtk_label_set_text(GTK_LABEL(self->keys_checked_label_),    "0");

    self->cipher_filename_    = "";
    self->ciphertext_changed_ = false;
    self->preserve_original_format_ = true;
    self->remove_punctuation_ = false;
    gtk_check_menu_item_set_active(self->preserve_format_item_, TRUE);
    gtk_check_menu_item_set_active(self->remove_punct_item_,    FALSE);
    self->fixed_.clear();
    self->crib_.clear();
    self->state_ = CLEARED;
    self->do_display();
}

void MainWindow::on_quit(GtkWidget*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    if (self->solver_thread_active_)
    {
        self->solver_.set_terminate();
        pthread_join(self->solver_thread_, nullptr);
        self->solver_thread_active_ = false;
    }
    // Destroy the GTK window widget now so the destructor doesn't double-destroy it.
    gtk_widget_destroy(self->window_);
    self->window_ = nullptr;
    gtk_main_quit();
}

gboolean MainWindow::on_delete_event(GtkWidget*, GdkEvent*, gpointer data)
{
    on_quit(nullptr, data);
    return TRUE; // prevent default destroy (gtk_main_quit handles it)
}

// ─────────────────────────────────────────────────────────────────────────────
// Menu signal handlers
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::on_file_open(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Open Cipher File",
        GTK_WINDOW(self->window_),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        nullptr);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
    {
        gchar* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (fname)
        {
            std::ifstream file(fname);
            std::string ciphertext;
            std::string line;
            while (std::getline(file, line))
                ciphertext += line;

            if (self->remove_punctuation_)
                ciphertext = remove_punctuation_str(ciphertext);

            self->cipher_signal_blocked_ = true;
            self->set_text_view(self->cipher_buf_, ciphertext);
            self->cipher_signal_blocked_ = false;

            self->cipher_filename_    = fname;
            self->ciphertext_changed_ = false;

            gtk_label_set_text(GTK_LABEL(self->cipher_filename_label_), fname);

            char phi_buf[32];
            snprintf(phi_buf, sizeof(phi_buf), "%6.4f",
                     Mono_Solver::phi(ciphertext.c_str()));
            gtk_label_set_text(GTK_LABEL(self->phi_label_), phi_buf);

            self->set_text_view(self->plain_buf_, "");
            self->set_text_view(self->key_buf_,   "");
            gtk_label_set_text(GTK_LABEL(self->keys_checked_label_), "0");
            gtk_label_set_text(GTK_LABEL(self->score_label_), "");

            self->state_ = STOPPED;
            g_free(fname);
        }
    }
    gtk_widget_destroy(dlg);
    self->do_display();
}

void MainWindow::on_file_save(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Save Results",
        GTK_WINDOW(self->window_),
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
            std::ofstream file(fname);
            file << "Secret Code Breaker Monoalphabetic Substitution Solver\n";
            file << "Copyright (C) 2001 Chris Card  All rights reserved.\n";
            file << "------------------------------------------------------\n";
            file << "Ciphertext:\n" << self->get_cipher_text() << "\n";
            file << "Plaintext:\n"  << self->get_text_view(self->plain_buf_) << "\n";
            file << "Key:\n"        << self->get_text_view(self->key_buf_)   << "\n";
            file.close();
            g_free(fname);
        }
    }
    gtk_widget_destroy(dlg);
    self->do_display();
}

void MainWindow::on_file_exit(GtkMenuItem*, gpointer data)
{
    on_quit(nullptr, data);
}

void MainWindow::on_set_ngraph(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Select NGraph File",
        GTK_WINDOW(self->window_),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Select", GTK_RESPONSE_ACCEPT,
        nullptr);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
    {
        gchar* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (fname)
        {
            self->ngraph_file_ = fname;
            gtk_entry_set_text(GTK_ENTRY(self->ngraph_entry_), fname);
            g_free(fname);
        }
    }
    gtk_widget_destroy(dlg);
    self->do_display();
}

void MainWindow::on_create_ngraph(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    NgraphDialog ng_dlg(GTK_WINDOW(self->window_));
    ng_dlg.run();
}

void MainWindow::on_fix_key(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    FixKeyDialog dlg(GTK_WINDOW(self->window_), self->fixed_);
    Fixed_Key result;
    if (dlg.run(result))
        self->fixed_ = result;
}

void MainWindow::on_cribbing(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);

    std::string ct = self->get_cipher_text();
    if (self->crib_.ct_has_changed(ct.c_str()))
        self->crib_ = Crib(ct.c_str(), "");

    CribDialog dlg(GTK_WINDOW(self->window_), self->crib_,
                   self->preserve_original_format_);
    Crib result;
    if (dlg.run(result))
    {
        self->crib_  = result;
        self->fixed_ = result.get_fixed_key();
    }
}

void MainWindow::on_preserve_format(GtkCheckMenuItem* item, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    self->preserve_original_format_ =
        gtk_check_menu_item_get_active(item) ? true : false;
}

void MainWindow::on_remove_punct(GtkCheckMenuItem* item, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    bool active = gtk_check_menu_item_get_active(item);
    self->remove_punctuation_ = active;
    if (active)
    {
        std::string ct = self->get_cipher_text();
        if (!ct.empty())
        {
            ct = remove_punctuation_str(ct);
            self->cipher_signal_blocked_ = true;
            self->set_text_view(self->cipher_buf_, ct);
            self->cipher_signal_blocked_ = false;
        }
    }
}

void MainWindow::on_instructions(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    std::string path = self->working_directory_ + "/instruct.txt";

    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) < 0)
    {
        char msg[512];
        snprintf(msg, sizeof(msg), "Instructions file '%s' does not exist.",
                 path.c_str());
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(self->window_),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
    }

    // Open with default text viewer (xdg-open)
    {
        gchar* argv[] = { (gchar*)"xdg-open", (gchar*)path.c_str(), nullptr };
        GError* err = nullptr;
        g_spawn_async(nullptr, argv, nullptr,
                      G_SPAWN_SEARCH_PATH, nullptr, nullptr, nullptr, &err);
        if (err) g_error_free(err);
    }
}

void MainWindow::on_theory(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    std::string path = self->working_directory_ + "/theory.txt";

    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) < 0)
    {
        char msg[512];
        snprintf(msg, sizeof(msg), "Theory file '%s' does not exist.",
                 path.c_str());
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(self->window_),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
    }

    // Open with default text viewer (xdg-open)
    {
        gchar* argv[] = { (gchar*)"xdg-open", (gchar*)path.c_str(), nullptr };
        GError* err = nullptr;
        g_spawn_async(nullptr, argv, nullptr,
                      G_SPAWN_SEARCH_PATH, nullptr, nullptr, nullptr, &err);
        if (err) g_error_free(err);
    }
}

void MainWindow::on_about(GtkMenuItem*, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    GtkWidget* dlg = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dlg),
                                      "Secret Code Breaker");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dlg), "2.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dlg),
        "Copyright (C) 2001 Chris Card. All rights reserved.");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dlg),
        "Monoalphabetic substitution cipher solver.\n"
        "Linux GTK3 port.");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    (void)self;
}

// ─────────────────────────────────────────────────────────────────────────────
// Cipher text changed
// ─────────────────────────────────────────────────────────────────────────────

void MainWindow::on_cipher_changed(GtkTextBuffer* buf, gpointer data)
{
    MainWindow* self = static_cast<MainWindow*>(data);
    if (self->cipher_signal_blocked_) return;

    std::string ct = self->get_cipher_text();
    if (!self->ciphertext_changed_)
    {
        self->ciphertext_changed_ = true;
        std::string fn = self->cipher_filename_ + "(changed)";
        gtk_label_set_text(GTK_LABEL(self->cipher_filename_label_), fn.c_str());
    }
    char phi_buf[32];
    snprintf(phi_buf, sizeof(phi_buf), "%6.4f", Mono_Solver::phi(ct.c_str()));
    gtk_label_set_text(GTK_LABEL(self->phi_label_), phi_buf);

    if (self->solver_.is_cipher_text_different(ct.c_str()))
    {
        self->do_stop();
        self->state_ = STOPPED;
    }
    self->do_display();
    (void)buf;
}
