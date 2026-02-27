#ifndef __MONO_SOLVER_H
#define __MONO_SOLVER_H
#include <iostream>
#include <fstream>
#include "mono.h"
#include "scorer.h"
#include "top.h"
#include "fixed.h"
#include <cctype>
#include "callback.h"

class Mono_Solver
{
   public:
      Mono_Solver()
            : top_(10), max_top_(1),
            max_iterations_(0), ciphertext_(0), verbose_(0),
            terminate_(0), fixed_(0), outtext_(0), solver_callback_(0)
      {
      }
      ~Mono_Solver()
      {
         delete [] ciphertext_;
         delete [] outtext_;
      }

      int spaces_scored() const
      {
         return scorer_.spaces_scored();
      }

      void set_trigraph_scoring(const char* trifile)
      {
         scorer_.set_trigram_file(trifile);
      }
      void set_tetragraph_scoring(const char* tetrafile)
      {
         scorer_.set_tetragram_file(tetrafile);
      }
      void set_ngraph_scoring(const char* nfile)
      {
         scorer_.set_ngraph_file(nfile);
      }
      void set_scoring_file(const char* filename)
      {
         std::fstream file(filename, std::ios::in);

         char line[132];

         if (file.getline(line, 132))
         {
            // format is XXXXX : NNNNN PCT
            file.close();
            char* c = strchr(line, ':');
            switch (c - line)
            {
            case 4:
               set_trigraph_scoring(filename);
               break;
            case 5:
               set_tetragraph_scoring(filename);
               break;
            default:
               set_ngraph_scoring(filename);
               break;
            }
         }
      }

      // user callback function to allow results to be
      // passed back
      void set_solver_callback(Solver_Callback* solver_callback)
      {
         solver_callback_ = solver_callback;
      }

      void set_fixed(Fixed_Key* fixed)
      {
         fixed_ = fixed;
      }

      int solve()
      {
         //cout << ciphertext_ << endl;
         terminate_ = 0;
         int max_score = -1000000;
         int local_max_score = -1000000;
         int best_local_max_score = -1000000;
         int score = -1000000;
         int done = 0;
         int i = 0;
         char* outtext = new char [ strlen(ciphertext_) + 1 ];
         scorer_.reset();
         top_.clear();

         time_t start_time = time(0);

         Alphabet random("abcdefghijklmnopqrstuvwxyz");
         Alphabet best_local_max_alphabet;
         Alphabet local_max_alphabet;
         stuck_count_ = 0;

         Top top(max_top_);
         i = 0;
         while (i < max_top_)
         {
            random.randomise(fixed_);
            mono_.set_key(random.alphabet());

            mono_.decode(ciphertext_, outtext);
            score = scorer_.score(outtext);
            if (top.qualifies(score))
            {
               top.add(random, score);
               i++;
            }
         }

         while (!done)
         {
            Top top1(max_top_);
            for (int g = 0; g < top.size(); g++)
            {
               int local_max_found = 0;
               int local_max_score = -1000000;
               random = top[g];
               while (!local_max_found)
               {
                  random.start_swaps();
                  max_score = -1000000;
                  Alphabet best_alphabet;
                  while (!random.end_swaps())
                  {
                     mono_.set_key(random.alphabet());
                     mono_.decode(ciphertext_, outtext);
                     score = scorer_.score(outtext);
                     if (score > max_score)
                     {
                        max_score = score;
                        best_alphabet = random;
                        if (max_score > best_local_max_score)
                        {
                           best_local_max_score = max_score;
                           best_local_max_alphabet = best_alphabet;
                           stuck_count_ = 0;
                           long int secs = time(0) - start_time;
                           long int mins = secs / 60;
                           secs -= 60 * mins;
                           score_ = best_local_max_score;
                           if (verbose_)
                           {
                              std::cout << scorer_.get_scored_count() << " alphabets in ";
                              std::cout << mins << " minutes, " << secs << " seconds" << std::endl;
                              mono_.display();
                              std::cout << "Score = " << best_local_max_score << std::endl;
                              std::cout << outtext << std::endl;
                           }
                           if (solver_callback_)
                           {
                              solver_callback_->process_result(*this);
                           }
                        }
                     }

                     if (top1.qualifies(score))
                     {
                        top1.add(random, score);
                     }
                     // keep track of the absolute best scorers.
                     if (top_.qualifies(score))
                     {
                        top_.add(random, score);
                     }
                     random.next_swap(fixed_);
                  }
                  if (max_score > local_max_score)
                  {
                     local_max_score = max_score;
                     local_max_alphabet = best_alphabet;
                     random = best_alphabet;
                     mono_.set_key(random.alphabet());
                  }
                  else
                  {
                     stuck_count_++;
                     local_max_found = 1;
                  }
               }
            }
            if (max_iterations_ > 0 && scorer_.get_scored_count() > max_iterations_)
            {
               if (verbose_)
               {
                  long int secs = time(0) - start_time;
                  long int mins = secs / 60;
                  secs -= 60 * mins;
                  std::cout << scorer_.get_scored_count() << " alphabets in ";
                  std::cout << mins << " minutes, " << secs << " seconds" << std::endl;
                  mono_.set_key(best_local_max_alphabet.alphabet());
                  mono_.decode(ciphertext_, outtext);
                  mono_.display();
                  std::cout << "Score = " << best_local_max_score << std::endl;
                  std::cout << outtext << std::endl;
               }
               delete [] outtext;
               return 0;
            }
            if (solver_callback_)
            {
               solver_callback_->process_result(*this);
            }
            if (terminate_)
            {
               delete [] outtext;
               clear();
               return 0;
            }
            if (top == top1)
            {
               top.clear();
               i = 0;
               while (i < max_top_)
               {
                  random = best_local_max_alphabet;
                  random.randomise1(fixed_, stuck_count_);
                  mono_.set_key(random.alphabet());
                  mono_.decode(ciphertext_, outtext);
                  score = scorer_.score(outtext);
                  if (top.qualifies(score))
                  {
                     top.add(random, score);
                     i++;
                  }
               }
            }
            else
            {
               top = top1;
            }
         }
         delete [] outtext;
         return 0;
      }

      int keys_checked() const
      {
         return scorer_.get_scored_count();
      }

      void set_cipher_text(const char* ciphertext)
      {
         if (ciphertext_) delete [] ciphertext_;
         ciphertext_ = new char [ strlen(ciphertext) + 1 ];
         const char* c = ciphertext;
         char* d = ciphertext_;
         int space_last = 0;
         while (c && *c)
         {
            if (isalpha(*c))
            {
               *d++ = toupper(*c);
               space_last = 0;
            }
            else if (scorer_.spaces_scored() && !space_last && *c == ' ')
            {
               *d++ = 'z' + 1; // we represent space as 'z' + 1
               space_last = 1;
            }
            c++;
         }
         *d = '\0';
         if (outtext_) delete [] outtext_;
         outtext_ = new char [ strlen(ciphertext_) + 1 ];
      }

      int is_cipher_text_different(const char* new_ciphertext)
      {
         if (!ciphertext_) return 1;
         const char* d = new_ciphertext;
         char* c = ciphertext_;
         while (d && *d)
         {
            if (isalpha(*d))
            {
               if (toupper(*d) != *c) return 1;
               c++;
            }
            d++;
         }
         return 0;
      }

      void set_verbose()
      {
         verbose_ = 1;
      }
      void set_quiet()
      {
         verbose_ = 0;
      }
      void set_max_iterations(int max_iterations)
      {
         max_iterations_ = max_iterations;
         if (max_iterations_ < 0) max_iterations_ = 0;
      }
      void display_best(int index = 0)
      {
         if (index > 9) index = 9;
         if (index < 0) index = 0;
         Alphabet best = top_[index];
         Mono m;
         m.set_key(best.alphabet());
         m.decode(ciphertext_, outtext_);
         int score = top_.score(index);
         m.display();
         std::cout << "Score = " << score << std::endl;
         std::cout << outtext_ << std::endl;
      }
      const char* best_result(int index = 0) const
      {
         if (!ciphertext_) return 0;
         if (index > 9) index = 9;
         if (index < 0) index = 0;
         Alphabet best = top_[index];
         Mono m;
         m.set_key(best.alphabet());
         m.decode(ciphertext_, outtext_);
         int score = top_.score(index);
         return outtext_;
      }
      const char* best_key(int index = 0) const
      {
         if (index > 9) index = 9;
         if (index < 0) index = 0;
         Alphabet best = top_[index];
         Mono m;
         m.set_key(best.alphabet());
         return m.get_key();
      }
      void set_terminate()
      {
         // set flag for termination
         terminate_ = 1;
      }
      int get_terminate() const
      {
         return terminate_;
      }
      static double phi(const char* cipher)
      {
         int freq[26];
         int i = 0;
         for (i = 0; i < 26; i++) freq[i] = 0;
         const char* c = cipher;
         int total = 0;
         while (c && *c)
         {
            if (isalpha(*c))
            {
               freq[tolower(*c) - 'a']++;
               total++;
            }
            c++;
         }
         if (total <= 1) return 0.0;
         double phi_value = 0.0;
         for (i = 0; i < 26; i++)
         {
            phi_value += freq[i] * (freq[i] - 1);
         }
         phi_value /= total * (total - 1);
         return phi_value;
      }
      static char* reformat(const char* plain, const char* cipher)
      {
         static char* buffer = 0;
         static int buffer_len = 0;
         if (strlen(cipher) > buffer_len)
         {
            delete [] buffer;
            buffer_len = strlen(cipher);
            buffer = new char [ buffer_len + 1 ];
         }

         const char* c = cipher;
         const char* o = plain;
         char* b = buffer;

         while (c && *c)
         {
            if (isalpha(*c))
            {
               //if (isupper(*c)) *b = toupper(*o);
               //if (islower(*c)) *b = tolower(*o);
               *b = tolower(*o);
               o++;
            }
            else
            {
               *b = *c;
            }
            b++;
            c++;
         }
         *b = '\0';
         return buffer;
      }
      int stuck() const
      {
         return stuck_count_;
      }
      int score() const
      {
         return score_;
      }

   private:
      void clear()
      {
         fixed_ = 0;
         max_top_ = 1;
         max_iterations_ = 0;
         delete [] ciphertext_;
         ciphertext_ = 0;
         verbose_ = 0;
      }
      Scorer scorer_;
      Mono mono_;
      Top top_;
      Fixed_Key* fixed_;
      int max_top_;
      int max_iterations_;
      int verbose_;
      int terminate_;
      int stuck_count_;
      int score_;
      Solver_Callback* solver_callback_;
      char* ciphertext_;
      char* outtext_;
};
#endif
